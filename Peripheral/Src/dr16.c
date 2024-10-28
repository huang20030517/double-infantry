#include "dr16.h"

_RC_Ctl_t RC_Ctl;

uint8_t RC_buffer[18];

void DR16_Iint(void)
{
    memset(&RC_Ctl, 0, sizeof(_RC_Ctl_t));
    memset(&RC_buffer, 0, sizeof(RC_buffer));

    HAL_UART_Receive_DMA(&huart5, RC_buffer, 18);
}

// 解析遥控器数据
bool _Parse_RC_Data(void)
{
    const float scale_factor = 1.0f / 660.0f;         // (1684.0 - 1024.0)
    const float mouse_scale_factor = 1.0f / 32768.0f; // 将鼠标值缩放到 -1 到 1 范围

    uint16_t rx = (RC_buffer[0] | (RC_buffer[1] << 8)) & 0x7FF;
    uint16_t ry = ((RC_buffer[1] >> 3) | (RC_buffer[2] << 5)) & 0x7FF;
    uint16_t lx = ((RC_buffer[2] >> 6) | (RC_buffer[3] << 2) | (RC_buffer[4] << 10)) & 0x7FF;
    uint16_t ly = ((RC_buffer[4] >> 1) | (RC_buffer[5] << 7)) & 0x7FF;
    uint16_t dial_wheel = (RC_buffer[17] << 8) | RC_buffer[16];

    if (rx < 364 || rx > 1684 ||
        ry < 364 || ry > 1684 ||
        lx < 364 || lx > 1684 ||
        ly < 364 || ly > 1684)
    {
        memset(&RC_Ctl, 0, sizeof(_RC_Ctl_t));
        HAL_UART_DMAStop(&huart5);
        __HAL_UART_CLEAR_OREFLAG(&huart5);
        HAL_UART_Receive_DMA(&huart5, RC_buffer, 18);

        return false;
    }

    RC_Ctl.rc.ch0 = (rx - 1024.0f) * scale_factor;
    RC_Ctl.rc.ch1 = (ry - 1024.0f) * scale_factor;
    RC_Ctl.rc.ch2 = (lx - 1024.0f) * scale_factor;
    RC_Ctl.rc.ch3 = (ly - 1024.0f) * scale_factor;
    RC_Ctl.rc.dial_wheel = (dial_wheel - 1024.0f) * scale_factor;

    RC_Ctl.rc.s1 = ((RC_buffer[5] >> 4) & 0x0C) >> 2;
    RC_Ctl.rc.s2 = (RC_buffer[5] >> 4) & 0x03;

    // 将鼠标数据转换为浮点值并缩放到 -1.0 到 1.0 范围
    RC_Ctl.mouse.x = (float)((int16_t)((RC_buffer[6]) | (RC_buffer[7] << 8))) * mouse_scale_factor;
    RC_Ctl.mouse.y = (float)((int16_t)((RC_buffer[8]) | (RC_buffer[9] << 8))) * mouse_scale_factor;
    RC_Ctl.mouse.z = (float)((int16_t)((RC_buffer[10]) | (RC_buffer[11] << 8))) * mouse_scale_factor;
    RC_Ctl.mouse.left_key = RC_buffer[12];
    RC_Ctl.mouse.right_key = RC_buffer[13];

    // 解析键盘数据
    RC_Ctl.keyboard.w = RC_buffer[14] & 0x01;
    RC_Ctl.keyboard.s = (RC_buffer[14] & 0x02) >> 1;
    RC_Ctl.keyboard.a = (RC_buffer[14] & 0x04) >> 2;
    RC_Ctl.keyboard.d = (RC_buffer[14] & 0x08) >> 3;
    RC_Ctl.keyboard.shift = (RC_buffer[14] & 0x10) >> 4;
    RC_Ctl.keyboard.ctrl = (RC_buffer[14] & 0x20) >> 5;
    RC_Ctl.keyboard.q = (RC_buffer[14] & 0x40) >> 6;
    RC_Ctl.keyboard.e = (RC_buffer[14] & 0x80) >> 7;

    return true;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle == &huart5)
    {

        if (!_Parse_RC_Data())
            return;
    }
}
