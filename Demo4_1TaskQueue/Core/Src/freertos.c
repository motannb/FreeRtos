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
#include "queue.h"
#include "lcd.h"
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
uint16_t	curScreenX=100;		//LCD当前X
uint16_t	curScreenY=260;		//LCD当前Y
uint16_t	lastScreenX=100;	// LCD前一步的X
uint16_t	lastScreenY=260;	// LCD前一步的Y


typedef enum{
    KEY_NONE=0,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_DOWN
}KEYS;
/* USER CODE END Variables */
/* Definitions for Task_ScanKeys */
osThreadId_t Task_ScanKeysHandle;
const osThreadAttr_t Task_ScanKeys_attributes = {
  .name = "Task_ScanKeys",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Draw */
osThreadId_t Task_DrawHandle;
const osThreadAttr_t Task_Draw_attributes = {
  .name = "Task_Draw",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Queue_Keys */
osMessageQueueId_t Queue_KeysHandle;
const osMessageQueueAttr_t Queue_Keys_attributes = {
  .name = "Queue_Keys"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

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

  /* Create the queue(s) */
  /* creation of Queue_Keys */
  Queue_KeysHandle = osMessageQueueNew (10, sizeof(uint8_t), &Queue_Keys_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_ScanKeys */
  Task_ScanKeysHandle = osThreadNew(StartDefaultTask, NULL, &Task_ScanKeys_attributes);

  /* creation of Task_Draw */
  Task_DrawHandle = osThreadNew(StartTask02, NULL, &Task_Draw_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the Task_ScanKeys thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  GPIO_PinState keyState=GPIO_PIN_SET;
  KEYS key=KEY_NONE;
  /* Infinite loop */
  for(;;)
  {
      key=KEY_NONE;
      // 1.检测KeyRight
      keyState=HAL_GPIO_ReadPin(KeyRight_GPIO_Port, KeyRight_Pin); //KeyRight
      if (keyState==GPIO_PIN_RESET)  //KeyRight 是低输入有效
          key=KEY_RIGHT;

      // 2.检测KeyDown
      keyState=HAL_GPIO_ReadPin(KeyDown_GPIO_Port, KeyDown_Pin); //KeyDown
      if (keyState==GPIO_PIN_SET)  //KeyDown 是高输入有效
          key=KEY_DOWN;

      // 3.检测KeyLeft
      keyState=HAL_GPIO_ReadPin(KeyLeft_GPIO_Port, KeyLeft_Pin); //KeyLeft
      if (keyState==GPIO_PIN_RESET)  //KeyLeft 是低输入有效
          key=KEY_LEFT;

/*      //4. 检测KeyUp
      keyState=HAL_GPIO_ReadPin(KeyUp_GPIO_Port, KeyUp_Pin); //KeyUp
      if (keyState==GPIO_PIN_SET)  //KeyUp 是高输入有效
          key=KEY_UP;*/

      // 5. 是否有键按下
      if (key != KEY_NONE)   //有键按下
          {
          BaseType_t err= xQueueSendToBack(Queue_KeysHandle, &key, pdMS_TO_TICKS(50));
          if (err == errQUEUE_FULL)  //如果队列满了，就复位队列
              xQueueReset(Queue_KeysHandle);
          vTaskDelay(pdMS_TO_TICKS(300)); //至少300才能去除抖动影响，同时让任务调度执行
          }
      else
          vTaskDelay(pdMS_TO_TICKS(5)); 	//死循环内延时，进入阻塞状态
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task_Draw thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  //读取队列信息
  char* qName=pcQueueGetName(Queue_KeysHandle);   //读取队列名称
  LCD_ShowString(10, 40, 200,24,16,(uint8_t *)"Queue Name =");
  LCD_ShowString(120, 40, 200,24,16,qName);

  UBaseType_t qSpaces=uxQueueSpacesAvailable(Queue_KeysHandle);
  LCD_ShowString(10, 70, 200,24,16,(uint8_t *)"Queue Size =");  //初始剩余空间，也就是队列的大小
  LCD_ShowNum(100, 70, qSpaces,2,16);

  LCD_ShowString(10, 110,200,24,16, (uint8_t *)"uxQueueMessagesWaiting()= ");
  LCD_ShowString(10, 150,200,24,16, (uint8_t *)"uxQueueSpacesAvailable()= ");

//uint16_t LcdX=10;		//记录显示位置

  UBaseType_t msgCount=0, freeSpace=0;
  KEYS  keyCode;
  /* Infinite loop */
  for(;;)
  {
      msgCount=uxQueueMessagesWaiting(Queue_KeysHandle);  //等待读取的消息个数
      LCD_ShowNum(200, 110, msgCount, 2,16);
      freeSpace=uxQueueSpacesAvailable(Queue_KeysHandle); //剩余空间个数
      LCD_ShowNum(200, 150, freeSpace, 2,16);
      //读取消息, 阻塞式等待
      BaseType_t  result=xQueueReceive(Queue_KeysHandle, &keyCode, pdMS_TO_TICKS(50));
      if (result != pdTRUE)
          continue;

      //读取到消息，根据按键代码移动画线
      if (keyCode==KEY_LEFT)
          curScreenX -= 10;
      else if (keyCode==KEY_RIGHT)
          curScreenX += 10;
      else if (keyCode==KEY_DOWN)
          curScreenY += 10;

      if  (curScreenX>lcddev.width)
          curScreenX=lcddev.width;
      if (curScreenY>lcddev.height)
          curScreenY=lcddev.height;

      LCD_DrawLine(lastScreenX, lastScreenY, curScreenX, curScreenY);
      lastScreenX =curScreenX;
      lastScreenY =curScreenY;

      vTaskDelay(pdMS_TO_TICKS(10)); //如果延时较长，可能导致队列里有多个项
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

