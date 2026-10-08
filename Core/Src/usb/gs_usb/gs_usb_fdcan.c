#include "gs_usb_fdcan.h"

#define MESSAGE_RAM_RX_FIFO0_ELEMENTS (20)
#define MESSAGE_RAM_TX_FIFO_ELEMENTS (16)
#define MESSAGE_RAM_TX_EVENT_ELEMENTS (16)

#define SOFTWARE_VERSION (1)
#define HARDWARE_VERSION (1)

static uint32_t clock_divider_to_value(uint32_t clock_divider) {
    // CKDIV encoding: 0 means /1, any other value n means /(2n)
    return clock_divider == 0 ? 1 : 2 * clock_divider;
}

struct GsCanCapabilities fdcan_get_capabilities(void) {
    // The clock calibration unit is shared, so channel 0's divider is the divider for all of them
    const uint32_t kernel_clock = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN);
    const uint32_t divider = clock_divider_to_value((FDCAN_CCU->CCFG & FDCANCCU_CCFG_CDIV_Msk) >> FDCANCCU_CCFG_CDIV_Pos);

    return (struct GsCanCapabilities){
        .clock_frequency = kernel_clock / divider,
        .features = GS_USB_FEATURE_LISTEN_ONLY | GS_USB_FEATURE_LOOP_BACK |
                    GS_USB_FEATURE_FD | GS_USB_FEATURE_BIT_TIMING_CONSTANTS_EXTENDED,
        .nominal_min = { .bitrate_prescaler = 1, .timing_segment1 = 1, .timing_segment2 = 1, .sync_jump_width = 1 },
        .nominal_max = { .bitrate_prescaler = 512, .timing_segment1 = 256, .timing_segment2 = 128, .sync_jump_width = 128 },
        .nominal_bitrate_prescaler_increment = 1,
        .data_min = { .bitrate_prescaler = 1, .timing_segment1 = 1, .timing_segment2 = 1, .sync_jump_width = 1 },
        .data_max = { .bitrate_prescaler = 32, .timing_segment1 = 32, .timing_segment2 = 16, .sync_jump_width = 16 },
        .data_bitrate_prescaler_increment = 1,
        .channel_count = FDCAN_PORT_CHANNEL_COUNT,
        .software_version = SOFTWARE_VERSION,
        .hardware_version = HARDWARE_VERSION,
    };
}

static uint32_t operating_mode_from_flags(uint32_t mode_flags) {
    if (mode_flags & GS_USB_FEATURE_LOOP_BACK)
        return FDCAN_MODE_INTERNAL_LOOPBACK; // frames never reach the pins
    if (mode_flags & GS_USB_FEATURE_LISTEN_ONLY)
        return FDCAN_MODE_BUS_MONITORING;
    return FDCAN_MODE_NORMAL;
}

static bool fdcan_start(void *context, uint8_t channel, const struct GsCanChannelConfiguration *configuration) {
    const struct GsUsbFdcanPort *port = context;
    const struct GsUsbFdcanPortChannel *port_channel = &port->channels[channel];
    FDCAN_HandleTypeDef *hfdcan = port_channel->hfdcan;
    FDCAN_InitTypeDef *init = &hfdcan->Init;

    // ClockDivider, AutoRetransmission, etc. keep whatever CubeMX generated
    const bool fd_enabled = (configuration->mode_flags & GS_USB_FEATURE_FD) != 0;
    init->Mode = operating_mode_from_flags(configuration->mode_flags);
    init->FrameFormat = fd_enabled ? FDCAN_FRAME_FD_BRS : FDCAN_FRAME_CLASSIC;
    init->AutoRetransmission = ENABLE;
    init->TransmitPause = DISABLE;
    init->ProtocolException = ENABLE;

    init->NominalPrescaler = configuration->nominal.bitrate_prescaler;
    init->NominalTimeSeg1 = configuration->nominal.timing_segment1;
    init->NominalTimeSeg2 = configuration->nominal.timing_segment2;
    init->NominalSyncJumpWidth = configuration->nominal.sync_jump_width;

    // The core only validates the data timing when FD is on, so don't load garbage otherwise
    init->DataPrescaler = fd_enabled ? configuration->data.bitrate_prescaler : 1;
    init->DataTimeSeg1 = fd_enabled ? configuration->data.timing_segment1 : 1;
    init->DataTimeSeg2 = fd_enabled ? configuration->data.timing_segment2 : 1;
    init->DataSyncJumpWidth = fd_enabled ? configuration->data.sync_jump_width : 1;

    // Identical layout for every channel, so it never depends on which ones are started
    init->MessageRAMOffset = port_channel->message_ram_offset;
    init->StdFiltersNbr = 0;
    init->ExtFiltersNbr = 0;
    init->RxFifo0ElmtsNbr = MESSAGE_RAM_RX_FIFO0_ELEMENTS;
    init->RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
    init->RxFifo1ElmtsNbr = 0;
    init->RxBuffersNbr = 0;
    init->TxEventsNbr = MESSAGE_RAM_TX_EVENT_ELEMENTS;
    init->TxBuffersNbr = 0;
    init->TxFifoQueueElmtsNbr = MESSAGE_RAM_TX_FIFO_ELEMENTS;
    init->TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    init->TxElmtSize = FDCAN_DATA_BYTES_64;

    if (HAL_FDCAN_Init(hfdcan) != HAL_OK)
        return false;

    // No hardware filters: everything goes to RX FIFO 0
    if (HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
        return false;

    return HAL_FDCAN_Start(hfdcan) == HAL_OK;
}

static void fdcan_stop(void *context, uint8_t channel) {
    const struct GsUsbFdcanPort *port = context;

    // Not DeInit: the clock calibration unit is shared between the instances
    HAL_FDCAN_Stop(port->channels[channel].hfdcan);
}

//--------------------------------------------------------------------
// Data: the hardware FIFOs in message RAM are the queues
//--------------------------------------------------------------------
static bool fdcan_transmit(void *context, uint8_t channel, const struct GsCanFrame *frame, uint8_t cookie) {
    const struct GsUsbFdcanPort *port = context;
    FDCAN_HandleTypeDef *handle = port->channels[channel].hfdcan;

    if (HAL_FDCAN_GetTxFifoFreeLevel(handle) == 0)
        return false;

    FDCAN_TxHeaderTypeDef header = {
        .Identifier = frame->identifier,
        .IdType = (frame->flags & GS_CAN_FRAME_FLAG_EXTENDED) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        .TxFrameType = (frame->flags & GS_CAN_FRAME_FLAG_REMOTE) ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME,
        .DataLength = frame->dlc & 0x0F, // HAL DLC constants are the code shifted by 16
        .ErrorStateIndicator = (frame->flags & GS_CAN_FRAME_FLAG_ERROR_STATE) ? FDCAN_ESI_PASSIVE : FDCAN_ESI_ACTIVE,
        .BitRateSwitch = (frame->flags & GS_CAN_FRAME_FLAG_BIT_RATE_SWITCH) ? FDCAN_BRS_ON : FDCAN_BRS_OFF,
        .FDFormat = (frame->flags & GS_CAN_FRAME_FLAG_FD) ? FDCAN_FD_CAN : FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_STORE_TX_EVENTS,
        .MessageMarker = cookie,
    };

    // A rejected frame is dropped, not retried, so one bad frame can't block the queue forever
    (void)HAL_FDCAN_AddMessageToTxFifoQ(handle, &header, (uint8_t *)frame->data);
    return true;
}

static bool fdcan_receive(void *context, uint8_t channel, struct GsCanFrame *frame) {
    const struct GsUsbFdcanPort *port = context;
    FDCAN_HandleTypeDef *handle = port->channels[channel].hfdcan;

    if (HAL_FDCAN_GetRxFifoFillLevel(handle, FDCAN_RX_FIFO0) == 0)
        return false;

    FDCAN_RxHeaderTypeDef header;
    if (HAL_FDCAN_GetRxMessage(handle, FDCAN_RX_FIFO0, &header, frame->data) != HAL_OK)
        return false;

    frame->identifier = header.Identifier;
    frame->dlc = (uint8_t)(header.DataLength);
    frame->flags = 0;
    if (header.IdType == FDCAN_EXTENDED_ID)
        frame->flags |= GS_CAN_FRAME_FLAG_EXTENDED;
    if (header.RxFrameType == FDCAN_REMOTE_FRAME)
        frame->flags |= GS_CAN_FRAME_FLAG_REMOTE;
    if (header.FDFormat == FDCAN_FD_CAN)
        frame->flags |= GS_CAN_FRAME_FLAG_FD;
    if (header.BitRateSwitch == FDCAN_BRS_ON)
        frame->flags |= GS_CAN_FRAME_FLAG_BIT_RATE_SWITCH;
    if (header.ErrorStateIndicator == FDCAN_ESI_PASSIVE)
        frame->flags |= GS_CAN_FRAME_FLAG_ERROR_STATE;
    return true;
}

static bool fdcan_transmitted(void *context, uint8_t channel, uint8_t *cookie) {
    const struct GsUsbFdcanPort *port = context;
    FDCAN_HandleTypeDef *handle = port->channels[channel].hfdcan;

    // Check the fill level first so an empty FIFO doesn't set ErrorCode on every poll
    if ((handle->Instance->TXEFS & FDCAN_TXEFS_EFFL) == 0)
        return false;

    FDCAN_TxEventFifoTypeDef event;
    if (HAL_FDCAN_GetTxEvent(handle, &event) != HAL_OK)
        return false;
    *cookie = (uint8_t)event.MessageMarker;
    return true;
}

static const struct GsCanOperations fdcan_operations = {
    .start = fdcan_start,
    .stop = fdcan_stop,
    .transmit = fdcan_transmit,
    .receive = fdcan_receive,
    .transmitted = fdcan_transmitted,
};

const struct GsCanOperations *fdcan_get_operations(void) {
    return &fdcan_operations;
}
