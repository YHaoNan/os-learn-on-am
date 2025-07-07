#include "am.h"
#include "klib.h"

// main.c
int jiffies = 0;

// 中断处理函数
Context* interrupt_handler(Event ev, Context *ctx) {
  iset(false);
  if (ev.event == EVENT_IRQ_TIMER) { // 判断是否为时钟中断
    jiffies++;
  }
  iset(true);
  return ctx; // 返回 context
}


int main() {
  printf("hello\n");
  cte_init(interrupt_handler); // 注册中断处理函数
  printf("interrupt_handler init done\n");

  iset(true);

  while (1) {
    printf("jiffies: %d\n", jiffies);
  }

}