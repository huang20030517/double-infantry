#ifndef __DR16_H__
#define __DR16_H__

#include "main.h"
#include "string.h"
#include "usart.h"
#include "stdbool.h"

// -------------------------- 遥控器通道（RC Channel） -------------------------- //
typedef struct
{
    float ch0;        // 遥控器通道 0
    float ch1;        // 遥控器通道 1
    float ch2;        // 遥控器通道 2
    float ch3;        // 遥控器通道 3
    float dial_wheel; // 拨轮
    uint8_t s1;       // 开关 1
    uint8_t s2;       // 开关 2
} RC_Channel_t;

// -------------------------- 鼠标模块（Mouse） -------------------------- //
typedef struct
{
    float x;               // 鼠标 X 轴
    float y;               // 鼠标 Y 轴
    float z;               // 鼠标 Z 轴
    uint8_t left_key : 1;  // 左键
    uint8_t right_key : 1; // 右键
} Mouse_t;

// -------------------------- 键盘模块（Keyboard） -------------------------- //
typedef struct
{
    uint8_t w : 1;     // W 键
    uint8_t s : 1;     // S 键
    uint8_t a : 1;     // A 键
    uint8_t d : 1;     // D 键
    uint8_t q : 1;     // Q 键
    uint8_t e : 1;     // E 键
    uint8_t shift : 1; // Shift 键
    uint8_t ctrl : 1;  // Ctrl 键
} Keyboard_t;

// -------------------------- 遥控器数据（RC Control） -------------------------- //
typedef struct
{
    RC_Channel_t rc;     // 遥控器通道数据
    Mouse_t mouse;       // 鼠标数据
    Keyboard_t keyboard; // 键盘数据
} _RC_Ctl_t;

// -------------------------- 全局变量声明 -------------------------- //
extern _RC_Ctl_t RC_Ctl;

extern uint8_t RC_buffer[18];

// -------------------------- 函数声明 -------------------------- //

void DR16_Init(void);

#endif /* __DR16_H__ */
