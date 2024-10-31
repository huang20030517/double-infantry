#include "dr16.h"

// 全局变量
_RC_Ctl_t RC_Ctl;
uint8_t RC_buffer[18];

// 常量定义
#define CHANNEL_MIN_VALUE 364
#define CHANNEL_MAX_VALUE 1684
#define SCALE_FACTOR (1.0f / 660.0f)
#define MOUSE_SCALE_FACTOR (1.0f / 32768.0f)
#define MAX_ERROR_COUNT 5

/**
 * @brief 初始化 DR16 遥控器
 * @details 配置 UART 以 DMA 方式接收 DR16 遥控器的数据，并初始化控制数据和接收缓冲区。
 */
void DR16_Init(void)
{
    memset(&RC_Ctl, 0, sizeof(_RC_Ctl_t));
    memset(RC_buffer, 0, sizeof(RC_buffer));
    HAL_UART_Receive_DMA(&huart5, RC_buffer, sizeof(RC_buffer));
}

/**
 * @brief 重置 RC 数据并重新启动 DMA
 * @details 当接收到的遥控器数据无效时，重置所有遥控器的控制数据并重新启动 DMA 接收。
 */
void Reset_RC_Data(void)
{
    memset(&RC_Ctl, 0, sizeof(_RC_Ctl_t));
    HAL_UART_DMAStop(&huart5);
    __HAL_UART_CLEAR_OREFLAG(&huart5);
    HAL_UART_Receive_DMA(&huart5, RC_buffer, sizeof(RC_buffer));
}

/**
 * @brief 解析单个键盘数据字节
 * @param key_byte 键盘数据的原始字节
 * @details 从键盘数据中提取各个按键的状态并存储到 RC_Ctl 结构体中。
 */
void Parse_Keyboard_Data(uint8_t key_byte)
{
    RC_Ctl.keyboard = (Keyboard_t){
        .w = key_byte & 0x01,
        .s = (key_byte & 0x02) >> 1,
        .a = (key_byte & 0x04) >> 2,
        .d = (key_byte & 0x08) >> 3,
        .q = (key_byte & 0x40) >> 6,
        .e = (key_byte & 0x80) >> 7,
        .shift = (key_byte & 0x10) >> 4,
        .ctrl = (key_byte & 0x20) >> 5};
}

/**
 * @brief 解析遥控器通道数据并缩放到 [-1, 1] 范围
 * @param raw_value 遥控器通道的原始数据值
 * @return 缩放后的通道值，范围为 [-1, 1]
 */
float Parse_Channel(uint16_t raw_value)
{
    return (raw_value - 1024.0f) * SCALE_FACTOR;
}

/**
 * @brief 解析 RC 数据包
 * @details 解析 DR16 遥控器数据包并将其存储在 RC_Ctl 结构体中，如果数据不合法，则会重置数据。
 * @return 解析是否成功，成功返回 true，失败返回 false
 */
bool Parse_RC_Data(void)
{
    static uint8_t error_count = 0;

    uint16_t channels[4] = {
        (RC_buffer[0] | (RC_buffer[1] << 8)) & 0x7FF,
        ((RC_buffer[1] >> 3) | (RC_buffer[2] << 5)) & 0x7FF,
        ((RC_buffer[2] >> 6) | (RC_buffer[3] << 2) | (RC_buffer[4] << 10)) & 0x7FF,
        ((RC_buffer[4] >> 1) | (RC_buffer[5] << 7)) & 0x7FF};

    for (uint8_t i = 0; i < 4; ++i)
    {
        if (channels[i] < CHANNEL_MIN_VALUE || channels[i] > CHANNEL_MAX_VALUE)
        {
            error_count++;
            if (error_count >= MAX_ERROR_COUNT)
            {
                Reset_RC_Data();
                error_count = 0;
            }
            return false;
        }
    }
    error_count = 0;

    RC_Ctl.rc.ch0 = Parse_Channel(channels[0]);
    RC_Ctl.rc.ch1 = Parse_Channel(channels[1]);
    RC_Ctl.rc.ch2 = Parse_Channel(channels[2]);
    RC_Ctl.rc.ch3 = Parse_Channel(channels[3]);

    uint16_t dial_wheel = (RC_buffer[17] << 8) | RC_buffer[16];
    RC_Ctl.rc.dial_wheel = Parse_Channel(dial_wheel);

    RC_Ctl.rc.s1 = (RC_buffer[5] >> 4) & 0x03;
    RC_Ctl.rc.s2 = (RC_buffer[5] >> 6) & 0x03;

    RC_Ctl.mouse.x = (float)((int16_t)(RC_buffer[6] | (RC_buffer[7] << 8))) * MOUSE_SCALE_FACTOR;
    RC_Ctl.mouse.y = (float)((int16_t)(RC_buffer[8] | (RC_buffer[9] << 8))) * MOUSE_SCALE_FACTOR;
    RC_Ctl.mouse.z = (float)((int16_t)(RC_buffer[10] | (RC_buffer[11] << 8))) * MOUSE_SCALE_FACTOR;
    RC_Ctl.mouse.left_key = RC_buffer[12] & 0x01;
    RC_Ctl.mouse.right_key = RC_buffer[13] & 0x01;

    Parse_Keyboard_Data(RC_buffer[14]);

    return true;
}

/**
 * @brief UART 接收完成回调函数
 * @param UartHandle UART 句柄
 * @details 在 UART 完成数据接收后调用，如果数据包有效则解析，否则重置遥控器数据。
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle == &huart5)
    {
        if (!Parse_RC_Data())
        {
            Reset_RC_Data();
        }
    }
}
