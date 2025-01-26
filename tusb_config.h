#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#define CFG_TUSB_DEBUG        0
#define CFG_TUD_ENABLED       1
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED
#define CFG_TUD_ENDPOINT0_SIZE    64

#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN    __attribute__ ((aligned(4)))
#endif

#define CFG_TUD_HID           1
#define CFG_TUD_CDC           0
#define CFG_TUD_MSC           1
#define CFG_TUD_MIDI          0
#define CFG_TUD_VENDOR        0

#define CFG_TUD_HID_EP_BUFSIZE    16
#define CFG_TUD_MSC_EP_BUFSIZE    512

#ifdef __cplusplus
}
#endif

#endif