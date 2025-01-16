/**************************************************************************************

 Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

 ***************************************************************************************

 $Id: OS_SPICustom.c 12305 2018-08-09 12:54:55Z Robert $:

 Description:
 Implementation of the custom SPI abstration layer

 Changes:
 Date        Description
 -----------------------------------------------------------------------------------
 2018-07-26  Added return value to OS_SpiInit()
 2014-08-27  created

 **************************************************************************************/

/*****************************************************************************/
/*! \file OS_SPICustom.c
 *    Sample SPI abstraction layer. Implementation must be done
 *    according to used target system                                         */
/*****************************************************************************/

#include "OS_Spi.h"
#include "OS_Dependent.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

//#ifdef CIFX_TOOLKIT_HWIF
//  #error "Implement SPI target system abstraction in this file"
//#endif

extern SPI_HandleTypeDef SpiHandle;
static UBaseType_t uxSavedInterruptStatus;
/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
 *    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Initialize SPI components
 *   \param pvOSDependent OS Dependent parameter
 *   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
long OS_SpiInit(void *pvOSDependent)
{
  /* initialize SPI device */
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
  return 0;
}

/*****************************************************************************/
/*! Assert chip select
 *   \param pvOSDependent OS Dependent parameter to identify card             */
/*****************************************************************************/
void OS_SpiAssert(void *pvOSDependent)
{
  /* assert chip select */
  HAL_GPIO_WritePin(SPM_CS_GPIO_Port, SPM_CS_Pin, GPIO_PIN_RESET);
}

/*****************************************************************************/
/*! Deassert chip select
 *   \param pvOSDependent OS Dependent parameter to identify card             */
/*****************************************************************************/
void OS_SpiDeassert(void *pvOSDependent)
{
  /* deassert chip select */
  HAL_GPIO_WritePin(SPM_CS_GPIO_Port, SPM_CS_Pin, GPIO_PIN_SET);
}

/*****************************************************************************/
/*! Lock the SPI bus
 *   \param pvOSDependent OS Dependent parameter                              */
/*****************************************************************************/
void OS_SpiLock(void *pvOSDependent)
{
  /* lock access to SPI device */
  if(xPortIsInsideInterrupt())
  {
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
  }
  else
  {
    taskENTER_CRITICAL();
  }
}

/*****************************************************************************/
/*! Unlock the SPI bus
 *   \param pvOSDependent OS Dependent parameter                              */
/*****************************************************************************/
void OS_SpiUnlock(void *pvOSDependent)
{
  /* unlock access to SPI device */
  if(xPortIsInsideInterrupt())
  {
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
  }
  else
  {
    taskEXIT_CRITICAL();
  }
}

/*****************************************************************************/
/*! Transfer byte stream via SPI
 *   \param pvOSDependent OS Dependent parameter to identify card
 *   \param pbSend        Send buffer (NULL for polling)
 *   \param pbRecv        Receive buffer (NULL if discard)
 *   \param ulLen         Length of SPI transfer                              */
/*****************************************************************************/
void OS_SpiTransfer(void *pvOSDependent, uint8_t *pbSend, uint8_t *pbRecv, uint32_t ulLen)
{
  HAL_StatusTypeDef tRet = HAL_OK;
  static uint32_t ulErrCnt = 0;

  /*no receive data requested (just transmit) */
  if(NULL == pbRecv)
  {
    tRet = HAL_SPI_Transmit_DMA(&SpiHandle, pbSend, (uint16_t) ulLen);
  }
  /*no transmit data (just receive) */
  else if(NULL == pbSend)
  {
    tRet = HAL_SPI_Receive_DMA(&SpiHandle, pbRecv, (uint16_t) ulLen);
  }
  else
  {
    tRet = HAL_SPI_TransmitReceive_DMA(&SpiHandle, pbSend, pbRecv, (uint16_t) ulLen);
  }

  if(HAL_OK != tRet)
  {
    ulErrCnt++;
  }

  /*blocking SPI transfer, wait until all bytes are transferred*/
  while(HAL_SPI_STATE_READY != HAL_SPI_GetState(&SpiHandle));
}
/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
