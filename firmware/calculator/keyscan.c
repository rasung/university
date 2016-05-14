#include "keyscan.h"

void put_scancode(uint32_t scan_x)
{
  GPIO_SetValue(0, 1<<5);
  GPIO_SetValue(0, 1<<10);
  GPIO_SetValue(2, 1<<12);
  GPIO_SetValue(2, 1<<13);

  switch (scan_x) {
    case 1: GPIO_ClearValue(0, 1<<5); break;
    case 2: GPIO_ClearValue(0, 1<<10); break;
    case 4: GPIO_ClearValue(2, 1<<12); break;
    case 8: GPIO_ClearValue(2, 1<<13); break;
    default: break;
  }
}

uint32_t get_scancode(void)
{
  uint32_t scan_y = 0;
  scan_y = (GPIO_ReadValue(0) >> 23) & 0x03;
  scan_y = ((GPIO_ReadValue(3) >> 23) & 0x0c) | scan_y;
  return scan_y;
}

uint32_t get_key1(uint32_t code)
{
  uint32_t key;
  switch (code) {
    case 0x11: key = '+'; break;
    case 0x12: key = 9; break;
    case 0x14: key = 8; break;
    case 0x18: key = 7; break;
    case 0x21: key = '-'; break;
    case 0x22: key = 6; break;
    case 0x24: key = 5; break;
    case 0x28: key = 4; break;
    case 0x41: key = '*'; break;
    case 0x42: key = 3; break;
    case 0x44: key = 2; break;
    case 0x48: key = 1; break;
    case 0x81: key = '/'; break;
    case 0x82: key = 'x'; break;
    case 0x84: key = 0; break;
    case 0x88: key = '='; break;
    default: break;
  }
  return key;
}

void keypad_Init()
{
  GPIO_SetDir(0, 1<<4, 1);
  GPIO_SetDir(0, 0x07<<19, 1);
  GPIO_SetDir(0, 1<<5, 1);
  GPIO_SetDir(0, 1<<10, 1);
  GPIO_SetDir(2, 1<<12, 1);
  GPIO_SetDir(2, 1<<13, 1);
  GPIO_SetDir(1, 1<<21, 1);
  GPIO_SetDir(2, 1<<11, 1);
  GPIO_SetDir(0, 0x03<<23, 0);
  GPIO_SetDir(3, 0x03<<25, 0);
  GPIO_ClearValue(1, 1<<21);
  GPIO_SetValue(0, 0x07<<19);
}

uint32_t scan_keypad(void)
{
  uint32_t scan_x, scan_y;
  uint32_t scan_code = 0xffffffff;
  uint32_t key;

  keypad_Init();
  GPIO_ClearValue(0, 0x01<<21);

  for (scan_x = 1; scan_x < 0x10; scan_x <<= 1) {
    GPIO_ClearValue(0, 0x01<<4);
    put_scancode(scan_x);
    clock_pin(2, 11);
    GPIO_SetValue(0, 0x01<<4);
    scan_y = get_scancode();
    if (scan_y != 0x0f) {
      scan_code = scan_x << 4 | ((~scan_y) & 0x0f);
      break;
    }
  }
  GPIO_SetValue(0, 0x01<<21);
  return scan_code;
}
