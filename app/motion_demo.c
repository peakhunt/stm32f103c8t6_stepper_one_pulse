#include "motion_demo.h"
#include "stepper.h"

#include "soft_timer.h"
#include "mainloop_timer.h"
#include "event_dispatcher.h"
#include "event_list.h"

#define IDLE_TIME         2000
#define BWD_WAIT_TIME     2500

typedef enum
{
  motion_command_forward,
  motion_command_backward,
  motion_command_wait,
} motion_command_t;

typedef struct
{
  motion_command_t      cmd;
  uint32_t              arg;
} motion_command_struct_t;

//
// I'm using 0.9 degree stepper.
// So 400 steps for rotate
//
// XXX
// 0 means oen step
// 200 means 199 step
//
static motion_command_struct_t    _commands[] =
{
  { motion_command_wait,      2000 },

  { motion_command_forward,   0 },
  { motion_command_forward,   0 },
  { motion_command_forward,   197 },
  { motion_command_forward,   55 },
  { motion_command_forward,   3 },
  { motion_command_forward,   7 },
  { motion_command_forward,   29 },
  { motion_command_forward,   33 },
  { motion_command_forward,   67 },

  { motion_command_wait,      500 },

  { motion_command_backward,  77 },
  { motion_command_backward,  77 },
  { motion_command_backward,  3 },
  { motion_command_backward,  0 },
  { motion_command_backward,  1 },
  { motion_command_backward,  2 },
  { motion_command_backward,  3 },
  { motion_command_backward,  99 },
  { motion_command_backward,  129 },

  { motion_command_wait,      500 },

  { motion_command_backward,  30 },
  { motion_command_forward,   30 },
  { motion_command_forward,   30 },
  { motion_command_backward,  30 },

  { motion_command_backward,  40 },
  { motion_command_forward,   40 },
  { motion_command_forward,   40 },
  { motion_command_backward,  40 },

  { motion_command_backward,  50 },
  { motion_command_forward,   50 },
  { motion_command_forward,   50 },
  { motion_command_backward,  50 },

  { motion_command_backward,  60 },
  { motion_command_forward,   60 },
  { motion_command_forward,   60 },
  { motion_command_backward,  60 },

  { motion_command_backward,  70 },
  { motion_command_forward,   70 },
  { motion_command_forward,   70 },
  { motion_command_backward,  70 },

  { motion_command_backward,  80 },
  { motion_command_forward,   80 },
  { motion_command_forward,   80 },
  { motion_command_backward,  80 },

  { motion_command_backward,  90 },
  { motion_command_forward,   90 },
  { motion_command_forward,   90 },
  { motion_command_backward,  90 },
};

static SoftTimerElem          _tmr;
static uint32_t               _current_ndx;

static void
execute_command(void)
{
  motion_command_struct_t*    cmd = &_commands[_current_ndx];

  switch(cmd->cmd)
  {
  case motion_command_forward:
    stepper_step(true, (uint8_t)cmd->arg);
    break;

  case motion_command_backward:
    stepper_step(false, (uint8_t)cmd->arg);
    break;

  case motion_command_wait:
    mainloop_timer_schedule(&_tmr, cmd->arg);
    break;
  }
}

static void
stepper_complete_handler(uint32_t e)
{
  _current_ndx++;
  if(_current_ndx >= sizeof(_commands)/sizeof(motion_command_struct_t))
  {
    _current_ndx = 0;
  }
  execute_command();
}

static void
motion_demo_timeout(SoftTimerElem* te)
{
  stepper_complete_handler(0);
}

void
motion_demo_init(void)
{
  soft_timer_init_elem(&_tmr);
  _tmr.cb = motion_demo_timeout;

  event_register_handler(stepper_complete_handler, DISPATCH_EVENT_STEPPER_COMPLETE);

  _current_ndx = 0;
  execute_command();
}
