/*
 * board_func.h
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#ifndef LED_INC_LED_FUNC_H_
#define LED_INC_LED_FUNC_H_

#include "main.h"

void Led_Init();
void Led_EnableConfigError();
void Led_EnableConfigRun();
void Led_ToggleConfigRun();
void Led_EnableError();
void Led_DisableError();
void Led_ToggleError();

#endif /* LED_INC_LED_FUNC_H_ */
