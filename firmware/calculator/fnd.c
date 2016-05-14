#include "fnd.h"

static uint32_t fndmap[0x10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x98};
static uint32_t bitmap[0x08] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static uint32_t FNDVALUE = 0xffffffff;

void clock_pin(uint8_t port, uint32_t pin) {
  GPIO_SetValue(port, 1<<pin);
  GPIO_ClearValue(port, 1<<pin);
}

void fnd_portout(uint32_t value) {
  if(value & 0x01) GPIO_SetValue(0, 1<<23);
  else GPIO_ClearValue(0, 1<<23);
  if(value & 0x02) GPIO_SetValue(0, 1<<24);
  else GPIO_ClearValue(0, 1<<24);
  if(value & 0x04) GPIO_SetValue(1, 1<<21);
  else GPIO_ClearValue(1, 1<<21);
  if(value & 0x08) GPIO_SetValue(2, 1<<11);
  else GPIO_ClearValue(2, 1<<11);
  if(value & 0x10) GPIO_SetValue(2, 1<<12);
  else GPIO_ClearValue(2, 1<<12);
  if(value & 0x20) GPIO_SetValue(2, 1<<13);
  else GPIO_ClearValue(2, 1<<13);
  if(value & 0x40) GPIO_SetValue(3, 1<<25);
  else GPIO_ClearValue(3, 1<<25);
  if(value & 0x80) GPIO_SetValue(3, 1<<26);
  else GPIO_ClearValue(3, 1<<26);
}

void fnd_init(void) {
  GPIO_SetDir(0, 1<<4, 1);
  GPIO_SetDir(0, 0x07<<19, 1);

  GPIO_SetDir(0, 0x03<<23, 1);
  GPIO_SetDir(0, 0x01<<21, 1);
  GPIO_SetDir(0, 0x07<<11, 1);
  GPIO_SetDir(0, 0x03<<25, 1);

  GPIO_SetDir(0, 0x01<<5, 1);
  GPIO_SetDir(0, 0x01<<10, 1);

  GPIO_SetValue(0, 0x07<<19);
  GPIO_ClearValue(0, 0x01<<5);
  GPIO_ClearValue(0, 0x01<<10);
}

void fnd_digit(uint32_t digit) {
  uint32_t value;

  fnd_init();
  GPIO_ClearValue(0, 0x01<<4);
  GPIO_ClearValue(0, 0x01<<20);

  fnd_portout(0);
  clock_pin(0, 10);

  value = (FNDVALUE >> (digit*4)) & 0x0f;

  if (value != 0x0f) {
    fnd_portout(fndmap[value]);
    clock_pin(0, 5);

    fnd_portout(bitmap[digit]);
    clock_pin(0, 10);
  }
  GPIO_SetValue(0, 0x01<<20);
}

void fndvalue_init(void) {
  FNDVALUE = 0xffffffff;
}

uint32_t input_fndvalue (uint32_t value) {
  uint32_t number;
  if (value < 0) return 0;
  number = value % 10;
  value = value/10;
  input_fndvalue(value);
  FNDVALUE = (FNDVALUE << 4) | number;
  return 0;
}

void erase_fndvalue (void) {
  FNDVALUE = (FNDVALUE >> 4) | (0x0f << 7);
}

uint32_t get_fndvalue (void) {
  return FNDVALUE;
}

void set_fndvalue (uint32_t value) {
  FNDVALUE = value;
}
