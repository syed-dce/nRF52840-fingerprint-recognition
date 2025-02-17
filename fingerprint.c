#include "fingerprint.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_uart.h"
#include "nrf_uart.h"
#include "app_error.h"
#include "app_util.h"
#include "boards.h"


#define UART1_RX_PIN  19 // Connect TX pin of fingerprint sensor here
#define UART1_TX_PIN  20 // Connect RX pin of Fingerprint sensor here


#define  UART_TX_BUF_SIZE 64 // TX buffer size
#define  UART_RX_BUF_SIZE 64 // RX buffer size

static uint8_t m_received_data[26];

static volatile bool m_data_available = false;
static volatile bool m_data_transmitted = false;
static char m_rec;

// This event handler will be called whenever a uart event occurs
void uart_event_handler(app_uart_evt_t * p_event)
{
	// check if some communcation error occured, this event will be trigered
	if(p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
		// check the reason of error by accessing the error variable and then pass it to error handler
		APP_ERROR_HANDLER(p_event->data.error_communication);
	}
	// check if fifo error occured, this might be fifo overflow or other relevant errors
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{	
		//check the reason of error by accessing the error variable and then pass it to error handler
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
	// check if there is a character available to be read from the buffer
	else if (p_event->evt_type == APP_UART_DATA_READY)
	{
		// Read the available character and store it in the variable
		// app_uart_get(&m_rec);
		// m_data_available = true;
		// print a message over uart port along with the character information
		// printf("The char received over UART is : %c \r\n", c);
	}
	// check if the transmission over uart port is finished. eg. uart tx empty event was generated
	else if (p_event->evt_type == APP_UART_TX_EMPTY)
	{
		//m_data_transmitted = true;	
	}

}

// Function that will configure the UART settings
void Uart_init(void)
{
	uint32_t err_code;  // a variable to hold error value

	// a struct that will hold all the uart configurations
	const app_uart_comm_params_t comm_params =  
	{
    	UART1_RX_PIN,  // Pin number for RX
    	UART1_TX_PIN,  // Pin number for TX
    	RTS_PIN_NUMBER, // Pin number for RTS
    	CTS_PIN_NUMBER, // Pin number for CTS
    	APP_UART_FLOW_CONTROL_DISABLED, // Disable the hardware flow control,
		// we only need it if we are using high baud rates, we can save some pins by disabling this
    	false, // event parity if true, if false no parity
    	NRF_UART_BAUDRATE_115200 
  	};

// initialize the uart with the fifo and pass it the parameters
// Parameters = (configurations, rx buffer size, tx buffer size, event handler, interrupt priority, error code variable)
APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, uart_event_handler, APP_IRQ_PRIORITY_LOWEST, err_code);
APP_ERROR_CHECK(err_code); // check if some error occured during initialization

for(uint8_t i = 0; i<10; i++)
{
    while(app_uart_put(i) != NRF_SUCCESS);
}
}

void uart_read_data(void)
{
	uint8_t c;

  for(uint8_t i; i<26; i++)
  { 
    while(app_uart_get(&c) != NRF_SUCCESS); // receive one byte of data
    m_received_data[i] = c;
	}
    (void)app_uart_flush();
}

// variables which are used as extern
// response
uint16_t Dectect_GD[26] = {
0xAA,0x55,0x01,0x00,0x21,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x01	
} ;//
uint16_t Get_Image_GD[26] = {
0xAA,0x55,0x01,0x00,0x20,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x01
};//

uint16_t Contrast_GD[26] = {
0xAA,0x55,0x01,0x00,0x64,0x00,0x05,0x00,0x00,0x00,0x01,0x00,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6b,0x01	
};

uint16_t Match_GD[26] ={
0xAA,0x55,0x01,0x00,0x62,0x00,0x02,0x00,0x00,0x00,0x08,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64,0x01	
};

uint16_t Search_GD[26] ={
0xAA,0x55,0x01,0x00,0x63,0x00,0x05,0x00,0x00,0x00,0x01,0x00,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0x01	
};

uint16_t Status_GD[26] ={
0xAA,0x55,0x01,0x00,0x46,0x00,0x03,0x00,0x00,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4a,0x01	
};

uint16_t get_buff[26] ;// receive buffer


static void Uart_Send_Data(uint8_t data)
{
    // a function to transmit the data over uart
    /*app_uart_put(data);
    while(!m_data_transmitted);
    m_data_transmitted = false;
*/
    while(app_uart_put(data) != NRF_SUCCESS);
}

static void Send_Header(void)
{
	Uart_Send_Data(0x55);
	Uart_Send_Data(0xaa);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
}

static void Send_CMD(uint8_t cmd)
{
	Uart_Send_Data(cmd);
}

static void Send_Length(int length)
{
	Uart_Send_Data(length>>8);
	Uart_Send_Data(length);
}

static void Send_Control(uint16_t control)
{
	Uart_Send_Data(control>>8);
	Uart_Send_Data(control);
	
}

static void Send_Check(uint16_t check)
{
	Uart_Send_Data(check);	
	Uart_Send_Data(check>>8);	
}

static void Send_Data()//
{
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
}

void MY_data (void) //
{
	uint8_t t; 
	uart_read_data(); // wait until 26 chars are received

	for(t=0;t<26;t++)
	{	
		get_buff[t]=   m_received_data[t]; // store data in the main buffer
        m_received_data[t] = 0x00; // clear the buffer
                
	}
			
}

void The_LED (uint8_t led) 
{
	uint16_t temp;
	Send_Header();
	Send_CMD(CMD_SLED_CTRL);
	Send_Length(0x02);
	Send_Control(led);
	Send_Data();
	temp = 0x55+0xaa+CMD_SLED_CTRL+0x02+led ;
	Send_Check(temp);
	MY_data ();//
}

void The_Clear_All(void)
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_DEL_CHAR);
	Send_Length(0x04);
	Send_Control(0x01) ;
	Uart_Send_Data(0x00);
	Uart_Send_Data(0xd0);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_DEL_CHAR+0x04+0x01+0xD0;
	Send_Check(temp);
	MY_data(); //
}

void The_Finger_Detect(void)//
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_FINGER_DETECT);
	Send_Length(0x00);
	Send_Control(0x00) ;
	Send_Data();
	temp = 0x55+0xaa+CMD_FINGER_DETECT ;
	Send_Check(temp);
	MY_data() ; //
}

void The_Get_Image(void)//
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_GET_IMAGE);
	Send_Length(0x00);
	Send_Control(0x00);
	Send_Data();
	temp = 0x55+0xaa+CMD_GET_IMAGE ;
	Send_Check(temp);
	MY_data() ; //
}

void The_Ram_Buff(uint8_t time)
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_GENERATE);
	Send_Length(0x02);
	Send_Control(time);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_GENERATE+0x02+time;
	Send_Check(temp);
	MY_data ();//
}

void The_Merge_Save(void)//
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_MERGE);
	Send_Length(0x03);
	Send_Control(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x03);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_MERGE+0x03+0x03;
	Send_Check(temp);
	MY_data ();//

}

void The_Char_Store(void) 
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_STORE_CHAR);
	Send_Length(0x04);
	Send_Control(0x01);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_STORE_CHAR+0x01+0x04;
	Send_Check(temp);
	MY_data ();//
}

void The_Verify_Contrast(void) 
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_VERIFY);
	Send_Length(0x04);
	Send_Control(0x01);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_VERIFY+0x04+0x01;
	Send_Check(temp);
	MY_data ();
}	

void The_Match_Contrast(void)
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_MATCH);
	Send_Control(0x04);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x01);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_MATCH+0x04+0x01;
	Send_Check(temp);
	MY_data ();//

}

void The_Search_Contrast(void)
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_SEARCH);
	Send_Control(0x06);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x01);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0xD0);
	Uart_Send_Data(0x07);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_SEARCH+0x01+0xD0+0x07+0x06;
	Send_Check(temp);
	MY_data ();;//
}

void The_Down_Image(void) 
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_DOWN_IMAGE);
	Send_Length(0x04);
	Send_Control(0xca);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x02);
	Uart_Send_Data(0x01);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_DOWN_IMAGE+0x02+0x01+0x04+0xca;
	Send_Check(temp);
	MY_data() ; //
}

void The_Get_Status(void) 
{
	uint16_t temp ;
	Send_Header();
	Send_CMD(CMD_GET_STATUS);
	Send_Length(0x02);
	Send_Control(0x10);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	Uart_Send_Data(0x00);
	temp = 0x55+0xaa+CMD_GET_STATUS+0x02+0x10;
	Send_Check(temp);
	MY_data() ; //
}

uint8_t The_Sending_Data(uint16_t *p)
{
	uint8_t i,j=0  ;
	
	uint16_t make[26] ; 
	for (i=0; i<26; i++)
	{
		make[i]=p[i];
	}
	for (i=0; i<26; i++)
	{
		if (make[i] != get_buff[i])
		{
			j = 1 ;
			break ;
		}
		j = 0 ;
	}
	return j  ;
}

uint8_t Sending_fingerprint(uint16_t *p) 
{
	uint8_t i,j=0  ;
	
	uint16_t make[26] ; 
	for (i=0; i<26; i++)
	{       
                           
		make[i]=p[i];
	}
	if (make[6] != get_buff[6])
	{
		j = 1 ;
	}
	else j = 0 ;
	return j  ;
}

uint8_t add(uint16_t *p) 
{
	uint8_t i,j=0  ;
	int x;
	uint16_t make[26]; 
        
       
	for (i=0; i<26; i++)
	{       
                if (p[10] == 0x01){
                    p[10] += 0;
                }
                
		make[i]=p[i];
	}
        
	if (make[6] != get_buff[6])
	{
		j = 1 ;
	}
	else j = 0 ;
	return j  ;
}





