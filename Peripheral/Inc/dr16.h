
#ifndef __DBUS_H__
#define __DBUS_H__

#include "main.h"
#include "string.h"
#include "usart.h"
#include "stdbool.h"

typedef struct
{
    struct
    {
        float ch0;
        float ch1;
        float ch2;
        float ch3;
        float dial_wheel;
        uint8_t s1;
        uint8_t s2;
    } rc;

    struct
    {
        float x;
        float y;
        float z;
        uint8_t left_key : 1;
        uint8_t right_key : 1;
    } mouse;

    struct
    {
        uint8_t w : 1;
        uint8_t s : 1;
        uint8_t a : 1;
        uint8_t d : 1;
        uint8_t q : 1;
        uint8_t e : 1;
        uint8_t shift : 1;
        uint8_t ctrl : 1;
    } keyboard;

} _RC_Ctl_t;

extern _RC_Ctl_t RC_Ctl;

void DR16_Iint(void);

#endif /*__ DBUS_H__ */
