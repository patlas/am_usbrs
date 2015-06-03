#include "usart.h"
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <stdbool.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;



// UART transmit buffer descriptor
static RingBuffer USART_RingBuffer_Tx;
// UART transmit buffer memory pool
static char RingBufferData_Tx[100];

// UART receive buffer descriptor
static RingBuffer USART_RingBuffer_Rx;
// UART receive buffer memory pool
static char RingBufferData_Rx[100];

static void (*func)(void);

bool USART_PutChar(char c){
		
		if( RingBuffer_PutChar(&USART_RingBuffer_Tx, c ) == true ) {
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
			return true;
		}
		else
			return false ;
}


size_t USART_WriteData(const void *data, size_t dataSize){
	char* ptr = (char*)data;
	size_t i = 0;
	uint32_t wait ;
	
	while ( i < dataSize ) {
		if (USART_PutChar( ptr[i] ) == true) {
			i++;
		}
		else {
			// Not break, because didn't add all data to usart tx buffer
			// just wait some time for freeing space in usart tx buffer
			for ( wait=0; wait < 10000; ++wait) {;}
		}
	}
	
	return i;
}



bool USART_GetChar(char *c){
		if( RingBuffer_GetChar(&USART_RingBuffer_Rx, c ) == true )
			return true ;
		else 
			return false ;
}


size_t USART_ReadData(char *data, size_t maxSize){
	
	size_t i;
	
	for ( i = 0; i < maxSize ; i++ ) {
		if( RingBuffer_GetChar(&USART_RingBuffer_Rx, data+i ) == false )
			return i ;
		}
	return i;
}





/* USART2 init function */
void MX_USART2_UART_Init(void)
{
	// initialize ring buffers
	RingBuffer_Init(&USART_RingBuffer_Tx, RingBufferData_Tx, sizeof(RingBufferData_Tx));
	RingBuffer_Init(&USART_RingBuffer_Rx, RingBufferData_Rx, sizeof(RingBufferData_Rx));
	
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

}


	/* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */ 
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).					*/

void USART2_Reconfig(uint8_t *settings)
{
	uint32_t stopBits, parity;
	uint32_t baudRate = settings[3];
	baudRate<<=8;
	baudRate |= settings[2];
	baudRate<<=8;
	baudRate |= settings[1];
	baudRate<<=8;
	baudRate |= settings[0];
	
	if(settings[4] == 0)
		stopBits = UART_STOPBITS_1;
	else
		stopBits = UART_STOPBITS_2;
	
	switch(settings[5])
	{
		case 0:
			parity = UART_PARITY_NONE;
			break;
		
		case 1:
			parity = UART_PARITY_ODD;
			break;
		
		case 2:
			parity = UART_PARITY_EVEN;
			break;
		
		default:
			parity = UART_PARITY_NONE;
			break;
	}
	
	huart2.Instance = USART2;
  huart2.Init.BaudRate = baudRate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = stopBits;
  huart2.Init.Parity = parity;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

}




// USART Interrupt Service Routine (ISR)
void USART2_IRQHandler(void){
	char tmp;
	
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)) {
		// the RXNE interrupt has occurred
		if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE)) {
			// the RXNE interrupt is enabled
			
			// TODO: read the received character and place it in the receive ring buffert
			tmp = huart2.Instance->DR;
			RingBuffer_PutChar(&USART_RingBuffer_Rx, tmp);
			
			//if ( (tmp == '\n') && ( &func != NULL ) )
			//	func() ;
			
			
		}
	}


	
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE)) {
	  // the TXE interrupt has occurred
    if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE)) {
	    // the TXE interrupt is enabled
		  
			
			// TODO: get a character from the transmit ring buffer and send it via UART
		//if ( USART_GetChar( (char*)UartHandle.Instance->DR ) == false) 
		//	__USART_DISABLE_IT(&UartHandle, USART_IT_TXE);
			if ( true == RingBuffer_GetChar(&USART_RingBuffer_Tx, &tmp) ) {
				huart2.Instance->DR = (uint8_t)tmp ;
			}
			else {
				__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
				
			}
			
	  }
  }
}