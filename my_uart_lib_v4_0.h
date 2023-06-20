

// My Uart lib
// 송 명 규
// 010-2402-4398
// End Ver = V3.5
// Ver1.0 = 2018, 07, 09
// Ver2.0 = 2019, 05, 19
// Ver3.0 = 2020, 11, 06
// vER3.2 = 2022, 11, 28
// vER3.4 = 2022, 12, 23
// VER3.5 = 2023, 06, 16
// VER4.0 = 2023, 06, 20

#ifndef __my_uart_lib__
#define __my_uart_lib__


#ifdef my_usart_LIB_en

#define Uart_Ch_1	1
#define Uart_Ch_2	2
#define Uart_Ch_3	3
#define Uart_Ch_4	4  // L152
#define Uart_Ch_5	5  // L152

//#define CR	0x0d  //13
//#define LF	0x0a  //10

#include <stdio.h>  // printf, sprintf 사용하기 위해서
#include <stdarg.h>
#include <string.h>

void vprint(const char *fmt, va_list argp);
void my_printf(const char *fmt, ...); // custom printf() function
void tx_send(uint8_t tx_data, uint8_t ch);
void tx_str(uint8_t  *tx_d, char ch);        // v3.5
void tx(uint8_t *tx_d, char ch, char lans); // v3.5


HAL_StatusTypeDef RcvStat;
uint8_t usart_ch = 2;  // Nrcleo default usart ch = 2
uint8_t Rx_data_1[20];  // uart 1 rx_buf
uint8_t Rx_data_2[20];  // uart 2 rx_buf
uint8_t Rx_data_3[20];  // uart 3 rx_buf

uint8_t rx_cnt_1 = 0;    // uart 1 수신 Data Counter
uint8_t rx_cnt_2 = 0;    // uart 2 수신 Data Counter
uint8_t rx_cnt_3 = 0;    // uart 3 수신 Data Counter

uint8_t rx_end_flag_1 = 0; // uart1 rx end flag
uint8_t rx_end_flag_2 = 0; // uart2 rx end flag
uint8_t rx_end_flag_3 = 0; // uart3 rx end flag

// 2022, 11, 06 추가 = 교육생들이 좀 더 쉽게하기 위해서 추가 함
uint8_t *rx_buf_1[10];
uint8_t *rx_buf_2;
uint8_t *rx_buf_3[10];

// 2022, 11, 28수정 = Ver 3.2
int8_t my_printf_ch = Uart_Ch_1;  // ch 2 uart = Nucleo Board

// 2022, 12, 23 수정 = Ver 3.4
uint8_t old_buf_1;
uint8_t old_buf_2;
uint8_t old_buf_3;

// 2022, 9 추가 = 수신 인터럽트가 2번식 걸리는 현상이 있음 = STM32L152
uint8_t uart_irq_cnt = 0;
#endif


// 2022, 11, 06 추가 = 교육생들이 좀 더 쉽게하기 위해서 추가 함 = 송신 함수
extern int __io_putchar(int ch)
{
    //UART_HandleTypeDef huart1;
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    __NOP();
  return ch;
}

// printf 사용하기 위해서 = 2023, 06, 23 위치이동
int fputc(int ch, FILE *f)
 {
   uint8_t temp[1]={ch};
   HAL_UART_Transmit(&huart2, temp, 1, 2);
   return(ch);
 }

// 2022, 11, 06 추가 = 교육생들이 좀 더 쉽게하기 위해서 추가 함 = 송신
extern int _write(int file, char *ptr, int len)
{
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
      // 콘솔에 출력할 수 있도록 하기 위해서 수정 함
      __io_putchar(*ptr++);// == org
      asm("nop");
    }
    return len;
}

// 2022, 11, 06 추가 = 교육생들이 좀 더 쉽게하기 위해서 추가 함 = 송신
extern int __io_getchar(void)
{
    char data[4];
    uint8_t ch, len = 1;

    while(HAL_UART_Receive(&huart2, &ch, 1, 10) != HAL_OK){ }

    memset(data, 0x00, 4);
    switch(ch)
    {
        case '\r':
        case '\n':
            len = 2;
            sprintf(data, "\r\n");
            break;

        case '\b':
        case 0x7F:
            len = 3;
            sprintf(data, "\b \b");
            break;

        default:
            data[0] = ch;
            break;
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 10);
    return ch;
}


// my usart LIB func == 2023, 06, 20 추가
void tx_send(uint8_t tx_data, uint8_t ch)
{
 switch(ch)
  {
   case 1: // uart 1 = CubeMX���� ����ϰڴٰ� ���� ���ϸ� ���� ��
           #ifdef USART_Ch_1
	          HAL_UART_Transmit(&huart1, (uint8_t *)&tx_data, 1,10);
            #endif
	     break;

   case 2: // uart 2
           #ifdef USART_Ch_2
             HAL_UART_Transmit(&huart2, (uint8_t *)&tx_data, 1,10);
           #endif
         break;

   case 3: // uart 3
           #ifdef USART_Ch_3
            HAL_UART_Transmit(&huart3, (uint8_t *)&tx_data, 1,10);
           #endif
         break;
  }
}

// v3.5 == 2023, 06, 18 수정
void tx_str(uint8_t  *tx_d, char ch)
{
  // uart �� ���ڿ� ���۽�
  while(*tx_d != '\0')
  {
    tx_send(*tx_d, ch);
    tx_d++;
  }
}

// v3.5 == 2023, 06, 18 추가
// low data tx
void tx(uint8_t *tx_d, char ch, char lans)
{
	 do{
	      tx_send(*tx_d, ch);
	      tx_d++;
	      lans--;
	     }while(lans != 0);
}

//2023, 06, 20 추가
void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if(0 < vsprintf(string,fmt,argp)) // build string
    {
      // 2022, 11, 28수정 = Ver 3.2
     #ifdef USART_Ch_1
     	if(my_printf_ch == Uart_Ch_1) HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
     #endif

     #ifdef USART_Ch_2
     	if(my_printf_ch == Uart_Ch_2) HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
     #endif

     #ifdef USART_Ch_3
     	if(my_printf_ch == Uart_Ch_3) HAL_UART_Transmit(&huart3, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
     #endif
    }
}

void my_printf(const char *fmt, ...) // custom printf() function
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}

//=======  interrupt call back func  ======================
// 2023, 06, 20 추가
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	 {
	   //---------------------------------------------------------
         cnt = rx_buf_1;
         cnt++; // 6

         printf("ch1 Test\t");
         my_printf_ch = Uart_Ch_2;
         my_printf("uart1 rx\r\n\n");
	   //---------------------------------------------------------
       #ifdef USART_Ch_1
	    __HAL_UART_CLEAR_PEFLAG(&huart1);
	    HAL_UART_Receive_DMA(&huart1, (uint8_t *)rx_buf_1, 1);
       #endif
	 }
	else if(huart->Instance == USART2)
	 {
		//---------------------------------------------------------

		if(rx_cnt_2 <= 3 )
		 {
		   rx_cnt_2 = 0;
		   rx_end_flag_2 = 1;
		 }
		else rx_cnt_2++;
	   //---------------------------------------------------------
      #ifdef USART_Ch_2
	    __HAL_UART_CLEAR_PEFLAG(&huart1);
	    HAL_UART_Receive_DMA(&huart2, (uint8_t *)Rx_data_2, 3);
      #endif
	 }
	else if(huart->Instance == USART3)
	 {
   	  //---------------------------------------------------------

		cnt = rx_buf_3;
		cnt++; // 7

		my_printf_ch = Uart_Ch_2;
		my_printf("%d\t",cnt);

		tx_str("rx3 ok\r\n\n", 2);

		Out_Bit_B(led1_Pin, 0);

		tx_send(cnt, 3);

	 //---------------------------------------------------------
      #ifdef USART_Ch_3
	    __HAL_UART_CLEAR_PEFLAG(&huart3);
	    HAL_UART_Receive_DMA(&huart3, (uint8_t *)rx_buf_3, 1);
      #endif
	 }
}



//=============================================================
/*
// uart test
 uint8_t my_buf[10] = "song\n\r";
 uint8_t user[10] = "sg\n\r";
 uint8_t pin_str_flag;

 uint8_t cnt= 255;

 char k = 'Y';
 float rr = 3.14;
 int ss= 7965539;  //uint16_t ss = 65535;
 char *bak = "Home@# �츮��";
 char *cur;



// UART RTX sample code
// main.c �ȿ� �ش�ȭ�� ȣ���ϱ����� �ݵ�� �ڵ��� �� = uart ���ÿ���... 
#define my_usart_LIB_en 1

 // usart rx int �� ���� ���ͷ�Ʈ ���� main �Լ� while�� ������ �ڵ��Ұ� = ���Ÿ�� ���� = ���� ���ͷ�Ʈ Ȱ��ȭ
#ifdef my_usart_LIB_en
 HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx_data, 1); // 1 byte rx int ����

// �ٸ���� ��
//if (HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx_data, 10) != HAL_OK) {
    //       Error_Handler();
   //    }


  if (HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) {
          Error_Handler();
      }


// ���� ���ͷ�Ʈ ��� sample code     
// usart RX INT func = ���� ���ͷ�Ʈ �Լ�
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
 if(huart -> Instance == USART2)
  {
	  HAL_UART_Transmit(&huart2, bufftx, 10, 100);

    if(Rx_data[0] == '\r')
     {
       rx_cnt = 0;
       rx_end_flag = 1;
       Rx_data[0] = 0;
     }
    else
    {
      rx_buf[rx_cnt++] = Rx_data[0];

      tx_send('K',2);
    }
  }

  // �ٽ� �������ͷ�Ʈ Ȱ��ȭ = �� ����
  HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx_data, 1);
}
*/
/*
// main �Լ� �ȿ��� ����������� ����üũ sample code
// polong rx chk samplae code
void poling_rx_chk(void)
{
	  RcvStat = HAL_UART_Receive(&huart2, Rx_data_2, 1, 100) ;  // receive data
	  if (RcvStat == HAL_OK)
	  	{
	  	  // receive check
	  	  HAL_UART_Transmit(&huart2, Rx_data_2, 1, 100) ;  // echo tx send received data
	   	  HAL_UART_Transmit(&huart2, Rx_data_2, 10, 100);  // send start data

	      // my_printf("uart OK �۸��= %d,hex = %x, float = %.4f, char = %c, string = %s int16 = %d!@#\n\r",cnt, cnt, rr, k, bak, ss);
          //  tx_send('W',2);
          //  tx("uart SMG!\n\r",2);

          //  tx_send(cnt/100 + 0x30,2);
          //  tx_send(cnt%100/10 + '0',2);
          //  tx_send(cnt%10 + 0x30,2);

	   	  // uart rx data test
	   	  // if(UsartData[0] == 'a')  //out_tg(GPIOA, LED1_Pin);
	   	  // else if(UsartData[0] == 'b') //out_tg(GPIOA, GPIO_PIN_6);
	  	}
}

void uart_poling_rtx_test(void)
{
// uart poling rtx test
   poling_rx_chk();
  // tx_send('K',2);
}   
 
void uart_int_test(void)
{  
 // 4. uart rx int test
	if(rx_end_flag_1 == 1)
	 {
	//   tx_send(rx_buf[0],2);
	//   tx_send(rx_buf[1],2);
	//   tx_send(rx_buf[2],2);

	   rx_end_flag_1 = 0;
	 }
}

void sprintf_test(void)
{    
  //sprintf test
	//printf(rcvdata,"%4d",cnt);
	//HAL_UART_Transmit(&huart3, (uint8_t *)&rcvdata, 10,100);

  // �ٸ��ҽ��ڵ�
  // cur = rcvdata;
  // sprintf(cur,"song\n\r");
   //int len = strlen(cur);
  // HAL_UART_Transmit(&huart2, (uint8_t *)&cur, len,100);
}
*/


#endif

