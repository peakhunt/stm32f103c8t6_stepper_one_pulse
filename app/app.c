#include "stm32f1xx_hal.h"
#include "app_common.h"

#include "sys_timer.h"
#include "event_dispatcher.h"
#include "mainloop_timer.h"
#include "shell.h"
#include "misc.h"
#include "stepper.h"
#include "motion_demo.h"

void
app_init(void)
{
  __disable_irq();

  event_dispatcher_init();
  mainloop_timer_init();

  sys_timer_init();
  shell_init();
  misc_init();

  stepper_init();

  __enable_irq();

  motion_demo_init();
}

void
app_run(void)
{
  while(1)
  {
    event_dispatcher_dispatch();
  }
}
