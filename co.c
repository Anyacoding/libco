#include "co.h"

#define LOCAL_MACHINE
#ifdef LOCAL_MACHINE
  #define debug(...) printf(__VA_ARGS__)
#else
  #define debug(...)
#endif

static co* CO_LIST[MAX_NR_CO] __attribute__((used)) = {};
static struct co Main = { .status = CO_RUNNING };
static struct co* current = &Main;

static inline void stack_switch_call(void *sp, void *entry, void* arg) {
  debug("\nIn stack_switch_call()\n");
  asm volatile (
#if __x86_64__
    "movq %0, %%rsp; movq %2, %%rdi; callq *%1"
      : : "b"((uintptr_t)sp), "d"(entry), "a"(arg) : "memory"
#else
    "movl %0, %%esp; movl %2, (%0); call *%1"
      : : "b"((uintptr_t)sp - 4), "d"(entry), "a"(arg) : "memory"
#endif
  );
  current->status = CO_DEAD;
  printf("\n%s dead\n", current->name);
  CO_LIST[current->index] = NULL;   // Remove from the list of coroutines
  if (current->waiter != NULL) {
    current->waiter->status = CO_RUNNING;
    longjmp(current->waiter->context, 1);
  }
  else {
    co_yield();
  }
}

static inline void schedule() {
  // for (int i = 0; i < MAX_NR_CO; ++i) {
  //   if (current != CO_LIST[i] && CO_LIST[i] != NULL) {
  //     current = CO_LIST[i];
  //     debug("index: %d\n", i);
  //     break;
  //   }
  // }
  srand(time(0));
  int flag = 1;
  while (flag) {
    current = CO_LIST[rand() % MAX_NR_CO];
    if(current != NULL && current->status != CO_DEAD) {
      // debug("index: %d\n", current->index);
      flag = 0;
    }
  }
  assert(current != &Main);
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  co* coroutine = (co*)malloc(sizeof(co));
  coroutine->status = CO_NEW;
  coroutine->name = name;
  coroutine->func = func;
  coroutine->arg = arg;
  coroutine->waiter = NULL;

  for (size_t i = 0; i < MAX_NR_CO; ++i) {
    if (CO_LIST[i] == NULL) {
      CO_LIST[i] = coroutine;
      coroutine->index = i;
      break;
    }
  }

  printf("%s: %d\n", (char*)coroutine->name, coroutine->index);
  return coroutine;
}

void co_wait(struct co *co) {
  debug("In co_wait(%s).\n", co->name);
  assert(co);
  assert(current);
  current->status = CO_WAITING;
  co->waiter = current;
  while (co->status != CO_DEAD) {
    co_yield ();
  }
  assert(co->status == CO_DEAD);
  current = co->waiter;     // Update current to waiter
  debug("Out co_wait(%s).\n\n", co->name);
  free(co);
  
}

void co_yield() {
  // debug("In co_yield\n");
  int val = setjmp(current->context);   // Save register field
  if (val == 0) {
    schedule();
    if (current->status == CO_NEW) {
      current->status = CO_RUNNING;
      stack_switch_call(current->stack + STACK_SIZE, current->func, current->arg);
    }
    else if (current->status == CO_RUNNING) {
      longjmp(current->context, 1);    // Recovery register field
    }
  }
  else {
    return;  // After being restored by longjmp, 
             // it arrives here and directly returns to resume the last task of the coroutine
  }
}
