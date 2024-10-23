
#ifndef __CAN_H__
#define __CAN_H__

#include "main.h"
#include "fdcan.h"
#include "usart.h"

// 模块化嵌套结构设计
// 定义单个电机的数据结构体
typedef struct
{
    float angle;         // 角度值（度）
    int16_t speed;       // 速度值（RPM）
    int16_t current;     // 电流值（mA）
    uint8_t temperature; // 温度值（摄氏度）
} MotorData_t;

// -------------------------- 云台模块（Gimbal） -------------------------- //
// 定义摩擦轮组的结构体（包含左电机和右电机）
typedef struct
{
    MotorData_t left_motor;  // 左摩擦轮电机
    MotorData_t right_motor; // 右摩擦轮电机
} FrictionWheel_t;

// 定义云台模块的结构体（包含摩擦轮、供弹电机和 Yaw 电机）
typedef struct
{
    MotorData_t launcher_motor;     // 供弹电机
    FrictionWheel_t friction_wheel; // 摩擦轮组（左电机+右电机）
    MotorData_t yaw_motor;          // Yaw 电机
} MotorGroup_t;

// -------------------------- 底盘模块（Chassis） -------------------------- //
// 定义底盘模块的结构体（包含4个驱动电机和1个 Yaw 电机）
typedef struct
{
    MotorData_t M0;  // M0 驱动电机
    MotorData_t M1;  // M1 驱动电机
    MotorData_t M2;  // M2 驱动电机
    MotorData_t M3;  // M3 驱动电机
    MotorData_t yaw; // 底盘的 Yaw 电机
} MotorChassis_t;

// -------------------------- 整体电机系统结构 -------------------------- //
// 定义电机系统的结构体，包含云台和底盘
typedef struct
{
    MotorGroup_t gimbal;    // 云台
    MotorChassis_t chassis; // 底盘
} MotorSystem_t;

// 声明一个全局电机系统实例
extern MotorSystem_t motor_system;
                   
void Can_Init(void);

#endif /*__ CAN_H__ */
