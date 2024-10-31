
#ifndef __CAN_H__
#define __CAN_H__

#include "main.h"
#include "fdcan.h"

#include "chassis_control.h"
#include "gimbal_control.h"

void Can_Init(void);

void FDCAN1_Init(void);
void FDCAN2_Init(void);
void FDCAN3_Init(void);

#endif /* __CAN_H__ */
