#ifndef __FINGERPRINT_h
#define __FINGERPRINT_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Define Command list
#define LED_ON  		     0x01 
#define LED_OFF 		     0x00 
#define CMD_TEST_CONNECTION	 0x0001
#define	CMD_SET_PARAM		 0x0002	
#define	CMD_GET_PARAM		 0x0003	
#define	CMD_GET_DEVICE_INFO	 0x0004
#define	CMD_ENTER_IAP_MODE	 0x0005
#define	CMD_GET_IMAGE		 0x0020	
#define	CMD_FINGER_DETECT	 0x0021	
#define	CMD_UP_IMAGE		 0x0022
#define	CMD_DOWN_IMAGE		 0x0023	
#define	CMD_SLED_CTRL        0x0024	
#define	CMD_STORE_CHAR     	 0x0040	
#define	CMD_LOAD_CHAR        0x0041	
#define	CMD_UP_CHAR	         0x0042	
#define	CMD_DOWN_CHAR        0x0043	
#define	CMD_DEL_CHAR         0x0044	
#define	CMD_GET_EMPTY_ID	 0x0045	
#define	CMD_GET_STATUS       0x0046	
#define	CMD_GET_BROKEN_ID	 0x0047	
#define	CMD_GET_ENROLL_COUNT 0x0048	
#define	CMD_GENERATE         0x0060	
#define	CMD_MERGE		     0x0061	
#define	CMD_MATCH		     0x0062	
#define	CMD_SEARCH		     0x0063	
#define	CMD_VERIFY		     0x0064
#define	CMD_SET_MODULE_SN	 0x0008	
#define	CMD_GET_MODULE_SN	 0x0009	
#define	CMD_FP_CANCEL            0x0025	
#define	CMD_GET_ENROLLED_ID_LIST 0x0049	
#define	CMD_ENTER_STANDY_STATE	 0x000C	


void Uart_init(void);
void The_LED(uint8_t led); 
void The_Clear_All(void);
void The_Finger_Detect(void);
void MY_data (void);
void The_Get_Image(void);
void The_Down_Image(void); 
void The_Ram_Buff(uint8_t time); 
void The_Merge_Save(void);
void The_Char_Store(void); 
void The_Verify_Contrast(void);
void The_Verify_Contrast_1(void);
 
void The_Match_Contrast(void);
void The_Search_Contrast(void);
uint8_t The_Sending_Data(uint16_t *p); 
uint8_t Sending_fingerprint(uint16_t *p);
uint8_t add(uint16_t *p); 
void The_Char_Store_1(void);
void The_Get_Status(void);

#endif 

