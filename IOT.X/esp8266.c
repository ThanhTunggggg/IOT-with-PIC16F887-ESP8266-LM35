/* 
 * File:  esp8266.c
 * Author:  TungHT
 * Comments: 
 * Revision history: May 7, 2017, 3:58 PM
 */

#include "esp8266.h"

void InitUART()
    {
    //cau hinh chan truyen,nhan
    TRISC6 = 0; // TX Pin
    TRISC7 = 1; // RX Pin

    //cau hinh toc do
    TXSTAbits.BRGH = 1; // Fast baudrate
    SPBRG = (_XTAL_FREQ / (BAUDRATE * 16)) - 1; //Cong thuc tinh SPBRG khi BRGH=1

    //cau hinh truyen
    TXSTAbits.SYNC = 0; // Asynchronous
    RCSTAbits.SPEN = 1; // Enable serial port pins
    //TXSTAbits.TXEN = 0; // Reset transmitter
    TXSTAbits.TXEN = 1; // Enable the transmitter
    TXSTAbits.TX9 = 0; // 8-bit transmission
   
    //cau hinh nhan
    RCSTAbits.CREN = 1; // Enable reception
    //RCSTAbits.SREN = 0; // No effect in Asynchronous
    RCSTAbits.RX9 = 0; // 8-bit reception

    //cho phep ngat truyen va nhan
    //PIE1bits.TXIE = 1; // Enable tx interrupts
    //PIE1bits.RCIE = 1; // Enable rx interrupts
    }

/*char InitUART(const long int baudrate)
{
  unsigned int x;
  x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
  if(x>255)                                       //If High Baud Rage Required
  {
    x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
    BRGH = 1;                                     //Setting High Baud Rate
  }
  if(x<256)
  {
    SPBRG = x;                                    //Writing SPBRG Register
    SYNC = 0;                                     //Setting Asynchronous Mode, ie UART
    SPEN = 1;                                     //Enables Serial Port
    TRISC7 = 1;                                   //As Prescribed in Datasheet
    TRISC6 = 1;                                   //As Prescribed in Datasheet
    CREN = 1;                                     //Enables Continuous Reception
    TXEN = 1;                                     //Enables Transmission
    return 1;                                     //Returns 1 to indicate Successful Completion
  }
  return 0;                                       //Returns 0 to indicate UART initialization failed
}*/

void SendByteSerially(unsigned char Byte) // Writes a character to the serial port
    {
    while (!TXIF); // wait for previous transmission to finish
    TXREG = Byte;
    }

unsigned char ReceiveByteSerially(void) // Reads a character from the serial port
    {
    if (OERR) // If over run error, then reset the receiver
        {
        CREN = 0;
        CREN = 1;
        }
    while (!RCIF); // Wait for transmission to receive
    return RCREG;
    }

void SendStringSerially(unsigned char* st)
    {
    while (*st)
        SendByteSerially(*st++);
    }

void SendCommandAT(unsigned char* command)
    {
    //strcpy(command,"\r\n");
    SendStringSerially(command);
    //unsigned char* temp[3] = {'\r','\n','\0'};
    //SendByteSerially(0x0d);
    //SendByteSerially(0x0a);
    SendStringSerially("\r\n");
    }

/*void ReceiveStringSerially(char* ReceiveString)
{
   unsigned int i=0;
   unsigned char ReceiveByte; 
  // for(i=0;i<length;i++)
   ReceiveByte = ReceiveByteSerially();
   while(ReceiveByte!=10)
   { 
      ReceiveString[i] = ReceiveByte;
      ReceiveByte = ReceiveByteSerially();
      i++;
   }
}*/

