#include <bsp/board_api.h>
#include <tusb.h>
#include "usb_descriptors.h"
#include "class/msc/msc_device.h"

#ifndef MSC_SUBCLASS_SCSI
#define MSC_SUBCLASS_SCSI    0x06
#endif

#define MAX_BUF_SIZE 32

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_MSC_DESC_LEN)

tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = 0xCafe,
    .idProduct = 0x0137,
    .bcdDevice = 0x0100,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

char const* string_desc_arr[] = {
    [0] = (const char[]){0x09, 0x04},
    [1] = "CZR_LABZ",
    [2] = "CZR_USB",
    [3] = "123456" // Added serial number
};

static uint16_t* _str_desc_cv(const char* ascii_str) {
    static uint16_t desc_buf[MAX_BUF_SIZE];
    uint8_t len = strlen(ascii_str);
    
    if (len >= MAX_BUF_SIZE) len = MAX_BUF_SIZE - 1;
    
    desc_buf[0] = (TUSB_DESC_STRING << 8) | (2 + 2 * len);
    
    for (uint8_t i = 0; i < len; i++) {
        desc_buf[1 + i] = ascii_str[i];
    }
    
    return desc_buf;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    
    if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) {
        return NULL;
    }
    
    return (uint16_t const*)_str_desc_cv(string_desc_arr[index]);
}

uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 
                         TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_KEYBOARD,
                      sizeof(desc_hid_report), EPNUM_HID,
                      CFG_TUD_HID_EP_BUFSIZE, 5),
                      
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT,
                      EPNUM_MSC_IN, CFG_TUD_MSC_EP_BUFSIZE)
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_MSC_DESC_LEN)

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    return (index == 0) ? desc_configuration : NULL;
}

uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance) {
    return (instance == 0) ? desc_hid_report : NULL;
}