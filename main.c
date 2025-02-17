#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "nrf_delay.h"
#include "fingerprint.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


// Use the variables from another file we use extern type
// these variables are decalred in Fingerprint.c file and use them in main
extern uint16_t Dectect_GD[26];
extern uint16_t Get_Image_GD[26];
extern uint16_t Contrast_GD[26];
extern uint16_t Search_GD[26];


#define LED_1 21
#define LED_2 22



int main(void)
{   
    uint8_t i;
    nrf_gpio_cfg_output(LED_1); // configure the led pin for showing the status
    nrf_gpio_cfg_output(LED_2); 
    nrf_gpio_pin_set(LED_1); // turn off the led by setting the logic high if the led is connected in reverse logic
    nrf_gpio_pin_set(LED_2);
     

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // initializing the UART by calling a function from the Fingerprint Library
    // so that it is ready to communicate with the sensor
    Uart_init();


    int mode;
    int bias = 48;
    while(1){
    NRF_LOG_INFO("Uarts ready for communication \r\n");
    NRF_LOG_INFO("==========Meun=============== \r\n");
    NRF_LOG_INFO("Create a new user-----------1 \r\n");
    NRF_LOG_INFO("Verify your fingerprint-----2 \r\n");
    NRF_LOG_INFO("==========END================ \r\n");
    NRF_LOG_INFO("Enter your mode: \r\n");


    mode = SEGGER_RTT_WaitKey() - bias;


    switch(mode){

    case 1:
    NRF_LOG_INFO("Now u can create a new user!!\r\n");
    
    NRF_LOG_INFO("Ready to store your fingerprint \r\n");
    int i;
    for (i=0; i<3; i++)
     {
	  The_Finger_Detect(); // function to see if there is any touch detected on the sensor

    // Ask the user to put his/her finger on the sensor
	  NRF_LOG_INFO("Please put your finger on the Sensor \r\n");
          nrf_delay_ms(1000);

        // wait until the user touch is detected
        while(The_Sending_Data(Dectect_GD) == 1)
	   {	
            //check and see if the user has touched the finger if touched 
            The_Finger_Detect();
            The_Sending_Data(Dectect_GD);	
                      
	    }

        // Turn on the backlight LED of sensor to let the user know that its reading the data
	    The_LED(LED_ON);
        nrf_gpio_pin_clear(LED_1); //Turn on the LED


        The_Get_Image(); // Read the fingerprint

        // Check if the image is read then save it to the Ram buffer
        if(The_Sending_Data(Get_Image_GD) == 0)
        {
            The_Ram_Buff(i);			 
        }


        NRF_LOG_INFO("FingerPrint Reading successful... \r\n");
        The_LED(LED_OFF);
        nrf_gpio_pin_set(LED_1);

        The_Finger_Detect(); // check if the user has touched the sensor
        nrf_delay_ms(1000);

		// keep waiting until the user picks up the finger
        while(The_Sending_Data(Dectect_GD) == 0)
        {		
            The_Finger_Detect();
            The_Sending_Data(Dectect_GD);		
        }


       }

    NRF_LOG_INFO("Saving the FingerPrints... \r\n");
    The_Merge_Save(); // put all the buffers into template and merge them
    nrf_delay_ms(10); // give a small delay to let the sensor merge image
    The_Char_Store(); // Save the template data in the internal flash
    NRF_LOG_INFO("Successfuly Saved the FingerPrint \r\n");

    break;
    
    case 2 :
    NRF_LOG_INFO("Now u can Verify your fingerprint!!\r\n");
    
    // check if the user touched the sensor
	The_Finger_Detect() ;
	
    // wait until the user hasn't touched
    while(The_Sending_Data(Dectect_GD)==1)
	{		
        The_Finger_Detect();
        The_Sending_Data(Dectect_GD);		
	}		

	The_LED(LED_ON);
    nrf_gpio_pin_set(LED_1);

	The_Get_Image(); // read the image from the sensor
	
    // check if the image is properly read
    if(The_Sending_Data(Get_Image_GD)==0)
    {
        // if read then copy it to ram buffer where it will be compared with the stored image
        The_Ram_Buff(0); 
    }	
                            
	The_LED(LED_OFF) ;
        
	The_Verify_Contrast(); // Compare the image with the internally save template
	
        // if the image matches tell the user that fingerprint is recognized
        if(Sending_fingerprint(Contrast_GD)==0)
          {
            NRF_LOG_INFO("Fingerprint recognition Successful ... \r\n");
            nrf_gpio_pin_clear(LED_2);
            nrf_delay_ms(1000);
            nrf_gpio_pin_set(LED_2);
            break;
            }

	else 
          {
            // if the image does not match tell the user that it didn't recognized the fingerprint so retry
            NRF_LOG_INFO("Fingerprint Not Recognized... Please Retry \r\n");
            nrf_gpio_pin_clear(LED_1); 
            nrf_delay_ms(1000);
            nrf_gpio_pin_set(LED_1);
            
            }



    break;

    default:
    break;
    }


}

}





