/**
  ******************************************************************************
  * @file    reference.h
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef __REFERENCE_H__
#define __REFERENCE_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <string.h>

typedef struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;
 uint16_t stage_remain_time;
uint64_t SyncTimeStamp;
} ext_game_status_t;

typedef  struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t remain_HP;
	uint16_t max_HP;
	
	uint16_t shooter_id1_17mm_cooling_rate;
	uint16_t shooter_id1_17mm_cooling_limit;
	uint16_t shooter_id1_17mm_speed_limit;
	
	uint16_t shooter_id2_17mm_cooling_rate;
	uint16_t shooter_id2_17mm_cooling_limit;
	uint16_t shooter_id2_17mm_speed_limit;
	
	uint16_t shooter_id1_42mm_cooling_rate;
	uint16_t shooter_id1_42mm_cooling_limit;
	uint16_t shooter_id1_42mm_speed_limit;
	uint16_t chassis_power_limit;
	
	uint8_t mains_power_gimbal_output : 1;
	uint8_t mains_power_chassis_output : 1;
	uint8_t mains_power_shooter_output : 1;
}ext_game_robot_status_t;

typedef  struct
{
	uint16_t chassis_volt; 
	uint16_t chassis_current; 
	float chassis_power; 
	uint16_t chassis_power_buffer; 
	uint16_t shooter_id1_17mm_cooling_heat;
	uint16_t shooter_id2_17mm_cooling_heat;
	uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;

extern ext_game_robot_status_t reference_game_robot_status;
extern ext_power_heat_data_t reference_power_heat_data;
extern ext_game_status_t reference_game_status;

void reference_init();
void reference_callback();

#endif