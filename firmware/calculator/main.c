#include "timer.h"
#include "calculator.h"
#include "keyscan.h"
#include "queue.h"
#include "fnd.h"

void initialize (void);

int main (void) {
  uint32_t key;

  initialize();

  while (1) {
    key = get_key1(key_dequeue());
    if (key != 0xffffffff) {
      if (key >= 0 && key <= 9) {  // key 0 ~ 9
        keypad_operand(key);
      }
      else if (key == '=') {  // key '='
        keypad_result();
      }
      else if (key == 'x') {  // key 'x' erase
        erase_fndvalue();
      }
      else {  // key '*', '/', '+', '-'
        keypad_operator(key);
      }
    }
  }
}

void initialize (void) {
  init_timer1();
  init_queue();
}
