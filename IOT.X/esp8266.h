/* 
 * File:  esp8266.h
 * Author:  TungHT
 * Comments: 
 * Revision history: May 7, 2017, 3:58 PM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ESP8266_H
#define	ESP8266_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#define _XTAL_FREQ                  8000000
#define BAUDRATE                     9600 //bps
#define AT_STARTUP                  "AT"
#define AT_ECHO_ON                  "ATE1"  
#define AT_ECHO_OFF                 "ATE0"  
#define AT_RESET                    "AT+RST"  
#define AT_CFW                      "AT+GMR"  
#define AT_WIFI_MODE                "AT+CWMODE"
#define AT_JOIN_AP                  "AT+CWJAP" 
#define AT_GET_IP                   "AT+CIFSR" 
#define AT_LIST_AP                  "AT+CWLAP"
#define AT_QUIT_AP                  "AT+CWQAP"  
#define AT_SET_AP_PARAMS            "AT+CWSAP" 
#define AT_CHECK_JOIN_AP            "AT+CWQAP" 
#define AT_GET_CON_STATUS           "AT+CIPSTATUS"
#define AT_START_CON                "AT+CIPSTART" 
#define AT_CIP_MODE                 "AT+CIPMODE"  
#define AT_SEND                     "AT+CIPSEND"  
#define AT_CLOSE_CON                "AT+CIPCLOSE"           
#define AT_SET_MUL_CON              "AT+CIPMUX=1"
#define AT_SET_SERVER               "AT+CIPSERVER"  
#define AT_SET_SERVER_TIMEOUT       "AT+CIPSTO"  
#define AT_RECEIVED_DATA            "+IPD"
    
    //char StringReceive[15];
    /* Functions for usart */
    void InitUART(void); // Ham khoi tao UART
    void SendByteSerially(unsigned char); // Gui 1 byte qua UART
    unsigned char ReceiveByteSerially(void); //Nhan 1 byte 
    void SendStringSerially(unsigned char*); //Truyen 1 xau ki tu
    void SendCommandAT(unsigned char*); //Hàm gửi lệnh AT
    //void ReceiveStringSerially(char

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ESP8266_H */


