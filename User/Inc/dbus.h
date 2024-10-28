/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DBUS_H__
#define __DBUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "stdbool.h"
#include "stdio.h"



/* USER CODE BEGIN Prototypes */

//typedef   signed          char    bool_t;

#define    FALSE    0
#define    TRUE     1

//bool_t REMOTE_IfDataError( void );

typedef struct
{
	struct
	{ 
		float ch0;
		float ch1;
		float ch2;
		float ch3;
		float dial_wheel;
		unsigned short s1;
		unsigned short s2;
		
	}rc;
	
	struct
	{ 
		int16_t x;
		int16_t y;
		int16_t z ;
		int16_t left_key ;
		int16_t right_key ;	
	}mouse;

	struct
	{
		uint8_t w;
		uint8_t s;
		uint8_t a;
		uint8_t d;
		uint8_t q;
		uint8_t e;
		uint8_t shift;
		uint8_t ctrl;
	}keyboard;
	
}_RC_Ctl_t;


void dbus_init(void);

bool REMOTE_IfDataError( void );
void dbus_callback(void);
extern uint8_t dbus_buffer[18];	
extern _RC_Ctl_t RC_Ctl;

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ DBUS_H__ */

