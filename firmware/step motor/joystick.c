#include "joystick.h"


void init_joystick(void) {
  GPIO_SetDir(1, 0xB0000000, 1); // GPIO LED 포트 설정
  GPIO_SetDir(2, 0x0000007C, 1);
  GPIO_SetDir(1, 0x02800000, 0); // Joystick 포트 설정
}

uint32_t joyleft (void) {
  init_joystick();
  return GPIO_ReadValue(1) & 0x00800000;
}

uint32_t joyright (void) {
  init_joystick();
  return GPIO_ReadValue(1) & 0x02000000;
}
