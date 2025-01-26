#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#define ITF_NUM_HID     0
#define ITF_NUM_MSC     1
#define ITF_NUM_TOTAL   2

#define EPNUM_HID       0x81
#define EPNUM_MSC_OUT   0x02
#define EPNUM_MSC_IN    0x82

#ifndef CFG_TUD_HID_EP_BUFSIZE
#define CFG_TUD_HID_EP_BUFSIZE    16
#endif

#ifndef CFG_TUD_MSC_EP_BUFSIZE
#define CFG_TUD_MSC_EP_BUFSIZE    512
#endif

enum {
   REPORT_ID_KEYBOARD = 1,
   REPORT_ID_MOUSE,
   REPORT_ID_CONSUMER_CONTROL,
   REPORT_ID_GAMEPAD,
   REPORT_ID_COUNT
};

uint8_t const* tud_descriptor_configuration_cb(uint8_t index);
uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance);

#endif