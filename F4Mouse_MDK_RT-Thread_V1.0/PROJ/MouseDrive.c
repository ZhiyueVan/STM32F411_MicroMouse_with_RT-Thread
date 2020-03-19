#include <MouseDrive.h>
#include "stm32f4xx.h"                  // Device header
#include <rtthread.h>

#include <arm_math.h>

#define delay_us(time) rt_hw_us_delay(time)

float Speed_ref;
float Speed_error;
float Speed_current;

void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}

void Speed_PID(void)
{
	arm_pid_instance_f32 S_PID;
	S_PID.Kp=Speed_Kp;
	S_PID.Ki=Speed_Ki;
	S_PID.Kd=Speed_Kd;
	
	arm_pid_init_f32(&S_PID, 1);
	Speed_error=Speed_current-Speed_ref;
	arm_pid_f32(&S_PID, Speed_error);
}