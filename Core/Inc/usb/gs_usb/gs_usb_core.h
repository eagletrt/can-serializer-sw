#ifndef GS_CORE_H
#define GS_CORE_H

#include <stdbool.h>
#include <stdint.h>

#include "gs_usb_port.h"
#include "gs_usb_protocol.h"

#ifndef GS_CHANNEL_COUNT_MAX
#define GS_CHANNEL_COUNT_MAX (3)
#endif

#define GS_ECHO_SLOT_COUNT (16)
#define GS_CONTROL_BUFFER_WORDS (sizeof(struct GsUsbBitTimingConstantsExtended) / sizeof(uint32_t))

enum GsControlStatus {
    GS_CONTROL_STATUS_STALL,
    GS_CONTROL_STATUS_OK,
};

struct GsChannel {
    struct GsCanChannelConfiguration configuration;
    bool started;
    struct GsUsbHostFrame echo[GS_ECHO_SLOT_COUNT]; /*<! frames in flight, indexed by echo_id */
};

struct GsCore {
    struct GsCanCapabilities capabilities;
    const struct GsCanOperations *can_operations;
    void *can_context;
    const struct GsUsbOperations *usb_operations;
    void *usb_context;

    struct GsChannel channels[GS_CHANNEL_COUNT_MAX];
    uint32_t host_byte_order;
    uint32_t control_buffer[GS_CONTROL_BUFFER_WORDS]; /*<! 4-byte aligned, must outlive the transfer */

    struct GsUsbHostFrame pending; /*<! read from USB, not yet accepted by the CAN port */
    bool has_pending;
    uint8_t next_channel; /*<! round-robin start for the IN stream */
};

/* Copies the capabilities and clears all channels. channel_count is clamped to GS_CHANNEL_COUNT_MAX. */
void gs_core_initialize(struct GsCore *core, const struct GsCanCapabilities *capabilities, const struct GsCanOperations *can_operations, void *can_context, const struct GsUsbOperations *usb_operations, void *usb_context);

/* Control plane. SETUP stage: on OK, *buffer and *buffer_length describe the data phase. */
enum GsControlStatus gs_control_setup(struct GsCore *core, uint8_t request, uint16_t value, uint16_t length, bool direction_in, void **buffer, uint16_t *buffer_length);

/* Control plane. ACK stage of an OUT request: the data is in the buffer, apply it. */
void gs_control_complete(struct GsCore *core, uint8_t request, uint16_t value);

/* Data plane. Main loop only. */
void gs_core_poll(struct GsCore *core);

#endif // GS_CORE_H
