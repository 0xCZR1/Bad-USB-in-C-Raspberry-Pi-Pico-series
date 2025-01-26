#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#define BLOCK_SIZE    512
#define BLOCK_COUNT   256
#define DISK_SIZE    (BLOCK_SIZE * BLOCK_COUNT)

static uint8_t virtual_disk[DISK_SIZE] __attribute__((aligned(4)));

enum {
  IS_NOT_MOUNTED = 100,
  IS_MOUNTED = 5000,
  IS_SUSPENDED = 2000,
};

static uint32_t blink_interval_ms = IS_NOT_MOUNTED;
static bool prev_mounted = false;
static bool prev_suspended = false;
const uint LED_PIN = 25;

static void send_keyboard_report(uint8_t report_id, uint8_t keycode) {
  if (!tud_hid_ready()) return;
  uint8_t keycode_arr[6] = { keycode, 0, 0, 0, 0, 0 };
  tud_hid_keyboard_report(report_id, 0, keycode_arr);
}

void led_blinking_task(void) {
    static uint32_t last_blink_time = 0;
    static bool led_state = false;
    uint32_t current_time = time_us_64() / 1000;
    uint32_t interval = tud_mounted() ? 
                       (tud_suspended() ? IS_SUSPENDED : IS_MOUNTED) : 
                       IS_NOT_MOUNTED;

    if (current_time - last_blink_time > interval) {
        last_blink_time = current_time;
        led_state = !led_state;
        gpio_put(LED_PIN, !led_state);  
    }
}

int main(void) {
  stdio_init_all();
  board_init();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
   
  if (!tud_init(BOARD_TUD_RHPORT)) {
      while (1) {} // Halt on failure
  }

  static const uint8_t fat_table[] = {
      0xF8, 0xFF, 0xFF, 0xFF, 0x0F
  };

  memset(virtual_disk, 0, DISK_SIZE);
  virtual_disk[0] = 0xEB;
  virtual_disk[1] = 0x3C;
  virtual_disk[2] = 0x90;
  memcpy(&virtual_disk[3], "MSDOS5.0", 8);
  *(uint16_t*)&virtual_disk[0x0B] = 512;
  virtual_disk[0x0D] = 1;
  *(uint16_t*)&virtual_disk[0x0E] = 1;
  virtual_disk[0x10] = 2;
  *(uint16_t*)&virtual_disk[0x11] = 16;
  *(uint16_t*)&virtual_disk[0x13] = BLOCK_COUNT;
  virtual_disk[0x15] = 0xF8;
  *(uint16_t*)&virtual_disk[0x16] = 1;
  virtual_disk[0x24] = 0x80;
  virtual_disk[0x26] = 0x29;
  memcpy(&virtual_disk[0x2B], "NO NAME    ", 11);
  memcpy(&virtual_disk[0x36], "FAT12   ", 8);

  memcpy(&virtual_disk[512], fat_table, sizeof(fat_table));
  memcpy(&virtual_disk[512 + 512], fat_table, sizeof(fat_table));

  while (1) {
      tud_task();
      led_blinking_task();
      sleep_ms(20);

      bool const mounted = tud_mounted();
      bool const suspended = tud_suspended();

      if (mounted != prev_mounted) {
          prev_mounted = mounted;
      }

      if (suspended != prev_suspended) {
          prev_suspended = suspended;
      }
  }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, 
                            hid_report_type_t report_type, uint8_t* buffer, 
                            uint16_t reqlen) {
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                        hid_report_type_t report_type, uint8_t const* buffer, 
                        uint16_t bufsize) {
}

bool tud_msc_test_unit_ready_cb(uint8_t lun) {
  return true;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], 
                     uint8_t product_id[16], uint8_t product_rev[4]) {
  const char vid[] = "CZR_LABZ";
  const char pid[] = "Virtual Disk";
  const char rev[] = "1.0";
  
  memcpy(vendor_id, vid, strlen(vid));
  memset(vendor_id + strlen(vid), ' ', 8 - strlen(vid));
  memcpy(product_id, pid, strlen(pid));
  memset(product_id + strlen(pid), ' ', 16 - strlen(pid));
  memcpy(product_rev, rev, strlen(rev));
  memset(product_rev + strlen(rev), ' ', 4 - strlen(rev));
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
  *block_count = BLOCK_COUNT;
  *block_size = BLOCK_SIZE;
}

int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, 
                       void* buffer, uint32_t bufsize) {
  uint32_t addr = lba * BLOCK_SIZE + offset;
  if (addr + bufsize > DISK_SIZE) return -1;
  memcpy(buffer, virtual_disk + addr, bufsize);
  return bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset,
                        uint8_t* buffer, uint32_t bufsize) {
  uint32_t addr = lba * BLOCK_SIZE + offset;
  if (addr + bufsize > DISK_SIZE) return -1;
  memcpy(virtual_disk + addr, buffer, bufsize);
  return bufsize;
}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16],
                      void* buffer, uint16_t bufsize) {
  return -1;
}