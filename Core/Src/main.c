/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../ECUAL/LCD16X2/LCD16X2.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "string.h"
#include "../../ECUAL/STEPPER/STEPPER.h"
#define MyLCD LCD16X2_1
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

/* USER CODE BEGIN PV */
char receiveData[10];
char receivedData[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_Toggle(void)
{
  int led_state;
  if (led_state == 0)
  {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // ??LED(PB15????)
    led_state = 1;
  }
  else
  {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // ??LED(PB15????)
    led_state = 0;
  }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   
    strcpy(receiveData, receivedData);
    
       LCD16X2_Set_Cursor(MyLCD, 1, 14);
      LCD16X2_Write_String(MyLCD, "c");
        // 处理接收到的数据
        // 例如，将接收到的数据存储到缓冲区
        // 并启动下一次接收中断
        HAL_UART_Receive_IT(&huart1, (uint8_t *)receivedData, sizeof(receivedData));
    
} */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{ 
  /* USER CODE BEGIN 1 */
  uint16_t AD_RES = 0;
  uint16_t last_AD_RES = 0;
  uint8_t Stepper1_Dir = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // 初始化步进电机
  STEPPERS_Init();
  STEPPER_SetSpeed(0, 14); // 设置步进电机速度

  LCD16X2_Init(MyLCD);
  LCD16X2_Clear(MyLCD);
  int value = 0;
  float measuredValue;  // 测量到的值
  measuredValue = 0.0f; // 初始化变量
  float voltage = 0.00;
  int alarm_active = 0;  // �Ƿ񱨾�״̬   0 for paused, 1 for active
  int led_state = 0;     // LED��״̬  0 for OFF, 1 for ON
  int system_active = 1; // ϵͳ��������ͣ��1 for running,0 for stopped
  float alarm_value_high = 2.00;
  float alarm_value_low = 1.00;
  int loop_counter = 0;
  receiveData[0] = 'k';
 receiveData[1] = '1';
        receiveData[2] = '1';
        receiveData[3] = '2';
        receiveData[4] = '.';
        receiveData[5] = '3';
        receiveData[6] = '1';
        receiveData[7] = '.';
        receiveData[8] = '4';
   
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
     HAL_UART_Receive_IT(&huart1, (uint8_t *)receivedData, strlen(receivedData));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { HAL_UART_Receive(&huart1, (uint8_t *)receiveData, sizeof(receiveData),500);
    system_active = (receiveData[1] == '1') ? 1 : 0;
    alarm_active = (receiveData[2] == '1') ? 1 : 0;
    
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)
    {                    // Check if button is pressed (PB11 is low)
      system_active = 1; // Start the system
    }
    else
    {
      system_active = 0; // Stop the system
    }

    value = HAL_ADC_GetValue(&hadc1);
    measuredValue = (value / 4095.0) * 3.3; // 生成0到3.3之间的电压
    voltage = (value / 4095.0) * 3.3;

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) // ????��鱨����PB10
    {
      alarm_active = 1; // ????
    }
    else
    {
      alarm_active = 0; // ????
    }
  




    char alarmHighStr[3] = {receiveData[3], receiveData[4], receiveData[5]};
    char alarmLowStr[3] = {receiveData[6], receiveData[7], receiveData[8]};

    alarm_value_high = atof(alarmHighStr);
    alarm_value_low = atof(alarmLowStr);
    // ??????2V??????,LED????
    if (system_active)
    { // If system is running

      // If voltage > alarm_value_high||voltage < alarm_value_low, blink the LED
      char uhbuffer[10]; // 3个字符 + 小数点 + 结束符
      char ulbuffer[10]; // 3个字符 + 小数点 + 结束符
      uhbuffer[0] = receiveData[3];
      uhbuffer[1] = '.';
      uhbuffer[2] = receiveData[5];
      uhbuffer[3] = '\0';
      ulbuffer[0] = receiveData[6];
      ulbuffer[1] = '.';
      ulbuffer[2] = receiveData[8];
      ulbuffer[3] = '\0';

      LCD16X2_Set_Cursor(MyLCD, 2, 7);
      LCD16X2_Write_String(MyLCD, "UH");

      LCD16X2_Set_Cursor(MyLCD, 2, 12);
      LCD16X2_Write_String(MyLCD, "UL");
      // 在 LCD 上显示 uhbuffer 和 ulbuffer
      LCD16X2_Set_Cursor(MyLCD, 2, 9);
      LCD16X2_Write_String(MyLCD, uhbuffer);

      LCD16X2_Set_Cursor(MyLCD, 2, 14);
      LCD16X2_Write_String(MyLCD, ulbuffer);
      if (system_active)
      {
        LCD16X2_Set_Cursor(MyLCD, 1, 15);
        LCD16X2_Write_String(MyLCD, "s");
      }
      else
      {
        LCD16X2_Set_Cursor(MyLCD, 1, 15);
        LCD16X2_Write_String(MyLCD, " ");
      }

      if (alarm_active)
      {
        LCD16X2_Set_Cursor(MyLCD, 1, 16);
        LCD16X2_Write_String(MyLCD, "a");
      }
      else
      {
        LCD16X2_Set_Cursor(MyLCD, 1, 16);
        LCD16X2_Write_String(MyLCD, " ");
      }
      HAL_Delay(1);

      LCD16X2_Set_Cursor(MyLCD, 1, 1);
      LCD16X2_Write_String(MyLCD, "U=");

      char buffer[10];
      int intPart = (int)measuredValue;                      // 整数部分
      int fracPart = (int)((measuredValue - intPart) * 100); // 小数部分，保留两位小数

      // 将整数部分转换为字符串
      int i = 0;
      if (intPart == 0)
      {
        buffer[i++] = '0';
      }
      else
      {
        while (intPart > 0)
        {
          buffer[i++] = (intPart % 10) + '0';
          intPart /= 10;
        }
        // 反转整数部分字符串
        for (int j = 0; j < i / 2; j++)
        {
          char temp = buffer[j];
          buffer[j] = buffer[i - j - 1];
          buffer[i - j - 1] = temp;
        }
      }

      buffer[i++] = '.'; // 添加小数点

      // 将小数部分转换为字符串
      buffer[i++] = (fracPart / 10) + '0'; // 十位
      buffer[i++] = (fracPart % 10) + '0'; // 个位

      buffer[i++] = 'V'; // 添加单位 "V"
      buffer[i] = '\0';

      LCD16X2_Set_Cursor(MyLCD, 1, 3);
      LCD16X2_Write_String(MyLCD, buffer);

      LCD16X2_Set_Cursor(MyLCD, 2, 1);
      LCD16X2_Write_String(MyLCD, "A=");
      int angle = value * 180 / 4095;
      char angleBuffer[10];     // 根据需要的大小进行声明
      int angleIntPart = angle; // 整数部分
      int angleIndex = 0;
      if (angleIntPart == 0)
      {
        angleBuffer[angleIndex++] = '0';
      }
      else
      {

        while (angleIntPart > 0)
        {
          angleBuffer[angleIndex++] = (angleIntPart % 10) + '0';
          angleIntPart /= 10;
        }
        // 反转整数部分字符串
        for (int j = 0; j < angleIndex / 2; j++)
        {
          char temp = angleBuffer[j];
          angleBuffer[j] = angleBuffer[angleIndex - j - 1];
          angleBuffer[angleIndex - j - 1] = temp;
        }
      }
      while (angleIndex < 3)
      {
        for (int j = angleIndex; j > 0; j--)
        {
          angleBuffer[j] = angleBuffer[j - 1];
        }
        angleBuffer[0] = '0';
        angleIndex++;
      }
      sprintf(angleBuffer, "%03d", angle);
      /*       char combinedBuffer[15]; // 根据需要调整大小

                // 使用 snprintf 将两个字符串和空格组合在一起
                snprintf(combinedBuffer, sizeof(combinedBuffer), "%s %s", buffer, angleBuffer);

                // 发送组合后的字符串
                HAL_UART_Transmit(&huart1, (uint8_t *)combinedBuffer, strlen(combinedBuffer), 100);
            // 添加字符串结束符 */

      LCD16X2_Set_Cursor(MyLCD, 2, 3);
      LCD16X2_Write_String(MyLCD, angleBuffer);
      LCD16X2_Set_Cursor(MyLCD, 2, 6);
      LCD16X2_Write_String(MyLCD, "d");

      AD_RES = value;

      if (AD_RES > last_AD_RES + 30)
      {
        // 电压变大，顺时针转动
        Stepper1_Dir = 0;
        STEPPER_Step_Blocking(0, (AD_RES - last_AD_RES) / 4, Stepper1_Dir);
      }
      else if (AD_RES < last_AD_RES - 30)
      {
        // 电压变小，逆时针转动
        Stepper1_Dir = 1;
        STEPPER_Step_Blocking(0, (last_AD_RES - AD_RES) / 4, Stepper1_Dir);
      }

      last_AD_RES = AD_RES;

      char valuebuffer[10];
      sprintf(valuebuffer, "%04d", value);
      HAL_UART_Transmit(&huart1, (uint8_t *)valuebuffer, strlen(valuebuffer), 100);

      HAL_Delay(100);

      if (alarm_active)
      {

        // 根据 measuredValue 的值显示 "low!" 或 "high!"
        LCD16X2_Set_Cursor(MyLCD, 1, 9);
        LCD16X2_Write_String(MyLCD, "     ");
        if (measuredValue > alarm_value_high)
        {
          LCD16X2_Set_Cursor(MyLCD, 1, 9);
          LCD16X2_Write_String(MyLCD, "high!");
          HAL_Delay(50);
        }
        else if (measuredValue < alarm_value_low)
        {
          LCD16X2_Set_Cursor(MyLCD, 1, 9);
          LCD16X2_Write_String(MyLCD, "low!!");
          HAL_Delay(50);
        }
        else
        {
          LCD16X2_Set_Cursor(MyLCD, 1, 9);
          LCD16X2_Write_String(MyLCD, "     "); // 清空显示
        }

        if (voltage > alarm_value_high || voltage < alarm_value_low)
        {
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);  // Toggle LED state
          HAL_Delay(5); // LED on time: 500ms
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_Delay(5);
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);  // Toggle LED state
          HAL_Delay(5); // LED on time: 500ms
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_Delay(5);HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);  // Toggle LED state
          HAL_Delay(5); // LED on time: 500ms
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_Delay(5);
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);  // Toggle LED state
          HAL_Delay(5); // LED on time: 500ms
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_Delay(5);
        }
        else
        {
          // Ensure LED is off when voltage <= 2.0V or system is not active
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // Turn off LED (PB15 is high)
        }
      }
      else
      {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // Turn off LED (PB15 is high)
      }
    }
    else
    {
      LCD16X2_Set_Cursor(MyLCD, 1, 1);
      LCD16X2_Write_String(MyLCD, "System Stopped");
      // Ensure LED is off when system is not active or alarm is not active
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // Turn off LED (PB15 is high)
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    LCD16X2_Set_Cursor(MyLCD, 1, 8);
    LCD16X2_Write_String(MyLCD, " ");
    LCD16X2_Set_Cursor(MyLCD, 1, 14);
    LCD16X2_Write_String(MyLCD, " ");

    if (loop_counter >= 10)
    {
      LCD16X2_Clear(MyLCD);
      loop_counter = 0; // 重置计数器
    }
    else
    {
      loop_counter++; // 增加计数器
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  STEPPER_TMR_OVF_ISR(htim);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
