
#ifndef __CHASSIS_CONTROL_H__
#define __CHASSIS_CONTROL_H__


#include "main.h"
#include "fdcan.h"
#include "usart.h"

#include "motor.h"
#include "utils.h"
#include "pid.h"
#include "can.h"


void Process_Chassis_Motor_Data(uint32_t id, uint8_t *data);

#endif /* __CHASSIS_CONTROL_H__ */
