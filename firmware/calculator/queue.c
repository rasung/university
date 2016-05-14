#include "queue.h"

static struct DeviceQueue keyqueue;

void init_queue(void)
{
  keyqueue.head = 0;
  keyqueue.tail = 0;
}

void key_enqueue(uint32_t scancode)
{
  keyqueue.queue[keyqueue.head] = scancode;
  keyqueue.head = (keyqueue.head + 1) % 10;
}

uint32_t key_dequeue(void)
{
  uint32_t scancode = 0xffffffff;
  if(keyqueue.tail != keyqueue.head) {
    scancode = keyqueue.queue[keyqueue.tail];
    keyqueue.tail = (keyqueue.tail + 1) % 10;
  }
  return scancode;
}
