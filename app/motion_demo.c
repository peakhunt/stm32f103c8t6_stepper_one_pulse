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
  motion_demo_state_idle,
  motion_demo_state_fwd,
  motion_demo_state_wait,
  motion_demo_state_bwd,
} motion_demo_state_t;

typedef enum
{
  motion_demo_event_timeout,
  motion_demo_event_step_complete,
} motion_demo_event_t;

//
// I'm using 0.9 degree stepper.
// So 400 steps for rotate
//
// XXX
// 0 means oen step
// 200 means 199 step
//
static uint8_t    _fwd_rotate[] =
{
  0,
  0,
  197,
  55,
  3,
  7,
  29,
  33,
  67,
};

static uint8_t    _bwd_rotate[] =
{
  77,
  77,
  3,
  0,
  1,
  2,
  3,
  99,
  129,
};

static SoftTimerElem          _tmr;
static motion_demo_state_t    _state;
static uint32_t               _current_ndx;

static void
execute_step(void)
{
  switch(_state)
  {
  case motion_demo_state_fwd:
    stepper_step(true, _fwd_rotate[_current_ndx]);
    break;

  case motion_demo_state_bwd:
    stepper_step(false, _bwd_rotate[_current_ndx]);
    break;

  default:
    break;
  }
}

static void
enter_state(motion_demo_state_t new_state)
{
  _state = new_state;

  switch(_state)
  {
  case motion_demo_state_idle:
    mainloop_timer_schedule(&_tmr, IDLE_TIME);
    break;

  case motion_demo_state_fwd:
    _current_ndx = 0;
    execute_step();
    break;

  case motion_demo_state_wait:
    mainloop_timer_schedule(&_tmr, BWD_WAIT_TIME);
    break;

  case motion_demo_state_bwd:
    _current_ndx = 0;
    execute_step();
    break;
  }
}

static void
motion_demo_event_handler(motion_demo_event_t e)
{
  switch(_state)
  {
  case motion_demo_state_idle:
    if(e == motion_demo_event_timeout)
    {
      enter_state(motion_demo_state_fwd);
    }
    else if(e == motion_demo_event_step_complete)
    {
      // impossible. BUG if occurs
    }
    break;

  case motion_demo_state_fwd:
    if(e == motion_demo_event_timeout)
    {
      // impossible. BUG if occurs
    }
    else if(e == motion_demo_event_step_complete)
    {
      _current_ndx++;
      if(_current_ndx < sizeof(_fwd_rotate))
      {
        execute_step();
      }
      else
      {
        enter_state(motion_demo_state_wait);
      }
    }
    break;

  case motion_demo_state_wait:
    if(e == motion_demo_event_timeout)
    {
      enter_state(motion_demo_state_bwd);
    }
    else if(e == motion_demo_event_step_complete)
    {
      // impossible. BUG if occurs
    }
    break;

  case motion_demo_state_bwd:
    if(e == motion_demo_event_timeout)
    {
      // impossible. BUG if occurs
    }
    else if(e == motion_demo_event_step_complete)
    {
      _current_ndx++;
      if(_current_ndx < sizeof(_bwd_rotate))
      {
        execute_step();
      }
      else
      {
        enter_state(motion_demo_state_idle);
      }
    }
    break;
  }
}

static void
stepper_complete_handler(uint32_t e)
{
  motion_demo_event_handler(motion_demo_event_step_complete);
}

static void
motion_demo_timeout(SoftTimerElem* te)
{
  motion_demo_event_handler(motion_demo_event_timeout);
}

void
motion_demo_init(void)
{
  soft_timer_init_elem(&_tmr);
  _tmr.cb = motion_demo_timeout;

  event_register_handler(stepper_complete_handler, DISPATCH_EVENT_STEPPER_COMPLETE);

  enter_state(motion_demo_state_idle);
}