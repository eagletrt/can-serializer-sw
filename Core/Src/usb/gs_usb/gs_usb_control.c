#include <string.h>

#include "gs_usb_core.h"

void gs_usb_core_init(struct GsUsbCore *core, const struct GsUsbCapabilities *capabilities, const struct GsUsbOperations *can_operations, void *can_context) {
    memset(core, 0, sizeof *core);
    core->capabilities = *capabilities;
    if (core->capabilities.channel_count > GS_USB_CHANNEL_COUNT_MAX)
        core->capabilities.channel_count = GS_USB_CHANNEL_COUNT_MAX;
    core->can_operations = can_operations;
    core->can_context = can_context;
}

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
static bool prv_channel_is_valid(const struct GsUsbCore *core, uint16_t channel) {
    return channel < core->capabilities.channel_count;
}

static struct GsUsbTiming prv_timing_from_wire(const struct GsUsbBitTiming *wire) {
    // The host controls both values, so saturate instead of wrapping around
    uint32_t timing_segment1 = wire->propagation_segment + wire->phase_segment1;
    if (timing_segment1 < wire->propagation_segment)
        timing_segment1 = UINT32_MAX;

    return (struct GsUsbTiming){
        .bitrate_prescaler = wire->bitrate_prescaler,
        .timing_segment1 = timing_segment1,
        .timing_segment2 = wire->phase_segment2,
        .sync_jump_width = wire->sync_jump_width,
    };
}

static bool prv_timing_is_valid(const struct GsUsbTiming *timing, const struct GsUsbTiming *minimum, const struct GsUsbTiming *maximum, uint32_t prescaler_increment) {
    if (prescaler_increment == 0)
        prescaler_increment = 1;

    if (timing->bitrate_prescaler < minimum->bitrate_prescaler ||
        timing->bitrate_prescaler > maximum->bitrate_prescaler)
        return false;
    if ((timing->bitrate_prescaler - minimum->bitrate_prescaler) % prescaler_increment != 0)
        return false;

    if (timing->timing_segment1 < minimum->timing_segment1 ||
        timing->timing_segment1 > maximum->timing_segment1)
        return false;
    if (timing->timing_segment2 < minimum->timing_segment2 ||
        timing->timing_segment2 > maximum->timing_segment2)
        return false;

    // CAN requires 1 <= SJW <= min(TSEG1, TSEG2)
    if (timing->sync_jump_width < 1 || timing->sync_jump_width > maximum->sync_jump_width)
        return false;
    if (timing->sync_jump_width > timing->timing_segment1 ||
        timing->sync_jump_width > timing->timing_segment2)
        return false;

    return true;
}

static bool prv_configuration_is_valid(const struct GsUsbCapabilities *capabilities,
                                       const struct GsUsbChannelConfiguration *configuration) {
    if (configuration->mode_flags & ~capabilities->features)
        return false; // the host asked for something that was never advertised

    if (!prv_timing_is_valid(&configuration->nominal, &capabilities->nominal_min, &capabilities->nominal_max, capabilities->nominal_bitrate_prescaler_increment))
        return false;

    if (configuration->mode_flags & GS_USB_FEATURE_FD) {
        if (!prv_timing_is_valid(&configuration->data, &capabilities->data_min, &capabilities->data_max, capabilities->data_bitrate_prescaler_increment))
            return false;
    }

    return true;
}

static struct GsUsbBitTimingConstants nominal_constants(const struct GsUsbCapabilities *capabilities) {
    return (struct GsUsbBitTimingConstants){
        .feature = capabilities->features,
        .can_clock_frequency = capabilities->clock_frequency,
        .timing_segment1_min = capabilities->nominal_min.timing_segment1,
        .timing_segment1_max = capabilities->nominal_max.timing_segment1,
        .timing_segment2_min = capabilities->nominal_min.timing_segment2,
        .timing_segment2_max = capabilities->nominal_max.timing_segment2,
        .sync_jump_width_max = capabilities->nominal_max.sync_jump_width,
        .bitrate_prescaler_min = capabilities->nominal_min.bitrate_prescaler,
        .bitrate_prescaler_max = capabilities->nominal_max.bitrate_prescaler,
        .bitrate_prescaler_inc = capabilities->nominal_bitrate_prescaler_increment,
    };
}

// Fills the control buffer with the response for an IN request and returns its length
static uint16_t prepare_in_response(struct GsUsbCore *core, uint8_t request) {
    const struct GsUsbCapabilities *capabilities = &core->capabilities;

    if (request == GS_USB_REQUEST_DEVICE_CONFIG) {
        const struct GsUsbDeviceConfiguration response = {
            .channel_count = (uint8_t)(capabilities->channel_count - 1),
            .software_version = capabilities->software_version,
            .hardware_version = capabilities->hardware_version,
        };
        memcpy(core->control_buffer, &response, sizeof response);
        return sizeof response;
    }

    if (request == GS_USB_REQUEST_BIT_TIMING_CONSTANTS) {
        const struct GsUsbBitTimingConstants response = nominal_constants(capabilities);
        memcpy(core->control_buffer, &response, sizeof response);
        return sizeof response;
    }

    // GS_USB_REQUEST_BIT_TIMING_CONSTANTS_EXTENDED
    const struct GsUsbBitTimingConstantsExtended response = {
        .nominal = nominal_constants(capabilities),
        .data_timing_segment1_min = capabilities->data_min.timing_segment1,
        .data_timing_segment1_max = capabilities->data_max.timing_segment1,
        .data_timing_segment2_min = capabilities->data_min.timing_segment2,
        .data_timing_segment2_max = capabilities->data_max.timing_segment2,
        .data_sync_jump_width_max = capabilities->data_max.sync_jump_width,
        .data_bitrate_prescaler_min = capabilities->data_min.bitrate_prescaler,
        .data_bitrate_prescaler_max = capabilities->data_max.bitrate_prescaler,
        .data_bitrate_prescaler_inc = capabilities->data_bitrate_prescaler_increment,
    };
    memcpy(core->control_buffer, &response, sizeof response);
    return sizeof response;
}

//--------------------------------------------------------------------
// SETUP stage
//--------------------------------------------------------------------
enum GsUsbControlStatus gs_usb_control_setup(struct GsUsbCore *core, uint8_t request, uint16_t value, uint16_t length, bool direction_in, void **buffer, uint16_t *buffer_length) {
    *buffer = core->control_buffer;

    switch (request) {
        case GS_USB_REQUEST_HOST_FORMAT: // wValue is not a channel here
            if (direction_in || length != sizeof(struct GsUsbHostConfiguration))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = sizeof(struct GsUsbHostConfiguration);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_DEVICE_CONFIG: // wValue is not a channel here
            if (!direction_in)
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = prepare_in_response(core, request);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_BIT_TIMING_CONSTANTS:
            if (!direction_in || !prv_channel_is_valid(core, value))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = prepare_in_response(core, request);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_BIT_TIMING_CONSTANTS_EXTENDED:
            if (!direction_in || !prv_channel_is_valid(core, value) ||
                !(core->capabilities.features & GS_USB_FEATURE_BIT_TIMING_CONSTANTS_EXTENDED))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = prepare_in_response(core, request);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_BIT_TIMING:
            if (direction_in || !prv_channel_is_valid(core, value) ||
                length != sizeof(struct GsUsbBitTiming))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = sizeof(struct GsUsbBitTiming);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_DATA_BIT_TIMING:
            if (direction_in || !prv_channel_is_valid(core, value) ||
                length != sizeof(struct GsUsbBitTiming) ||
                !(core->capabilities.features & GS_USB_FEATURE_FD))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = sizeof(struct GsUsbBitTiming);
            return GS_USB_CONTROL_STATUS_OK;

        case GS_USB_REQUEST_MODE:
            if (direction_in || !prv_channel_is_valid(core, value) ||
                length != sizeof(struct GsUsbModeRequest))
                return GS_USB_CONTROL_STATUS_STALL;
            *buffer_length = sizeof(struct GsUsbModeRequest);
            return GS_USB_CONTROL_STATUS_OK;

        default:
            return GS_USB_CONTROL_STATUS_STALL; // BERR, TIMESTAMP, IDENTIFY: not implemented
    }
}

//--------------------------------------------------------------------
// ACK stage
//--------------------------------------------------------------------
void gs_usb_control_complete(struct GsUsbCore *core, uint8_t request, uint16_t value) {
    switch (request) {
        case GS_USB_REQUEST_HOST_FORMAT:
            memcpy(&core->host_byte_order, core->control_buffer, sizeof core->host_byte_order);
            return;

        case GS_USB_REQUEST_BIT_TIMING:
        case GS_USB_REQUEST_DATA_BIT_TIMING: {
            if (!prv_channel_is_valid(core, value))
                return;
            struct GsUsbBitTiming wire;
            memcpy(&wire, core->control_buffer, sizeof wire);
            struct GsUsbChannelConfiguration *configuration = &core->channels[value].configuration;
            if (request == GS_USB_REQUEST_BIT_TIMING)
                configuration->nominal = prv_timing_from_wire(&wire);
            else
                configuration->data = prv_timing_from_wire(&wire);
            return;
        }

        case GS_USB_REQUEST_MODE: {
            if (!prv_channel_is_valid(core, value))
                return;
            struct GsUsbModeRequest wire;
            memcpy(&wire, core->control_buffer, sizeof wire);

            struct GsUsbChannel *channel = &core->channels[value];
            const uint8_t index = (uint8_t)value;

            if (channel->started) {
                core->can_operations->stop(core->can_context, index);
                channel->started = false;
            }
            if (wire.mode != GS_USB_MODE_START)
                return;

            channel->configuration.mode_flags = wire.flags;
            if (!prv_configuration_is_valid(&core->capabilities, &channel->configuration))
                return; // stays stopped, the port never sees invalid values

            channel->started = core->can_operations->start(core->can_context, index, &channel->configuration);
            return;
        }

        default:
            return;
    }
}
