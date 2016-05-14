#include "joystick.h"
#include "stepmotor.h"
#include "timer.h"

void initialize(void);

int main(void) {
  initialize();
  while (1) {
		if (joyleft() == 0) {
			set_SM_state(SM_LEFT);
		}
    else if (joyright() == 0) {
			set_SM_state(SM_RIGHT);
		}
  }
  return 0;
}

void initialize(void) {
  init_joystick();
  init_timer0();
  init_stepmotor();
}
