
#ifndef __GIMBAL_CONTROL_H__
#define __GIMBAL_CONTROL_H__

#include "main.h"
#include "fdcan.h"
#include "usart.h"

#include "motor.h"
#include "utils.h"
#include "pid.h"
#include "dr16.h"

void Process_Left_Head_Data(uint32_t id, uint8_t *data);
void Process_Right_Head_Data(uint32_t id, uint8_t *data);

#endif /*__ GIMBAL_CONTROL_H__ */
