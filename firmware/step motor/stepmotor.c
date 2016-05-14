#include "stepmotor.h"

static uint32_t SMPhase[4] = {0x05, 0x06, 0x0a, 0x09};
static int rotation_step = 0;
static uint32_t SM_state = SM_NOT;

void init_stepmotor (void) {
  GPIO_SetDir(0, 1U<<5, 1);
  GPIO_SetDir(0, 1U<<10, 1);
  GPIO_SetDir(0, 1U<<23, 1);
  GPIO_SetDir(0, 1U<<24, 1);

	GPIO_SetDir(0, 1U<<11, 1);
	GPIO_SetDir(1, 1U<<21, 1);
	GPIO_SetDir(2, 1U<<11, 1);
	GPIO_SetDir(2, 1U<<12, 1);
	GPIO_SetDir(2, 1U<<13, 1);
	GPIO_SetDir(3, 1U<<25, 1);
	GPIO_SetDir(3, 1U<<26, 1);

  GPIO_SetDir(0, 1U<<4, 0);
  GPIO_SetDir(0, 1U<<19, 1);
  GPIO_SetDir(0, 1U<<20, 1);
  GPIO_SetDir(0, 1U<<21, 1);

  GPIO_ClearValue(0, 1U<<4);
  GPIO_ClearValue(0, 1U<<19);
  GPIO_SetValue(0, 1U<<20);
  GPIO_SetValue(0, 1U<<21);
}

void SM_left_move (int n) {
  static int i = 0;
  input_stepmotor(SMPhase[rotation_step]);
  ++rotation_step;
  if (rotation_step > 3) rotation_step = 0;
  ++i;
  if (i == (48 * n)) {
    SM_state = SM_NOT;
    i = 0;
  }
}

void SM_right_move (int n) {
  static int i = 0;
  input_stepmotor(SMPhase[rotation_step]);
	--rotation_step;
	if (rotation_step == -1) rotation_step = 3;

  ++i;
  if (i == (48 * n)) {
    SM_state = SM_NOT;
    i = 0;
  }
}

void input_stepmotor(uint32_t value) {
  if(value & 0x01) GPIO_SetValue(0, 1<<5);
  else GPIO_ClearValue(0, 1<<5);
  if(value & 0x02) GPIO_SetValue(0, 1<<10);
  else GPIO_ClearValue(0, 1<<10);
  if(value & 0x04) GPIO_SetValue(0, 1<<23);
  else GPIO_ClearValue(0, 1<<23);
  if(value & 0x08) GPIO_SetValue(0, 1<<24);
  else GPIO_ClearValue(0, 1<<24);
}

void set_SM_state (uint32_t value) {
  SM_state = value;
}

uint32_t get_SM_state (void) {
  return SM_state;
}

void set_rotation_step (int value) {
  rotation_step = value;
}
