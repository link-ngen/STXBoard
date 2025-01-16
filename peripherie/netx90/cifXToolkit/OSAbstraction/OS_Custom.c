/**************************************************************************************

 Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

 ***************************************************************************************

 $Id: OS_Custom.c 14561 2022-07-26 13:22:39Z RMayer $:

 Description:
 Target system abstraction layer

 Changes:
 Date        Description
 -----------------------------------------------------------------------------------
 2022-04-14  Added options and functions to handle cached I/O buffer access via PLC functions
 2021-09-01  - updated function parameters to match definitions in OS_Dependent.h.
 - changed OS-Time() parameters to 64Bit data types
 2011-12-13  added OS_Time() function body
 2006-08-07  initial version

 **************************************************************************************/

/*****************************************************************************/
/*! \file OS_Custom.c
 *    Sample Target system abstraction layer. Implementation must be done
 *    according to used target system                                         */
/*****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "OS_Includes.h"
#include "cifXToolkit.h"
#include "cifXErrors.h"
#include "OS_Dependent.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"

static int s_fOSInitDone = 0;

/* Interrupt members (static variables sufficient as only one DPM is handled) */
static DEVICEINSTANCE *s_ptIrqDevInst = NULL; /*<! Interrupt context */
static TaskHandle_t s_xDsrTask = NULL; /*<! DSR task handle, notified by DPM IRQ */
static bool s_fDSRActive = false;

#define DIRQ_Handler  HAL_GPIO_EXTI_Callback

/* idle time processing, provided in the application */
void (*g_pfnOSAL_Idle)(void);

//#error "Implement target system abstraction in this file"

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
 *    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Memory allocation function
 *   \param ulSize    Length of memory to allocate
 *   \return Pointer to allocated memory                                      */
/*****************************************************************************/
void* OS_Memalloc(uint32_t ulSize)
{
  void *pt = pvPortMalloc(ulSize);
  /* there are some people who don't catch NULL return of malloc, therefore we catch
   * it here */
  while(NULL == pt);
  return pt;
}

/*****************************************************************************/
/*! Memory freeing function
 *   \param pvMem Memory block to free                                        */
/*****************************************************************************/
void OS_Memfree(void *pvMem)
{
  vPortFree(pvMem);
}

/*****************************************************************************/
/*! Memory reallocating function (used for resizing dynamic toolkit arrays)
 *   \param pvMem     Memory block to resize
 *   \param ulNewSize new size of the memory block
 *   \return pointer to the resized memory block                              */
/*****************************************************************************/
void* OS_Memrealloc(void *pvMem, uint32_t ulNewSize)
{
  if(ulNewSize == 0)
  {
    vPortFree(pvMem);
    return NULL;
  }

  void *pvRet;
  pvRet = pvPortMalloc(ulNewSize);
  if(pvRet)
  {
    /* zero the memory */
    if(pvMem != NULL)
    {
      memcpy(pvRet, pvMem, ulNewSize);
      vPortFree(pvMem);
    }
  }
  return pvRet;
}

/*****************************************************************************/
/*! Memory setting
 *   \param pvMem     Memory block
 *   \param bFill     Byte to use for memory initialization
 *   \param ulSize    Memory size for initialization)                         */
/*****************************************************************************/
void OS_Memset(void *pvMem, unsigned char bFill, uint32_t ulSize)
{
  memset(pvMem, bFill, ulSize);
}

/*****************************************************************************/
/*! Copy memory from one block to another
 *   \param pvDest    Destination memory block
 *   \param pvSrc     Source memory block
 *   \param ulSize    Copy size in bytes                                      */
/*****************************************************************************/
void OS_Memcpy(void *pvDest, void *pvSrc, uint32_t ulSize)
{
  memcpy(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Compare two memory blocks
 *   \param pvBuf1    First memory block
 *   \param pvBuf2    Second memory block
 *   \param ulSize    Compare size in bytes
 *   \return 0 if both buffers are equal                                      */
/*****************************************************************************/
int OS_Memcmp(void *pvBuf1, void *pvBuf2, uint32_t ulSize)
{
  return memcmp(pvBuf1, pvBuf2, ulSize);
}

/*****************************************************************************/
/*! Move memory
 *   \param pvDest    Destination memory
 *   \param pvSrc     Source memory
 *   \param ulSize    Size in byte to move                                    */
/*****************************************************************************/
void OS_Memmove(void *pvDest, void *pvSrc, uint32_t ulSize)
{
  memmove(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Sleep for a specific time
 *   \param ulSleepTimeMs  Time in ms to sleep for                            */
/*****************************************************************************/
void OS_Sleep(uint32_t ulSleepTimeMs)
{
  vTaskDelay(pdMS_TO_TICKS(ulSleepTimeMs));
}

/*****************************************************************************/
/*! Sleep for a specific time
 *   \param ulNum  number of objects                            */
/*   \param ulSize size of each object 							*/
/*****************************************************************************/
void* OS_Memcalloc(uint32_t ulNum, uint32_t ulSize)
{
  return pvPortCalloc(ulNum, ulSize);
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
 *   \return Current counter value (resolution of this value will influence
 *           timeout monitoring in driver/toolkit functions(                  */
/*****************************************************************************/
uint32_t OS_GetMilliSecCounter(void)
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/*****************************************************************************/
/*! Create an auto reset event
 *   \return handle to the created event                                      */
/*****************************************************************************/
void* OS_CreateEvent(void)
{
  return (void*)xSemaphoreCreateBinary();
}

/*****************************************************************************/
/*! Signal event
 *     \param pvEvent Handle to event                                         */
/*****************************************************************************/
void OS_SetEvent(void* pvEvent)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(pdFALSE == xPortIsInsideInterrupt())
  {
    (void)xSemaphoreGive((SemaphoreHandle_t)pvEvent);
  }
  else
  {
    (void)xSemaphoreGiveFromISR((SemaphoreHandle_t)pvEvent, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
  }
}

/*****************************************************************************/
/*! Reset event
 *     \param pvEvent Handle to event                                         */
/*****************************************************************************/
void OS_ResetEvent(void* pvEvent)
{
  UNREFERENCED_PARAMETER(pvEvent);
}

/*****************************************************************************/
/*! Delete event
 *     \param pvEvent Handle to event                                         */
/*****************************************************************************/
void OS_DeleteEvent(void* pvEvent)
{
  vSemaphoreDelete((SemaphoreHandle_t)pvEvent);
}

/*****************************************************************************/
/*! Wait for event
 *     \param pvEvent   Handle to event
 *     \param ulTimeout Timeout in ms to wait for event
 *     \return CIFX_EVENT_SIGNALLED if event was set, CIFX_EVENT_TIMEOUT otherwise */
/*****************************************************************************/
uint32_t OS_WaitEvent(void* pvEvent, uint32_t ulTimeout)
{
  BaseType_t tResult = pdPASS;
  if(0 == ulTimeout)
  {
    // A block time of zero can be used to poll the semaphore.
    tResult = xSemaphoreTake((SemaphoreHandle_t)pvEvent, 0);
    if(pdFAIL == tResult)
    {
      return CIFX_EVENT_TIMEOUT;
    }
  }
  else
  {
    tResult = xSemaphoreTake((SemaphoreHandle_t)pvEvent, pdMS_TO_TICKS(ulTimeout));
    if(pdFAIL == tResult)
    {
      return CIFX_EVENT_TIMEOUT;
    }
  }
  return CIFX_EVENT_SIGNALLED;
}

/*****************************************************************************/
/*! Compare two ASCII string
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strcmp(const char *pszBuf1, const char *pszBuf2)
{
  return strcmp(pszBuf1, pszBuf2);
}

/*****************************************************************************/
/*! Compare characters of two strings without regard to case
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \param ulLen     Number of characters to compare
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strnicmp(const char *pszBuf1, const char *pszBuf2, uint32_t ulLen)
{
  return strncasecmp(pszBuf1, pszBuf2, ulLen);
}

/*****************************************************************************/
/*! Query the length of an ASCII string
 *   \param szText    ASCII string
 *   \return character count of szText                                        */
/*****************************************************************************/
int OS_Strlen(const char *szText)
{
  return strlen(szText);
}

/*****************************************************************************/
/*! Copies one string to another monitoring the maximum length of the target
 *   buffer.
 *   \param szDest    Destination buffer
 *   \param szSource  Source buffer
 *   \param ulLen     Maximum length to copy
 *   \return pointer to szDest                                                */
/*****************************************************************************/
char* OS_Strncpy(char *szDest, const char *szSource, uint32_t ulLen)
{
  return strncpy(szDest, szSource, ulLen);
}

/*****************************************************************************/
/*! Create an interrupt safe locking mechanism (Spinlock/critical section)
 *   \return handle to the locking object                                     */
/*****************************************************************************/
void* OS_CreateLock(void)
{
  return (void*) 0xDEADDA7A;
}

/*****************************************************************************/
/*! Enter a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_EnterLock(void *pvLock)
{
  taskENTER_CRITICAL();
}

/*****************************************************************************/
/*! Leave a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_LeaveLock(void *pvLock)
{
  taskEXIT_CRITICAL();
}

/*****************************************************************************/
/*! Delete a critical section/spinlock object
 *   \param pvLock Handle to the locking object being deleted                 */
/*****************************************************************************/
void OS_DeleteLock(void *pvLock)
{
  UNREFERENCED_PARAMETER(pvLock);
}

/*****************************************************************************/
/*! Create an Mutex object for locking code sections
 *   \return handle to the mutex object                                       */
/*****************************************************************************/
void* OS_CreateMutex(void)
{
  return (void*) xSemaphoreCreateMutex();
}

/*****************************************************************************/
/*! Wait for mutex
 *   \param pvMutex    Handle to the Mutex locking object
 *   \param ulTimeout  Wait timeout
 *   \return !=0 on succes                                                    */
/*****************************************************************************/
int OS_WaitMutex(void *pvMutex, uint32_t ulTimeout)
{
  BaseType_t tRes = xSemaphoreTake((SemaphoreHandle_t )pvMutex, (TickType_t)pdMS_TO_TICKS(ulTimeout));
  /* Map return value for cifX Toolkit */
  return (pdPASS == tRes) ? pdPASS : pdFAIL;
}

/*****************************************************************************/
/*! Release a mutex section section
 *   \param pvMutex Handle to the locking object                              */
/*****************************************************************************/
void OS_ReleaseMutex(void *pvMutex)
{
  (void) xSemaphoreGive((SemaphoreHandle_t )pvMutex);
}

/*****************************************************************************/
/*! Delete a Mutex object
 *   \param pvMutex Handle to the mutex object being deleted                  */
/*****************************************************************************/
void OS_DeleteMutex(void *pvMutex)
{
  vSemaphoreDelete((SemaphoreHandle_t )pvMutex);
}

/*****************************************************************************/
/*! Opens a file in binary mode
 *   \param szFile     Full file name (incl. path if necessary) of the file to open
 *   \param pulFileLen Returned length of the opened file
 *   \return handle to the file, NULL mean file could not be opened           */
/*****************************************************************************/
void* OS_FileOpen(char *szFile, uint32_t *pulFileLen)
{
  UNREFERENCED_PARAMETER(szFile);
  UNREFERENCED_PARAMETER(pulFileLen);
  return NULL;
}

/*****************************************************************************/
/*! Closes a previously opened file
 *   \param pvFile Handle to the file being closed                            */
/*****************************************************************************/
void OS_FileClose(void *pvFile)
{
  UNREFERENCED_PARAMETER(pvFile);
}

/*****************************************************************************/
/*! Read a specific amount of bytes from the file
 *   \param pvFile   Handle to the file being read from
 *   \param ulOffset Offset inside the file, where read starts at
 *   \param ulSize   Size in bytes to be read
 *   \param pvBuffer Buffer to place read bytes in
 *   \return number of bytes actually read from file                          */
/*****************************************************************************/
uint32_t OS_FileRead(void *pvFile, uint32_t ulOffset, uint32_t ulSize, void *pvBuffer)
{
  UNREFERENCED_PARAMETER(pvFile);
  UNREFERENCED_PARAMETER(ulOffset);
  UNREFERENCED_PARAMETER(ulSize);
  UNREFERENCED_PARAMETER(pvBuffer);
  return 0;
}

/*****************************************************************************/
/*! OS specific initialization (if needed), called during cifXTKitInit()
 *   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t OS_Init(void)
{
  s_fOSInitDone = 1;

  return CIFX_NO_ERROR;
}

/*****************************************************************************/
/*! OS specific de-initialization (if needed), called during cifXTKitInit()  */
/*****************************************************************************/
void OS_Deinit(void)
{
  s_fOSInitDone = 0;
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * write back all BAR's (Base address registers), Interrupt and Command
 * Register. These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent OS Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \param pvPCIConfig   Configuration returned by OS_ReadPCIConfig
 *                        (implementation dependent)                          */
/*****************************************************************************/
void OS_WritePCIConfig(void *pvOSDependent, void *pvPCIConfig)
{
  UNREFERENCED_PARAMETER(pvOSDependent);
  UNREFERENCED_PARAMETER(pvPCIConfig);
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * read all BAR's (Base address registers), Interrupt and Command Register.
 * These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent OS Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \return pointer to stored register copies (implementation dependent)     */
/*****************************************************************************/
void* OS_ReadPCIConfig(void *pvOSDependent)
{
  UNREFERENCED_PARAMETER(pvOSDependent);
  return NULL;
}

/*****************************************************************************/
/*! This function Maps a DPM pointer to a user application if needed.
 *   This example just returns the pointer valid inside the driver.
 *   \param pvDriverMem   Pointer to be mapped
 *   \param ulMemSize     Size to be mapped
 *   \param ppvMappedMem  Returned mapped pointer (usable by application)
 *   \param pvOSDependent OS Dependent variable passed during call to
 *                        cifXTKitAddDevice
 *   \param fCached       Caching option (0 = do not use caching)
 *   \return Handle that is needed for unmapping NULL is a mapping failure    */
/*****************************************************************************/
void* OS_MapUserPointer(void *pvDriverMem, uint32_t ulMemSize, void **ppvMappedMem, void *pvOSDependent, unsigned char fCached)
{
  UNREFERENCED_PARAMETER(ulMemSize);
  UNREFERENCED_PARAMETER(pvOSDependent);
  /* We are running in user mode, so it is not necessary to map anything to user space */
  *ppvMappedMem = pvDriverMem;

  return pvDriverMem;
}

/*****************************************************************************/
/*! This function unmaps a previously mapped user application pointer
 *   \param phMapping      Handle that was returned by OS_MapUserPointer
 *   \param pvOSDependent  OS Dependent variable passed during call to
 *                         cifXTKitAddDevice
 *   \return !=0 on success                                                   */
/*****************************************************************************/
int OS_UnmapUserPointer(void *phMapping, void *pvOSDependent)
{
  UNREFERENCED_PARAMETER(phMapping);
  UNREFERENCED_PARAMETER(pvOSDependent);
  /* We are running in user mode, so it is not necessary to map anything to user space */
  return 1;
}

/*****************************************************************************/
/*! This function flushes a cached memory area to the device buffer
 *   \param pvMem      Pointer to the cached memory
 *   \param ulMemSize  Length of the cached memory area                   */
/*****************************************************************************/
void OS_FlushCacheMemory_ToDevice(void *pvMem, unsigned long ulMemSize)
{
  UNREFERENCED_PARAMETER(pvMem);
}

/*****************************************************************************/
/*! This function invalidates a cache buffer to be refreshed by
 *   the physical memory.
 *   \param pvMem      Pointer to the cached memory
 *   \param ulMemSize  Length of the cached memory area                       */
/*****************************************************************************/
void OS_InvalidateCacheMemory_FromDevice(void *pvMem, unsigned long ulMemSize)
{
  UNREFERENCED_PARAMETER(pvMem);
}

/*****************************************************************************/
/*****************************************************************************/
/*!<                         Interrupt handling                              */
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*! DSR task evaluating DPM changes.
 *   Function should be instantiated as a high priority task
 *   (all cifX API calls should be lower that its priority).
 *     \param pvParameters   User parameter (unused)                          */
/*****************************************************************************/
void CIFX_DSRHandler(void *pvParameters)
{
  while(s_ptIrqDevInst)
  {
    (void) ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* Block indefinitely */
    if(s_fDSRActive)
    {
      cifXTKitDSRHandler(s_ptIrqDevInst);
    }
    else
      break;
  }

  vTaskDelete(s_xDsrTask);
  s_xDsrTask = NULL;
}

/*****************************************************************************/
/*! IRQ handler for DPM interrupt.
 *   Fixed location in NVIC (idpm_com_host_IRQn).                             */
/*****************************************************************************/
void DIRQ_Handler(uint16_t GPIO_Pin)
{
  if(NETX_DIRQ_Pin == GPIO_Pin)
  {
    /* Prohibit any call if toolkit is not fully configured */
    if(!s_fOSInitDone || !s_ptIrqDevInst || !s_xDsrTask)
    {
      /* Spurious IRQ, disable if detected */
      HAL_NVIC_DisableIRQ(NETX_DIRQ_EXTI_IRQn);
    }
    else
    {
      /* "The DSR is expected to be interruptible and will process the interrupt events in non-interrupt mode."
       -> Signal to DSR thread */
      if(CIFX_TKIT_IRQ_DSR_REQUESTED == cifXTKitISRHandler(s_ptIrqDevInst, 0))
      {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        /* Unblock the handling task so the task can perform any processing necessitated
         by the interrupt. s_xDsrTask is the task's handle, which was obtained
         when the task was created. */
        vTaskNotifyGiveFromISR(s_xDsrTask, &xHigherPriorityTaskWoken);

        /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  }
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent OS Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_EnableInterrupts(void *pvOSDependent)
{
  s_fDSRActive = true;

  (void) xTaskCreate(CIFX_DSRHandler, "DSR_Task",
                      configMINIMAL_STACK_SIZE, /* Depending on the application usage, more Stack may be needed */
                      (void*) 1,
                      configMAX_PRIORITIES - 2,
                      &s_xDsrTask);

  /* Store context locally of ISR/DSR */
  s_ptIrqDevInst = (DEVICEINSTANCE*) pvOSDependent;
  HAL_NVIC_EnableIRQ(NETX_DIRQ_EXTI_IRQn);
}

/*****************************************************************************/
/*! This function disables the interrupts for the device physically
 *   \param pvOSDependent OS Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_DisableInterrupts(void *pvOSDependent)
{
  UNREFERENCED_PARAMETER(pvOSDependent);

  /* Disable DIRQ Pin */
  HAL_NVIC_DisableIRQ(NETX_DIRQ_EXTI_IRQn);
  HAL_NVIC_SetPriority(NETX_DIRQ_EXTI_IRQn, 0, 0);

  /* Trigger deinit of DSR task */
  s_fDSRActive = false;
  (void) xTaskNotifyGive(s_xDsrTask);

  taskYIELD();

  /* Task handle is deleted in DSR epilogue */
  s_ptIrqDevInst = NULL;
}

#ifdef CIFX_TOOLKIT_ENABLE_DSR_LOCK
/*****************************************************************************/
/*! Lock DSR against ISR
*   \param pvOSDependent  Device Extension                                   */
/*****************************************************************************/
void OS_IrqLock(void* pvOSDependent)
{
	/* Only mask DPM interrupt if it was configured */
	if(s_ptIrqDevInst)
	{
		NVIC_DisableIRQ(NETX_DIRQ_EXTI_IRQn);
	}
}

/*****************************************************************************/
/*! Unlock DSR against ISR
*   \param pvOSDependent  Device Extension                                   */
/*****************************************************************************/
void OS_IrqUnlock(void* pvOSDependent)
{
	if(s_ptIrqDevInst)
	{
	    NVIC_EnableIRQ(NETX_DIRQ_EXTI_IRQn);
	}
}
#endif

#ifdef CIFX_TOOLKIT_TIME
/*****************************************************************************/
/*! Get System time
*   \param ptTime   Pointer to store the time value
*   \return actual time value in seconds sincd 01.01.1970                    */
/*****************************************************************************/
uint64_t OS_Time( uint64_t *ptTime)
{
  if (NULL != ptTime)
    *ptTime = 0;

  return 0;
}
#endif

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
