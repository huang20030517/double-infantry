

#ifndef __CAN_H__
#define __CAN_H__

#include "main.h"
#include "fdcan.h"
#include "usart.h"
#include "pid.h"

// 定义单个电机的数据结构体
typedef struct
{
    float angle;         // 角度值
    int16_t speed;       // 速度值
    int16_t current;     // 电流值
    uint8_t temperature; // 温度值
} MotorData_t;

// -------------------------- 云台模块（Gimbal） -------------------------- //
// 存储云台的 4 个电机：供弹电机、左摩擦轮、右摩擦轮、Yaw 电机
typedef struct
{
    MotorData_t motors[4];      // motors[0]: 供弹电机, motors[1]: 左摩擦轮, motors[2]: 右摩擦轮, motors[3]: Yaw 电机
    pid_controler pid_group[3]; // pid_group[0]: 供弹电机, pid_group[1]: 左摩擦轮, pid_group[2]: 右摩擦轮
    pid_controler yaw_pid[2];   // yaw_pid[0]: 位置环, yaw_pid[1]: 速度环

    // Pitch
} GimbalHead_t;

// -------------------------- 底盘模块（Chassis） -------------------------- //
// 存储底盘的 4 个驱动电机，并单独存储 Yaw 电机
typedef struct
{
    MotorData_t motors[4]; // 4 个驱动电机（motors[0]-motors[3]）
    MotorData_t yaw_motor; // 底盘的大 Yaw 电机
    // 超电
} MotorChassis_t;

// -------------------------- 电机系统结构体 ------------------------------ //
typedef struct
{
    GimbalHead_t left_head;  // 左头电机组
    GimbalHead_t right_head; // 右头电机组

} MotorGroup_t;

// 包含云台模块和底盘模块
typedef struct
{
    MotorGroup_t gimbal;    // 云台模块
    MotorChassis_t chassis; // 底盘模块
} MotorSystem_t;

// 声明一个全局电机系统实例
extern MotorSystem_t motor_system;

void Can_Init(void);
void Motor_ProcessCANMessage(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
HAL_StatusTypeDef Send_Motor_Currents(int16_t *currents, uint8_t num_motors);
HAL_StatusTypeDef Send_MultiTurn_Position_Control_Command(int32_t angleControl, uint16_t maxSpeed);

#endif /*__ CAN_H__ */
