#ifndef __STEPPER_DEF_H__
#define __STEPPER_DEF_H__

#include "app_common.h"

typedef enum
{
  stepper_state_idle,
  stepper_state_stepping,
} stepper_state_t;

typedef struct
{
  stepper_state_t   state;
  uint8_t           steps_done;
} stepper_status_t;

extern void stepper_init(void);
extern bool stepper_step(bool dir, uint8_t steps);
extern bool stepper_cancel(void);
extern void stepper_status(stepper_status_t* status);

#endif /* !__STEPPER_DEF_H__ */
