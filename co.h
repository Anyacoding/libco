#include <setjmp.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define PGSIZE 4096
#define STACK_SIZE (PGSIZE * 8)
#define MAX_NR_CO 4   // The maximum number of coroutines

struct co* co_start(const char *name, void (*func)(void *), void *arg);
void co_yield();
void co_wait(struct co *co);

enum co_status {
  CO_NEW = 1,  // Newly created, not yet executed
  CO_RUNNING,  // Have been executed
  CO_WAITING,  // Wait on co_wait
  CO_DEAD,     // It's over, but have not been free
};

typedef struct co {
  const char *name;
  void (*func)(void *);                       // co_start specifies the entry address and parameters
  void *arg;

  enum co_status status;                      // The state of the coroutine
  struct co *    waiter;                      // Whether there are other coroutines waiting for the current coroutine
  jmp_buf        context;                     // Field of register (setjmp.h)
  uint8_t        stack[STACK_SIZE];           // The stack of coroutines is allocated eight pages
  int            index;                       // Index in the list of coroutines
} co;
