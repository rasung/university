#ifndef _FND_H_
#define _FND_H_

#include <stdint.h>

void clock_pin(uint8_t port, uint32_t pin);
void fnd_portout(uint32_t value);
static void fnd_init(void);
void fnd_digit(uint32_t digit);
void fndvalue_init(void);
uint32_t input_fndvalue(uint32_t value);
void erase_fndvalue(void);
uint32_t get_fndvalue (void);
void set_fndvalue (uint32_t value);

#endif
