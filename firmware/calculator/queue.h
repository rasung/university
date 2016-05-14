#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>

struct DeviceQueue {
  uint32_t queue[20];
  uint32_t head;
  uint32_t tail;
};

void init_queue(void);
void key_enqueue(uint32_t scancode);
uint32_t key_dequeue(void);

#endif
