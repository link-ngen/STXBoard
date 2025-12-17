/*
 * board_func.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include "../inc/led_func.h"

void Led_Init()
{
  HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CONFIG_LED_GPIO_Port, CONFIG_LED_Pin, GPIO_PIN_RESET);
}

void Led_EnableConfigured()
{
  HAL_GPIO_WritePin(CONFIG_LED_GPIO_Port, CONFIG_LED_Pin, GPIO_PIN_RESET);
}

void Led_EnableConfigRun()
{
  HAL_GPIO_WritePin(CONFIG_LED_GPIO_Port, CONFIG_LED_Pin, GPIO_PIN_SET);
}

void Led_ToggleConfigRun()
{
  HAL_GPIO_TogglePin(CONFIG_LED_GPIO_Port, CONFIG_LED_Pin);
}

void Led_EnableError()
{
  HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_SET);
}

void Led_DisableError()
{
  HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_RESET);
}

void Led_ToggleError()
{
  HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
}
