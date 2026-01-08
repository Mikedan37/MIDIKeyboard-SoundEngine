#include "tusb_config.h"
#include "tusb.h"
#include <stdint.h>
#include <string.h>

// USB Configuration Descriptor
enum {
    ITF_NUM_MIDI,
    ITF_NUM_TOTAL
};

// MIDI Endpoint Macros
#define EPNUM_MIDI_OUT  0x02
#define EPNUM_MIDI_IN   0x83

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN)

// Device Descriptor
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = TUSB_CLASS_AUDIO,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,
    .idProduct          = 0x4000,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

// Configuration Descriptor
uint8_t const desc_configuration[] = {
    // Configuration descriptor
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

    // MIDI Interface
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 4, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 64)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

// String Descriptors
char const* string_desc_arr[] = {
    "Sharkisha Studios",     // 0: Manufacturer
    "MIDI Keyboard",        // 1: Product
    "123456",                // 2: Serial
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;

    if (index == 0) {
        _desc_str[1] = 0x0409; // English
        _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 + 2);
        return _desc_str;
    }

    if ( index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0]) ) return NULL;

    const char* str = string_desc_arr[index];
    uint8_t chr_count = strlen(str);

    for (uint8_t i = 0; i < chr_count; i++) {
        _desc_str[1 + i] = str[i];
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
    return _desc_str;
}
