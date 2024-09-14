/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
//USER define of arm length
#define L1 104
#define L2 130
#define L3 36
#define RX_BUFFER_SIZE 20
#define pi 3.1415926
#define a  85
#define b  141


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */

#endif

ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);

/*--------------------------------------------------------
                        自定义声明函数
--------------------------------------------------*--*/
int* Calculate_theta(int* coordinate);
int* get_xy(char message[RX_BUFFER_SIZE], int xy[2]);
char* get_message();
char* theta_to_char(float theta, int theta_type);
void send_message(char* message);

void WIFI_INT(void);

int main(void)
{
  // 函数初始化
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
 
	WIFI_INT();
	
	int s_num=0;
	int r_num=0;
	int b_num=0;
	int g_num=0;
	int c_num=0;
	int t_num=0;
	
	
	//WIFI上行数据拼接
	char mess1[]="AT+MQTTPUB=0,\"/sys/k1gbsFZaJJy/STM32H753ZI/thing/event/property/post\",\"{\\\"params\\\":{\\\"";
	char mess2[]="\\\":";
	char mess3[]="}}\",1,0";
	
	char stats;
	char mess[100];
	char num;
	 
	// 阿里云上传数据初始化
	
	stats='S';
	num=(char)(s_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='S';
	num=(char)(s_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='R';
	num=(char)(r_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='B';
	num=(char)(b_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='G';
	num=(char)(g_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='C';
	num=(char)(c_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	stats='T';
	num=(char)(t_num+48);
	snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
	HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
	HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
	HAL_Delay(1500);
	
	
  while (1)
  {
		char rxData[RX_BUFFER_SIZE]; // ?????
		strncpy(rxData, get_message(), RX_BUFFER_SIZE);
		
		
		rxData[RX_BUFFER_SIZE - 1] = '\0'; // ????????
		//HAL_UART_Transmit(&huart3, (uint8_t*)rxData, 15, 0xFFFF); // ?????????
		
		int zuobiao[2]= {0,0};
		int* test_zuobiao;
		test_zuobiao = get_xy(rxData, zuobiao);
		
		//HAL_UART_Transmit(&huart3, (uint8_t*)zuobiao[0], 5, 0xFFFF);
		
		Calculate_theta(zuobiao);
		
		HAL_Delay(2000);
		//气泵吸气
		HAL_UART_Transmit(&huart3, "#005P2500T2000!", 15, 0xFFFF);
		HAL_Delay(2000);
		//到达指定位置
		if(rxData[0] == 'b')
		{
			HAL_UART_Transmit(&huart3, "#001P1500T1000!#000P2200T3000!", 30, 0xFFFF);
			HAL_Delay(3000);
			HAL_UART_Transmit(&huart3, "#002P0900T2000!#003P1000T2000!#001P1000T2000!", 45, 0xFFFF);
			
			//阿里云
			s_num = s_num +1;
			b_num = b_num +1;
			
			// 更新分拣总数
			stats='S';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			
			// 更新分拣蓝色总数
			stats='B';
			num=(char)(b_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			
			if(b_num % 2==1)
			{
				c_num = c_num +1;
		// 更新圆形分拣数
			stats='C';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}else
			{
				t_num = t_num +1;
		// 更新矩形分拣数
			stats='T';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}
			//HAL_UART_Transmit(&huart3, "#000", 15, 0xFFFF);
		}
		else if(rxData[0] == 'g')
		{
			HAL_UART_Transmit(&huart3, "#001P1500T1000!#000P1800T3000!", 30, 0xFFFF);
			HAL_Delay(3000);
			HAL_UART_Transmit(&huart3, "#002P0800T2000!#003P0800T2000!#001P1000T2000!", 45, 0xFFFF);
			//阿里云
			s_num = s_num +1;
			g_num = g_num +1;
			
			// 更新分拣总数
			stats='S';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			
			// 更新分拣蓝色总数
			stats='G';
			num=(char)(g_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			if(g_num % 2==1)
			{
				c_num = c_num +1;
		// 更新圆形分拣数
			stats='C';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}else
			{
				t_num = t_num +1;
		// 更新矩形分拣数
			stats='T';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}
			//HAL_UART_Transmit(&huart3, "#000", 15, 0xFFFF);
		}
		else if(rxData[0] == 'r')
		{
			HAL_UART_Transmit(&huart3, "#001P1500T1000!#000P2050T3000!", 30, 0xFFFF);
			HAL_Delay(1000);
			HAL_UART_Transmit(&huart3, "#002P0900T2000!#003P1000T2000!#001P1000T2000!", 45, 0xFFFF);
			
			//阿里云
			s_num = s_num +1;
			r_num = r_num +1;
			
			// 更新分拣总数
			stats='S';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			
			// 更新分拣蓝色总数
			stats='R';
			num=(char)(r_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			if(r_num % 2==1)
			{
				c_num = c_num +1;
		// 更新圆形分拣数
			stats='C';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}else
			{
				t_num = t_num +1;
		// 更新矩形分拣数
			stats='T';
			num=(char)(s_num+48);
			snprintf(mess, sizeof(mess), "%s%c%s%c%s", mess1, stats, mess2, num, mess3);
			HAL_UART_Transmit(&huart1, (uint8_t*)mess, strlen(mess), 0xFFFF);
			HAL_UART_Transmit(&huart1,"\r\n", strlen("\r\n"), 0xFFFF);
			HAL_Delay(2000);
			}
			//HAL_UART_Transmit(&huart3, "#000", 15, 0xFFFF);
		}
		//气泵呼气
		//HAL_Delay(2000);
		HAL_UART_Transmit(&huart3, "#005P1500T2000!", 15, 0xFFFF);
		HAL_Delay(2000);
		HAL_UART_Transmit(&huart3, "#001P1500T2000!#000P1500T2000!", 30, 0xFFFF);
		HAL_Delay(3000);
		
		HAL_UART_Transmit(&huart3, "#001P1500T2000!#000P1500T2000!#002P0500T2000!#003P0500T2000!", 60, 0xFFFF);
		HAL_Delay(2000);

  }
 
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
}


static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10707DBC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_OTG_FS_PWR_EN_GPIO_Port, USB_OTG_FS_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_PWR_EN_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_OTG_FS_PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_OVCR_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_OVCR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
 
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

/*--------------------------------------------------------
                        自定义函数
----------------------------------------------------*/

int* Calculate_theta(int* coordinate){
	//第一个旋转角的定义
	float theta0 = 0;
	
	//坐标定义
	float x = 1.3324*coordinate[1] -2.9611 + a;
	float y = -(1.2304*coordinate[0] -12.869) + b;
	float x1;
	float y1;
	//角度定义
	float theta1;

	float theta2;

	float theta3;

	//计算第一个theta
	//y/x = tan theta0
	if(y == 0)
	{
		theta0 = 0;
	}
	else if(x == 0)
	{
		theta0 = 1.046;
	}
	else
	{
		theta0 = atan(y/x)-0.08;
	}

	//第一次旋转之后,机器转的角度只需要和上一次的角度补全即可
	//存储转完之后的位置用于下次计算补足的角度
	
	//计算角度的中间变量定义
	float c2;
	float c_fine;
	float fine;
	float beta;
	//将平面坐标系转换为另一个坐标系
	x1 = pow(pow(x,2) + pow(y,2),0.5);
	y1 = L3;
	
	

	//计算角度2过程
	c2 = (pow(x1,2) + pow(y1,2) - pow(L1,2) - pow(L2,2))/(2*L1*L2);
	theta2 = acos(c2);
	

	
	c_fine = ((pow(x1,2) + pow(y1,2) + pow(L1,2)) - pow(L2,2))/(2*L1*pow(pow(x1,2) + pow(y1,2),0.5));
	fine = acos(c_fine);
	beta = atan(y1/x1);
	
	theta1 = beta + fine;
	

	
	theta3 = pi/2 - theta2 + fine + beta;
	send_message(theta_to_char(theta3, 3));
	send_message(theta_to_char(theta0, 0));
	send_message(theta_to_char(theta2, 2));
	send_message(theta_to_char(theta1, 1));
	
	return 0;
}

int* get_xy(char message[RX_BUFFER_SIZE], int xy[2]){
	//记下冒号的位置
	int remember_colon;
	//记下逗号的位置
	int remember_comma;
	//记下结尾的位置
	int remember_end;
	//保存字符串长度
	int len;
	len = strlen(message);
	
	//遍历字符串获取标志位
	for(int i = 0; i < len; i++)
	{
		if(message[i] == ':')
		{
			remember_colon = i;
		}
		else if(message[i] == ',')
		{
			remember_comma = i;
		}
//		else if(message[i] == '\0')
//		{
//			remember_end = i;
//		}
	}
	

	
	//获取x坐标字符串的长度
	int x_len;
	x_len = remember_comma - remember_colon - 1;
	
	//存储x字符串用于转换
	char str_x[x_len];
	for(int i = 0; i < x_len; i++)
	{
		str_x[i] = message[remember_colon + i + 1];
	}
	
	//获取y坐标字符串的长度
	int y_len;
	y_len = len - remember_comma;
	
	//存储y字符串用于转换
	char str_y[y_len];
	for(int i = 0; i < y_len; i++)
	{
		str_y[i] = message[remember_comma + i + 1];
	}
	
	//转为int型
	xy[0] = atoi(str_x);
	xy[1] = atoi(str_y);
	return xy;
}

char *get_message(){
	
	uint8_t rxIndex = 0;
	static char rtData[RX_BUFFER_SIZE];
	
	while (rxIndex < RX_BUFFER_SIZE - 1) {
    HAL_UART_Receive(&huart3, (uint8_t*)(rtData + rxIndex), 1, 0xFFFF);
    if (rtData[rxIndex] == '\n') { 
        break;
    }
    rxIndex++; 
}
	
	return rtData;
}

char* theta_to_char(float theta, int theta_type){
	float servo;
	static char sover[RX_BUFFER_SIZE];
	if(theta_type == 2){
		servo = ((theta*180/pi)*2000)/180 + 500;
	}
	else if(theta_type == 1)
	{	
		servo = ((theta*180/pi)*2000)/180 + 500;
	}
	else if(theta_type == 0)
	{
		servo = ((theta*180/pi)*2000)/270 + 1500;
	}
	else if(theta_type == 3)
	{
		servo = ((theta*180/pi)*2000)/180 + 500;
	}
	
	int i_servo;
	i_servo = (int)servo;
	char c_servo[3];
	if(i_servo < 1000)
	{
		sprintf(c_servo, "%d%d", 0, i_servo);
	}
	else
	{
		sprintf(c_servo, "%d", i_servo);
	}
	
	//send_message(c_servo);
	char s[]  = {"T2000!"};
	char s4[] = {"T1000!"};
	char s0[] = {"#000P"};
	char s1[] = {"#001P"};
	char s2[] = {"#002P"};
	char s3[] = {"#003P"};
	if(theta_type == 0)
	{
		sprintf(sover, "%.5s%.4s%.6s", s0, c_servo, s);
	}
	else if(theta_type == 1)
	{
		sprintf(sover, "%.5s%.4s%.6s", s1, c_servo, s);
	}
	else if(theta_type == 2)
	{
		sprintf(sover, "%.5s%.4s%.6s", s2, c_servo, s);
	}
	else if(theta_type == 3)
	{
		sprintf(sover, "%.5s%.4s%.6s", s3, c_servo, s4);
	}
	//send_message(sover);
	return sover;
}

void send_message(char* order){
	
	order[16] = '\0'; 
  //order[RX_BUFFER_SIZE - 1] = '\0'; // ????????
	HAL_UART_Transmit(&huart3, (uint8_t*)order, 18, 0xFFFF); // ?????????
	

}

void WIFI_INT(void)
{
  HAL_UART_Transmit(&huart1,"AT+RST\r\n", strlen("AT+RST\r\n"), 0xFFFF);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1,"AT+RESTORE\r\n", strlen("AT+RESTORE\r\n"), 0xFFFF);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1,"AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"), 0xFFFF);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1,"AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n", strlen("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n"), 0xFFFF);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1,"AT+CWJAP=\"Redmi\",\"12345678\"\r\n", strlen("AT+CWJAP=\"Redmi\",\"12345678\"\r\n"), 0xFFFF);
	HAL_Delay(8000);
	HAL_UART_Transmit(&huart1,"AT+MQTTUSERCFG=0,1,\"NULL\",\"STM32H753ZI&k1gbsFZaJJy\",\"3F1DF9D1D1928D46F1497F3B223518006725606A\",0,0,\"\"\r\n", strlen("AT+MQTTUSERCFG=0,1,\"NULL\",\"STM32H753ZI&k1gbsFZaJJy\",\"3F1DF9D1D1928D46F1497F3B223518006725606A\",0,0,\"\"\r\n"), 0xFFFF);
	HAL_Delay(3000);
	HAL_UART_Transmit(&huart1,"AT+MQTTCLIENTID=0,\"STM32H753ZI|securemode=2\\,signmethod=hmacsha1\\,timestamp=1720340995971|\"\r\n", strlen("AT+MQTTCLIENTID=0,\"STM32H753ZI|securemode=2\\,signmethod=hmacsha1\\,timestamp=1720340995971|\"\r\n"), 0xFFFF);
	HAL_Delay(3000);
	HAL_UART_Transmit(&huart1,"AT+MQTTCONN=0,\"k1gbsFZaJJy.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n", strlen("AT+MQTTCONN=0,\"k1gbsFZaJJy.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n"), 0xFFFF);
	HAL_Delay(10000);
	HAL_UART_Transmit(&huart1,"AT+MQTTCONN=0,\"k1gbsFZaJJy.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n", strlen("AT+MQTTCONN=0,\"k1gbsFZaJJy.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n"), 0xFFFF);
	HAL_Delay(10000);
	HAL_UART_Transmit(&huart1,"AT+MQTTSUB=0,\"topic\",1\r\n", strlen("AT+MQTTSUB=0,\"topic\",1\r\n"), 0xFFFF);
	HAL_Delay(5000);
}
	
