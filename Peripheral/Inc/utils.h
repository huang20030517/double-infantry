#ifndef __UTILS_H_
#define __UTILS_H_

#include "main.h"


float low_pass_filter(float input, float previous_output, float alpha);
float Update_Angle(float current_angle);

#endif /* __UTILS_H_ */