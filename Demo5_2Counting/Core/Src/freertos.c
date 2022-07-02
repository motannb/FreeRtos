/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Checkln */
osThreadId_t Task_ChecklnHandle;
const osThreadAttr_t Task_Checkln_attributes = {
  .name = "Task_Checkln",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Sem_Tables */
osSemaphoreId_t Sem_TablesHandle;
const osSemaphoreAttr_t Sem_Tables_attributes = {
  .name = "Sem_Tables"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Checkln(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of Sem_Tables */
  Sem_TablesHandle = osSemaphoreNew(5, 5, &Sem_Tables_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_Checkln */
  Task_ChecklnHandle = osThreadNew(AppTask_Checkln, NULL, &Task_Checkln_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Checkln */
/**
  * @brief  Function implementing the Task_Checkln thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Checkln */
void AppTask_Checkln(void *argument)
{
  /* USER CODE BEGIN AppTask_Checkln */
  UBaseType_t totalTables= uxSemaphoreGetCount(Sem_TablesHandle);
  LCD_ShowString(20,50,160,24,16,(uint8_t *)"Total tables= ");
  LCD_ShowNum(160,50,totalTables,2,16);
  LCD_ShowString(20,80,160,24,16,(uint8_t *)"Available tables= ");

  /* Infinite loop */
  for(;;)
  {
    GPIO_PinState keyState= HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5);
    if(keyState==GPIO_PIN_RESET)
    {
      BaseType_t result= xSemaphoreTake(Sem_TablesHandle, pdMS_TO_TICKS(100));
      if(result==pdTRUE)
      {
        LCD_ShowString(20,100,160,24,16,(uint8_t *)"Check in OK ");
      }
      else
        LCD_ShowString(20,120,160,24,16,(uint8_t *)"Check in fail ");
      vTaskDelay(pdMS_TO_TICKS(300));
    }
    UBaseType_t availableTables= uxSemaphoreGetCount(Sem_TablesHandle);
    LCD_ShowNum(160,80,availableTables,2,16);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  /* USER CODE END AppTask_Checkln */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

