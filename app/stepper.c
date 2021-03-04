#include "tim.h"
#include "gpio.h"
#include "stepper.h"
#include "event_dispatcher.h"
#include "event_list.h"

static stepper_state_t    _state;

static void
tim_update_handler(uint32_t e)
{
  HAL_TIM_Base_Stop_IT(&htim1);
  //HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
  _state = stepper_state_idle;
  event_set(1 << DISPATCH_EVENT_STEPPER_COMPLETE);
}

//
// driver callback in IRQ context
//
void
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  event_set(1 << DISPATCH_EVENT_TIMER_UPDATE);
}

void
stepper_init(void)
{
  //
  // TIM1 is already set up for 800Hz frequency
  //
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  _state = stepper_state_idle;
  event_register_handler(tim_update_handler, DISPATCH_EVENT_TIMER_UPDATE);

  //
  // enable stepper driver
  //
  HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_RESET);
}

bool
stepper_step(bool dir, uint8_t steps)
{
  if(_state != stepper_state_idle)
  {
    return false;
  }

  HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, dir ? GPIO_PIN_SET : GPIO_PIN_RESET);

  __HAL_TIM_SET_COUNTER(&htim1, 0);
  htim1.Instance->RCR = steps;
  htim1.Instance->EGR = TIM_EGR_UG;

  _state = stepper_state_stepping;

  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);

  HAL_TIM_Base_Start_IT(&htim1);

  //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  return true;
}

bool
stepper_cancel(void)
{
  if(_state == stepper_state_idle)
  {
    return false;
  }

  //
  // FIXME
  // how to implement this?
  //
  // we need a mechanism to keep track of
  // the exact number of steps made
  // before cancelling
  //

#if 0
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Stop(&htim1);
#endif

  return true;
}

void
stepper_status(stepper_status_t* status)
{
  status->state = _state;
  status->steps_done = 0;   // FIXME
}
