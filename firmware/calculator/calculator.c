#include "calculator.h"

static uint32_t operand = 0;
static uint32_t operator = 0;
static uint32_t clear_fnd = 0; //  if 1 -> '=' 입력시 fnd 리셋 스위치

uint32_t decimal (int n) {
  int i;
  uint32_t decimal = 1;
  for (i=0; i<=n; ++i) {
    decimal = decimal * 10;
  }
  return decimal;
}

uint32_t decimalize_fndvalue (void) {
  uint32_t FNDVALUE = get_fndvalue();
  uint32_t decimal_value = 0;
  int i = 0;
  while (FNDVALUE != 0xffffffff) {
    decimal_value += (FNDVALUE & 0x0f) * decimal(i);
    FNDVALUE = (FNDVALUE >> 4) | (0x0f << 7);
    i++;
  }
  return decimal_value;
}

uint32_t calculate (void) {
  if (operator != 0) {
    switch (operator) {
      case '*': operand *= decimalize_fndvalue(); break;
      case '/': operand /= decimalize_fndvalue(); break;
      case '+': operand += decimalize_fndvalue(); break;
      case '-': operand -= decimalize_fndvalue(); break;
    }
  }
}

void keypad_operand(uint32_t value) {
  if (clear_fnd == 1) {
    fndvalue_init();
    set_fndvalue(get_fndvalue() << 4 | value);
    clear_fnd = 0;
  }
  else set_fndvalue(get_fndvalue() << 4 | key);
}

void keypad_result(void) {
  if (operator != 0) {
    if (get_fndvalue() != 0xffffffff) calculate();
    input_fndvalue(operand);
    operand = 0;
    operator = 0;
    clear_fnd = 1;
  }
  else if (get_fndvalue() != 0xffffffff) clear_fnd = 1;
}

void keypad_operator(uint32_t value) {
  if (operator != 0) {
    if (get_fndvalue() != 0xffffffff) {
      calculate();
      input_fndvalue(operand);
      clear_fnd = 1;
    }
    else operator = value;
  }
  else if (get_fndvalue() != 0xffffffff) {
    calculate();
    clear_fnd = 1;
  }
}
