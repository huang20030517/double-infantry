#include "can.h"

void Can_Init(void)
{
    FDCAN1_Init();
    FDCAN2_Init();
    FDCAN3_Init();
}

// 配置过滤器的函数
void Configure_FIFO_Filter(FDCAN_HandleTypeDef *hfdcan, FDCAN_FilterTypeDef *filterConfig, uint32_t id, uint32_t filterIndex, uint32_t fifo)
{
    filterConfig->IdType = FDCAN_STANDARD_ID;
    filterConfig->FilterIndex = filterIndex;
    filterConfig->FilterType = FDCAN_FILTER_MASK;
    filterConfig->FilterConfig = fifo == 0 ? FDCAN_FILTER_TO_RXFIFO0 : FDCAN_FILTER_TO_RXFIFO1;
    filterConfig->FilterID1 = id;
    filterConfig->FilterID2 = 0x7FF;

    HAL_FDCAN_ConfigFilter(hfdcan, filterConfig);
}

void FDCAN1_Init(void)
{
    FDCAN_InitTypeDef can_init;
    FDCAN_FilterTypeDef sFilterConfig;

    // 设置 FIFO 深度
    can_init.RxFifo0ElmtsNbr = 32;
    can_init.RxFifo1ElmtsNbr = 16;

    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE,
                                 FDCAN_REJECT_REMOTE);

    HAL_FDCAN_Init(&hfdcan1);

    // 配置 FIFO0 的过滤器
    uint32_t fifo0_ids[] = {0x201, 0x202, 0x203, 0x204};
    for (int i = 0; i < sizeof(fifo0_ids) / sizeof(fifo0_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan1, &sFilterConfig, fifo0_ids[i], i, 0);
    }

    // 配置 FIFO1 的过滤器
    uint32_t fifo1_ids[] = {0x209, 0x210};
    for (int i = 0; i < sizeof(fifo1_ids) / sizeof(fifo1_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan1, &sFilterConfig, fifo1_ids[i], i + 4, 1);
    }

    HAL_FDCAN_Start(&hfdcan1);

    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
}

void FDCAN2_Init(void)
{
    FDCAN_InitTypeDef can_init;
    FDCAN_FilterTypeDef sFilterConfig;

    // 设置 FIFO 深度
    can_init.RxFifo0ElmtsNbr = 32;
    can_init.RxFifo1ElmtsNbr = 16;

    HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE,
                                 FDCAN_REJECT_REMOTE);

    HAL_FDCAN_Init(&hfdcan2);

    // 配置 CAN2 的 FIFO0 过滤器
    uint32_t fifo0_ids[] = {0x205, 0x206, 0x207, 0x208};
    for (int i = 0; i < sizeof(fifo0_ids) / sizeof(fifo0_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan2, &sFilterConfig, fifo0_ids[i], i, 0);
    }

    // 配置 CAN2 的 FIFO1 过滤器
    uint32_t fifo1_ids[] = {0x141};
    for (int i = 0; i < sizeof(fifo1_ids) / sizeof(fifo1_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan2, &sFilterConfig, fifo1_ids[i], i + 4, 1);
    }

    HAL_FDCAN_Start(&hfdcan2);

    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
}

void FDCAN3_Init(void)
{
    FDCAN_InitTypeDef can_init;
    FDCAN_FilterTypeDef sFilterConfig;

    // 设置 FIFO 深度
    can_init.RxFifo0ElmtsNbr = 32;
    can_init.RxFifo1ElmtsNbr = 16;

    HAL_FDCAN_ConfigGlobalFilter(&hfdcan3,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE,
                                 FDCAN_REJECT_REMOTE);

    HAL_FDCAN_Init(&hfdcan3);

    // 配置 CAN3 的 FIFO0 过滤器
    uint32_t fifo0_ids[] = {0x205, 0x206, 0x207, 0x208};
    for (int i = 0; i < sizeof(fifo0_ids) / sizeof(fifo0_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan3, &sFilterConfig, fifo0_ids[i], i, 0);
    }

    // 配置 CAN3 的 FIFO1 过滤器
    uint32_t fifo1_ids[] = {0x141};
    for (int i = 0; i < sizeof(fifo1_ids) / sizeof(fifo1_ids[0]); i++)
    {
        Configure_FIFO_Filter(&hfdcan3, &sFilterConfig, fifo1_ids[i], i + 4, 1);
    }

    HAL_FDCAN_Start(&hfdcan3);

    HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef rx_heaber;
    uint8_t rx_data[8];

    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
    {
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_heaber, rx_data) == HAL_OK)
        {
            if (hfdcan->Instance == FDCAN1)
            {
                Process_Chassis_Motor_Data(rx_heaber.Identifier, rx_data);
            }
            else if (hfdcan->Instance == FDCAN2)
            {
                Process_Left_Head_Data(rx_heaber.Identifier, rx_data);
            }
            else if (hfdcan->Instance == FDCAN3)
            {
                Process_Right_Head_Data(rx_heaber.Identifier, rx_data);
            }
        }
    }
}
