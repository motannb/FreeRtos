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
#include "adc.h"
#include "stdio.h"
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
/* Definitions for Task_ADC */
osThreadId_t Task_ADCHandle;
const osThreadAttr_t Task_ADC_attributes = {
  .name = "Task_ADC",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Info */
osThreadId_t Task_InfoHandle;
const osThreadAttr_t Task_Info_attributes = {
  .name = "Task_Info",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void App_ADC(void *argument);
void App_Info(void *argument);

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
  /* creation of Task_ADC */
  Task_ADCHandle = osThreadNew(App_ADC, NULL, &Task_ADC_attributes);

  /* creation of Task_Info */
  Task_InfoHandle = osThreadNew(App_Info, NULL, &Task_Info_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_App_ADC */
/**
  * @brief  Function implementing the Task_ADC thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_App_ADC */
void App_ADC(void *argument)
{
  /* USER CODE BEGIN App_ADC */
  char ADC_Value[10];
  LCD_ShowString(10,40,200,24,16,(uint8_t *)"Task:ADC:ADC by polling");
  LCD_ShowString(10,60,200,24,16,(uint8_t *)"ADC Value(mV)=");

  TickType_t previousWakeTime=xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
      HAL_ADC_Start(&hadc1);
      if(HAL_ADC_PollForConversion(&hadc1,100)==HAL_OK)
      {
          uint32_t val= HAL_ADC_GetValue(&hadc1);
          //LCD_ShowxNum(125,60,val,4,16,0);
          uint32_t Volt=3300*val;
          Volt=Volt>>12;
          sprintf(ADC_Value,"%d",Volt);
          LCD_ShowString(125,60,200,24,16,(uint8_t *)ADC_Value);

      }
      vTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(500));
  }
  /* USER CODE END App_ADC */
}

/* USER CODE BEGIN Header_App_Info */
/**
* @brief Function implementing the Task_Info thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_App_Info */
void App_Info(void *argument)
{
  /* USER CODE BEGIN App_Info */
  char str[8];
  TaskHandle_t taskHandle;
  UBaseType_t  hwm;
  POINT_COLOR=RED;


  taskHandle=Task_ADCHandle;
  hwm=uxTaskGetStackHighWaterMark(taskHandle);
  sprintf(str,"%d",hwm);
  LCD_ShowString(10,80,200,24,16,(uint8_t *)"High Water Mark of tasks:");
  LCD_ShowString(10,100,200,24,16,(uint8_t *)"Task_ADC= ");
  LCD_ShowString(100,100,200,24,16,(uint8_t *)str);

  taskHandle=Task_InfoHandle;
  hwm=uxTaskGetStackHighWaterMark(taskHandle);
  sprintf(str,"%d",hwm);
  LCD_ShowString(10,120,200,24,16,(uint8_t *)"Task_Info= ");
  LCD_ShowString(100,120,200,24,16,(uint8_t *)str);

  UBaseType_t loopCount=0;



  /* Infinite loop */
  for(;;)
  {
      loopCount++;
      HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
      vTaskDelay(pdMS_TO_TICKS(500));
      if (loopCount==10)
          break;
  }

  POINT_COLOR=BLACK;
    LCD_ShowString(10,140,200,24,16,(uint8_t *)"Task_Info is deleted");
    vTaskDelete(NULL);
  /* USER CODE END App_Info */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

