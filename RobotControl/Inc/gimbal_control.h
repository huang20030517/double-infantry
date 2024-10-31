
#ifndef __GIMBAL_CONTROL_H__
#define __GIMBAL_CONTROL_H__

#include "main.h"
#include "fdcan.h"
#include "usart.h"

#include "can_motor.h"
#include "pid.h"
#include "dr16.h"

extern float L_x_t;
extern int32_t  L_y_t;

extern float L_x_t_actual;

extern float yaw_init;

extern int16_t currents[4];
extern int16_t filtered_speed;

void Gimbal_Left_Head_Pid_Init();
void Gimbal_Left_Head_Motor_Control(MotorData_t *motor, uint8_t index);


#endif /*__ GIMBAL_CONTROL_H__ */
