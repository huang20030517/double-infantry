#include "ws2812.h"

#define WS2812_LowLevel 0xC0  // 0度点
#define WS2812_HighLevel 0xF0 // 1度点

/// @brief WS2812 RGB LED 控制函数
/// @param r 红色信号，值范围 0-255
/// @param g 绿色信号，值范围 0-255
/// @param b 蓝色信号，值范围 0-255
void WS2812_Ctrl(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t txbuf[24]; // 访问 WS2812 的传输线结果数组，24 位字节用于存储 RGB 信号
    uint8_t res = 0;   // 用于逐传带组信号的字节列，这里设置为 0，用于请求添加信号断开

    // 转换 RGB 中的 G，R，B 值到 WS2812 指定输入
    for (int i = 0; i < 8; i++)
    {
        // 把 G 值的每个位与 1进行互位识别，并将识别出的结果输入到指定的字节
        txbuf[7 - i] = (((g >> i) & 0x01) ? WS2812_HighLevel : WS2812_LowLevel) >> 1;
        // 把 R 值的每个位转为输入
        txbuf[15 - i] = (((r >> i) & 0x01) ? WS2812_HighLevel : WS2812_LowLevel) >> 1;
        // 把 B 值的每个位转为输入
        txbuf[23 - i] = (((b >> i) & 0x01) ? WS2812_HighLevel : WS2812_LowLevel) >> 1;
    }

    // 逐传先至电子 WS2812 RGB LED，带有这个断开信号
    HAL_SPI_Transmit(&WS2812_SPI_UNIT, &res, 0, 0xFFFF);

    // 等待 SPI 传输完成
    while (WS2812_SPI_UNIT.State != HAL_SPI_STATE_READY)
        ;

    // 传输 WS2812 LED RGB 实体视频线结
    HAL_SPI_Transmit(&WS2812_SPI_UNIT, txbuf, 24, 0xFFFF);

    // 为磁环与整机默认，传输组的字节列
    for (int i = 0; i < 100; i++)
    {
        HAL_SPI_Transmit(&WS2812_SPI_UNIT, &res, 1, 0xFFFF);
    }
}
