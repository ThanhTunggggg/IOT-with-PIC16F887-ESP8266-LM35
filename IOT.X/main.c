/*
 * File:   main.c
 * Author: TungHT
 *
 * Created on May 7, 2017, 3:58 PM
 */

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "KeyPad.h"
#include "esp8266.h"
#include "LedMatrix.h"
//#include "LedMatrix.c"
//#include "KeyPad.c"
//#include "esp8266.c"

/* CONFIG1 */
#pragma config FOSC = HS         // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF        // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF       // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

/* CONFIG2 */
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/* Macro */
#ifndef _XTAL_FREQ   
#define _XTAL_FREQ                  8000000
#endif   
#define LED_1                       RC0
#define LED_2                       RC1
#define LED_3                       RC2
#define LED_4                       RC3
#define LED_PORT(x)                 {LED_1 = (x & 1); LED_2 = ((x>>1) & 1); LED_3= ((x>>2) & 1); LED_4 = ((x>>3) & 1);}
#define LED_TRIS(x)                 (TRISC0=TRISC1=TRISC2=TRISC3=x)
#define PIN_ADC_LOW                 1 //AN0
#define PIN_ADC_HIGH                0 //AN0
#define TIMER0_START                56
#define TIMER0_LOOP                 20 //80
#define TIMER1_START                65336
#define TIMER1_LOOP                 20
#define TIME_NEXT                   2
#define WIFI_NAME                   "Vinh Bac Bo"
#define PASSWORD                    "barcareal"
#define API                         "9YDIVRVEGBWI8JNF"
#define FIELD                       "field1"
#define WRITE_API_KEY               "9YDIVRVEGBWI8JNF"
#define OK                          'D'
#define CANCEL                      'C'
#define STAR                        '*'
#define SHARP                       '#'
#define MODE_DEFAULT                'B'
#define TIMER2_START                56
#define TIMER2_FINISH               255
#define TIMER2_LOOP                 10

//void interrupt high_priority ISR(void); // Khai bao ham ngat ngoai
//void interrupt low_priority TIMER0(); //ngat timer0

/* Bien cho ADC */
//char adc; // nhiet do tuc thoi
//char cout_adc = 0; //bien dem so lan do
//long int sum_adc = 0; //Nhiet do tong 10 lan do
long adc_lm35; //bien nhiet do trung binh
//char ones, tens; //gia hang chuc, hang don vi cua nhiet do
//unsigned char char_tens = 48, char_ones = 48; //Ma ASCII chu so hang chuc, hang don vi
char Adc_lm35String[5];
/* Bien cho timer0 */
unsigned int cout = 0; // dem so lan lap
char up = 0, down = 1; //Bien xac dinh vi tri den dang sang va chieu den sang

/* Bien cho timer1 */
unsigned char sec;

/*Bien cho UART*/
unsigned char StringReceive[30]; // Xau chua gia tri gui, nhan tu USART
unsigned char pos; //Bien vi tri cho xau StringReceive
unsigned char WifiName[30]="_";
unsigned char PassWord[30]="_";
char mode = MODE_DEFAULT;
char StringDisplay[32];
void SetEsp8266AsClient(void);

/*Bien chay va do dai chuoi hien thi da duoc dinh nghia tai LedMatrix.h*/
extern unsigned int next;
extern unsigned int LenghString;
int CountNext = 0;

/* Ham clear xau nhan */
void ClearStringReceive()
    {
    pos = 0;
    for (int i = 0; i < 30; i++)
        StringReceive[i] = '\0';
    }

void delay(unsigned long a)
    {
    while (a--);
    }

void delay1sec()//delay 1s
    {
    for (int i = 0; i < 20; i++)
        {
        TMR1H = 0x3a;
        TMR1L = 0xbb;
        TMR1ON = 1;
        while (TMR1IF == 0);
        TMR1ON = 0;
        TMR1IF = 0;
        }
    sec++;
    }

/* Khai bao ngat */
void interrupt ISR(void);

/* main */
void main(void)
    {
    /* Cau hinh ADC */
    ANSELH = PIN_ADC_HIGH;
    ANSEL = PIN_ADC_LOW; //chan RA0 la chan Anolog
    ADCON0bits.CHS0 = CHS1 = CHS2 = CHS3 = 0; // chon kenh AN0 lam bo chuyen doi
    ADCON0bits.ADON = 1; //cho phep ADC hoat dong
    ADCON0bits.ADCS1 = 0;
    ADCON0bits.ADCS0 = 0; //chan xung clock cho ADC bang Fosc/2
    ADCON1bits.ADFM = 1; // ket qua 10bit luu o ben phai 2 thanh ghi ADRESH:ADRESL
    ADCON1bits.VCFG0 = VCFG1 = 0; //dien ap tham chieu la Vref+ = Vdd, Vref- = Vss
    PIR1bits.ADIF = 0; // clear co ngat
    ADCON0bits.GO_nDONE = 1; //khoi dong ADC

    /* Cau hinh PORT cho LED bao trang thai va ngat ngoai */
    LED_TRIS(0); //Den bao trang thai
    LED_PORT(0); //xuat Led o PortD, ban dau cac Led tat
    OPTION_REGbits.nRBPU = 0; //cho dien tro keo len PORTB
    TRISBbits.TRISB0 = 1; // Chan RB0 la ngo vao phuc vu ngat ngoai

    /* Cau hinh Timer0 */
    TMR0 = TIMER0_START;
    OPTION_REGbits.T0CS = 0; //su dung xung noi  Fosc/4
    OPTION_REGbits.PSA = 0; // gan bo chia cho timer0 (k cho watchdog timer)
    OPTION_REGbits.PS2 = 0;
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 0; //bo chia tan 1:2

    /* Cau hinh Timer1 */
    TMR1 = TIMER1_START;
    T1CONbits.TMR1CS = 0; //su dung xung noi ben trong Fosc/4
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0; //bo chia tan 1:1
    T1CONbits.TMR1ON = 1;
    PIR1bits.TMR1IF = 0;

    /* Cau hinh Timer2 */
    //    //TMR2 = TIMER2_START; //
    //    T2CONbits.TMR2ON = 1; // cho phep Timer2 dem
    //    T2CONbits.T2CKPS0 = 0;//
    //    T2CONbits.T2CKPS1 = 0;// Bo chia tan 1:1
    //    PR2 = TIMER2_FINISH;  // Diem TMR2 reset ve 0    

    /* Cau hinh ngat ngoai */
    //OPTION_REGbits.INTEDG = 0; // chon suon xuong de ngat

    /* Cho phep ngat */
    INTCONbits.GIE = 1; // Bat ngat toan cuc
    INTCONbits.PEIE = 1; // Bat ngat ngoai vi
    //INTCONbits.INTE = 1; // cho phep ngat ngoai
    INTCONbits.TMR0IE = 1; // cho phep ngat Timer0
    //INTCONbits.RBIE = 1; // ngat portb
    // PIE1bits.TMR1IE = 1; //Ngat Timer1
    // PIE1bits.ADIE = 1; //cho phep ngat ADC
    //PIE1bits.TMR2IE = 1; //Ngat Timer2
    //PIE1bits.TXIE = 1; // Enable tx interrupts
    PIE1bits.RCIE = 1; // Enable rx interrupts

    /* Khoi dong cac giao tiep ngoai vi */
    InitLedMatrix();
    InitKeyPad();
    InitUART(); // Initialize UART

    /* Thiet lap dien tro keo len o PorB B*/
    //WPUB = 0xF0; //Dien tro keo len o chan RB70-RD4
    OPTION_REGbits.nRBPU = 0; //Cho phep dien tro keo len

    /* Vong lap chinh */
    while (1)
        {
        delay(100);
               next++;
               if (next == LenghString)
                   next = 0;
               LED_1 = 1;
               LED_2 = 0;
               delay(1000);
               LED_1 = 0;
               LED_2 = 1;
               delay(1000);
               mode = GetKey();
        
        display(Adc_lm35String);
        SetEsp8266AsClient();
               // switch (mode)
                   // {
                   // case 'B': //SetEsp8266AsServer(); // An phim B de dung esp8266 nhu sever phat wifi
                       // break;
                   // default:
                       // SetEsp8266AsClient(); // Mac dinh che do esp8266 nhu client ket noi internet
                       // break;
                   // }
               // if (ADCON0bits.GO_nDONE == 0)
                   // {
                   // //Ngat ADC khi  ADCON0bits.GO_nDONE = 0
                   // adc = ADRESH * 256 + ADRESL; //lay ket qua tu thanh ghi
                   // //lay ket qua trung binh cua 10 lan do
                   // if (cout_adc == 10)
                       // {
                       // adc_lm35 = (char) (sum_adc * (0.49 / 10)); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
                       // Adc_lm35String[0] = ' ';
                       // Adc_lm35String[1] = (adc_lm35 / 100) + 48;
                       // Adc_lm35String[2] = ((adc_lm35 % 100) / 10) + 48;
                       // Adc_lm35String[3] = (adc_lm35 % 10) + 48;
                       // cout_adc = 0;
                       // sum_adc = 0;
                       // }
                   // else
                       // {
                       // sum_adc += adc;
                       // cout_adc++;
                       // }
                   // PIR1bits.ADIF = 0;
                   // ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
                   // }
        }
    return;
    }

//        if(PIR1bits.RCIF==1)
//    {
//        char ReceiveString[15];
//        char ReceiveByte;
//        int i = 0;
//        ReceiveByte = ReceiveByteSerially();
//        SendByteSerially(ReceiveByte); //send the received byte to dislay on terminal
//        while (ReceiveByte != 13 && ReceiveByte != 9)//press ENTER or Tab to finish adding character to string
//        {
//            ReceiveString[i] = ReceiveByte;
//            ReceiveByte = ReceiveByteSerially();
//            SendByteSerially(ReceiveByte);
//            i++;
//        }
//        if (((ReceiveString[0] >= 'a') && (ReceiveString[0] <= 'z')) && i < 2)//if string has a lower character in aphabeta
//        {
//            SendByteSerially(ReceiveString[0] - 32); //send capital of character
//            SendByteSerially(13); //ASCII for ENTER to new line
//        }
//        if (!(strcmp(ReceiveString, "hello"))) //string compare "hello"
//        {
//            SendStringSerially("WORLD");
//            SendByteSerially(13);
//        }
//        if (!(strcmp(ReceiveString, "abcd efgh"))) {
//            SendStringSerially("1234 5678");
//            SendByteSerially(13);
//        }
//        i = 0;
//        strcpy(ReceiveString, ""); //emply string
//    }
// SendByteSerially(a);

/* Ham phuc vu ngat */
void interrupt ISR()
    {

    if (PIR1bits.RCIF == 1 && pos < 28)//Ngat nhan UART
        {
        StringReceive[pos] = RCREG;
        pos++;
        RCIF = 0;
        }

    //    if (INTCONbits.INTF == 1)
    //        { //Ngat ngoai
    //        while (RB0 == 0); //
    //        while (RB0 == 1); //Nha tay khoi nut bam mach van dung
    //        INTCONbits.INTF = 0; // xoa co ngat de mach tiep tuc chay
    //        }



    //    if (PIR1bits.TMR1IF == 1)
    //        {
    //        //mode = ScanKey();
    //        //  RunString("codientu");
    //        //            if (key != '\0')
    //        //                display(key);
    //        TMR1 = TIMER1_START;
    //        T1CONbits.TMR1ON = 1;
    //        TMR1IF = 0;
    //        }


    //    if (TMR1IF == 1)
    //        {
    //        //RunString(Adc_lm35String);
    //        TMR1 = TIMER1_START;
    //        //TMR1H = 0x3a;
    //        //TMR1L = 0xbb;
    //        TMR1ON = 1;
    //        TMR1IF = 0;
    //        }
    if (INTCONbits.TMR0IF == 1)//Ngat Timer0
        {
        cout++;
        if (cout == TIMER0_LOOP)//cho tran timer0 156 lan
            {
            cout = 0;
            //            if (up < 4)
            //                {
            //                // LED_PORT(0x01 << up);
            //                up++;
            //                }
            //            else
            //                {
            //                LED_PORT(0x08 >> down);
            //                down++;
            //                }
            //            if (down > 3)
            //                {
            //                up = 1;
            //                down = 1;
            //                }

            if (ADCON0bits.GO_nDONE == 0) //ADC
                {
                //Ngat ADC khi  ADCON0bits.GO_nDONE = 0
                adc_lm35 = (ADRESH << 8) + ADRESL; //lay ket qua tu thanh ghi
                adc_lm35 = (int) (adc_lm35 * 0.49); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
                Adc_lm35String[0] = ' ';
                Adc_lm35String[1] = ' ';
                Adc_lm35String[2] = (adc_lm35 / 10) + 48;
                Adc_lm35String[3] = (adc_lm35 % 10) + 48;
                Adc_lm35String[4] = '\0';
                PIR1bits.ADIF = 0;
                ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
                }
            //            if (ADCON0bits.GO_nDONE == 0) { //Ngat ADC khi  ADCON0bits.GO_nDONE = 0
            //                adc_lm35 = ADRESH * 256 + ADRESL; //lay ket qua tu thanh ghi
            //                adc_lm35 = (char) (adc_lm35 * 0.49/2); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
            //                //            Temp= adc_lm35;
            //                //            ones=Temp%10;
            //                //            tens=Temp/10;
            //                char_ones = adc_lm35 % 10 + 48; //Ma ASCII cua chu so hang don vi cua nhiet do
            //                char_tens = adc_lm35 / 10 + 48; //Ma ASCII cua chu so hang chuc cua nhiet do
            //                PIR1bits.ADIF = 0;
            //                ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
            //            }
            }
        //if (mode != '0')
        //display(StringDisplay); //hien thi nhiet do bang Led ma tran
        //else
        display(Adc_lm35String);
        CountNext++;
        if (CountNext == TIME_NEXT)
            {
            CountNext = 0;
            next++;
            if (next == LenghString)
                next = 0;
            }
        TMR0 = TIMER0_START;
        INTCONbits.TMR0IF = 0;
        }


    //    if (PIR1bits.ADIF == 1)
    //        { //Ngat ADC khi  ADCON0bits.GO_nDONE = 0
    //        adc = ADRESH * 256 + ADRESL; //lay ket qua tu thanh ghi
    //        //lay ket qua trung binh cua 10 lan do
    //        if (cout_adc == 10)
    //            {
    //            adc_lm35 = (char) (sum_adc * (0.49 / 10)); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
    //            Adc_lm35String[0] = ' ';
    //            Adc_lm35String[1] = (adc_lm35 / 100) + 48;
    //            Adc_lm35String[2] = ((adc_lm35 % 100) / 10) + 48;
    //            Adc_lm35String[3] = (adc_lm35 % 10) + 48;
    //            cout_adc = 0;
    //            sum_adc = 0;
    //            }
    //        else
    //            {
    //            sum_adc += adc;
    //            cout_adc++;
    //            }
    //        PIR1bits.ADIF = 0;
    //        ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
    //        }


    //        if (PIR1bits.ADIF == 1) { //Ngat ADC khi  ADCON0bits.GO_nDONE = 0
    //            adc_lm35 = ADRESH * 256 + ADRESL; //lay ket qua tu thanh ghi
    //            adc_lm35 = (char) (adc_lm35 * 0.49); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
    //            //            Temp= adc_lm35;
    //            //            ones=Temp%10;
    //            //            tens=Temp/10;
    //            char_ones = adc_lm35 % 10 + 48; //Ma ASCII cua chu so hang don vi cua nhiet do
    //            char_tens = adc_lm35 / 10 + 48; //Ma ASCII cua chu so hang chuc cua nhiet do
    //            dislayTempOnLedMatrix(char_tens, char_ones); //hien thi nhiet do bang Led ma tran
    //            PIR1bits.ADIF = 0;
    //            ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
    //        }

    //    if (PIR1bits.RCIF == 1)
    //        {
    //        char ReceiveByte;
    //        ReceiveByte = RCREG;
    //        SendByteSerially(ReceiveByte); //send the received byte to dislay on terminal
    //        if (ReceiveByte != 13 && ReceiveByte != 9)//press ENTER or Tab to finish adding character to string
    //            {
    //            ReceiveString[z] = ReceiveByte;
    //            ReceiveByte = ReceiveByteSerially();
    //            SendByteSerially(ReceiveByte);
    //            z++;
    //            }
    //        else
    //            {
    //            if (((ReceiveString[0] >= 'a') && (ReceiveString[0] <= 'z')) && z < 2)//if string has a lower character in aphabeta
    //                {
    //                SendByteSerially(ReceiveString[0] - 32); //send capital of character
    //                SendByteSerially(13); //ASCII for ENTER to new line
    //                }
    //            if (!(strcmp(ReceiveString, "hello"))) //string compare "hello"
    //                {
    //                SendStringSerially("WORLD");
    //                SendByteSerially(13);
    //                }
    //            if (!(strcmp(ReceiveString, "abcd efgh")))
    //                {
    //                SendStringSerially("1234 5678");
    //                SendByteSerially(13);
    //                }
    //            z = 0;
    //            strcpy(ReceiveString, ""); //emply string
    //            }
    //        }

    }

/*void interrupt ISR(void)
{
if(RCIF)  // If UART Rx Interrupt
{
//  if(OERR) // If over run error, then reset the receiver
// {
//   CREN = 0;
//      CREN = 1;
//  }
SendByteSerially(RCREG);    // Echo back received char
ReceiveByte = RCREG;
if(RCREG!=10)
{ 
ReceiveString[i] = RCREG;
    
i++;
}
else
{
if ((ReceiveString[0]>='a' && ReceiveString[0]<= 'z') && i <1)
SendByteSerially(*ReceiveString-32);
i=0;
} 
}
}*/
void setesp8266asclient(void)
   {
   led_1 = 1;
   strcpy(stringdisplay, "  nhap hostname!");
   delay(2000);
   char i = 0;
   char ascii_huns, ascii_tens, ascii_ones;
   long ascii_char;
   while (1)
       {
       led_4 = 0;
       strcpy(stringdisplay, wifiname);
       ascii_huns = getkey();
       if (ascii_huns == cancel)
           {
           strcpy(wifiname, wifi_name);
           break;
           }
       if (ascii_huns == ok)
           {
           break;
           }
       if ((ascii_huns == star) || (ascii_huns == sharp) || (ascii_huns == 'b') || (ascii_huns == 'a'))
           continue;
       strcpy(stringdisplay, "");
       stringdisplay[0] = ascii_huns;
       stringdisplay[1] = '_';
       stringdisplay[2] = '\0';

       ascii_tens = getkey();
       if (ascii_tens == cancel)
           {
           strcpy(wifiname, wifi_name);
           break;
           }
       if ((ascii_tens == star) || (ascii_tens == sharp) || (ascii_tens == 'b') || (ascii_tens == 'a'))
           continue;
       strcpy(stringdisplay, "");
       stringdisplay[0] = ascii_huns;
       stringdisplay[1] = ascii_tens;
       stringdisplay[2] = '_';
       stringdisplay[3] = '\0';

       ascii_ones = getkey();
       if (ascii_ones == cancel)
           {
           strcpy(wifiname, wifi_name);
           break;
           }
       if ((ascii_ones == star) || (ascii_ones == sharp) || (ascii_ones == 'b') || (ascii_ones == 'a'))
           continue;
       strcpy(stringdisplay, "");
       stringdisplay[0] = ascii_huns;
       stringdisplay[1] = ascii_tens;
       stringdisplay[2] = ascii_ones;
       stringdisplay[3] = '\0';

       ascii_char = (ascii_huns - 48) * 100 + (ascii_tens - 48) * 10 + (ascii_ones - 48); //(ascii_huns << 3 + ascii_huns << 1) << 3 + (ascii_huns << 3 + ascii_huns << 1) << 1 + ascii_tens << 3 + ascii_tens << 1 + ascii_ones;

       if ((ascii_char > 32) && (ascii_char < 128))
           {
           stringdisplay[3] = ' ';
           stringdisplay[4] = ascii_char;
           stringdisplay[5] = '\0';
           wifiname[i] = ascii_char;
           wifiname[++i] = '\0';
           led_4 = 1;
           }
       delay(300);
       }
   strcpy(stringdisplay, wifiname);
   delay(2000);

   strcpy(stringdisplay, "  nhap password!");
   i = 0;
   delay(2000);
   while (1)
       {
       led_4 = 0;
       strcpy(stringdisplay, password);
       ascii_huns = getkey();
       if (ascii_huns == ok)
           {
           break;
           }
       if (ascii_huns == cancel)
           {
           strcpy(password, password);
           break;
           }
       if ((ascii_huns == star) || (ascii_huns == sharp) || (ascii_huns == 'b') || (ascii_huns == 'a'))
           continue;
       strcpy(stringdisplay, "");
       stringdisplay[0] = ascii_huns;
       stringdisplay[1] = '_';
       stringdisplay[2] = '\0';

       ascii_tens = getkey();
       if (ascii_tens == cancel)
           {
           strcpy(password, password);
           break;
           }
       if ((ascii_tens == star) || (ascii_tens == sharp) || (ascii_tens == 'b') || (ascii_tens == 'a'))
           continue;
       strcpy(stringdisplay, "");
       stringdisplay[0] = ascii_huns;
       stringdisplay[1] = ascii_tens;
       stringdisplay[2] = '_';
       stringdisplay[3] = '\0';

       ascii_ones = getkey();
       if (ascii_ones == cancel)
           {
           strcpy(password, password);
           break;
           }
       if ((ascii_ones == star) || (ascii_ones == sharp) || (ascii_ones == 'b') || (ascii_ones == 'a'))
           continue;
       strcpy(StringDisplay, "");
        StringDisplay[0] = ASCII_huns;
        StringDisplay[1] = ASCII_tens;
        StringDisplay[2] = ASCII_ones;
        StringDisplay[3] = '\0';

        ASCII_char = (ASCII_huns - 48) * 100 + (ASCII_tens - 48) * 10 + (ASCII_ones - 48); //(ASCII_huns << 3 + ASCII_huns << 1) << 3 + (ASCII_huns << 3 + ASCII_huns << 1) << 1 + ASCII_tens << 3 + ASCII_tens << 1 + ASCII_ones;

        if ((ASCII_char > 32) && (ASCII_char < 128))
            {
            StringDisplay[3] = ' ';
            StringDisplay[4] = ASCII_char;
            StringDisplay[5] = '\0';
            PassWord[i] = ASCII_char;
            PassWord[++i] = '\0';
            LED_4 = 1;
            }
        delay(300);
        }
    strcpy(StringDisplay, PassWord);
    delay(2000);
    LED_1 = 0;
    LED_2 = 1;
    strcpy(StringDisplay, Adc_lm35String);
    delay(2000);

    /*** SENDING COMMANDS ***/

    char x, flag;

    /* AT : tests AT startup */
    ClearStringReceive();
    SendCommandAT("AT"); //Kiem tra esp8266 hoat dong chua?
    //strcpy(StringDisplay, "AT");
    while (1)
        {
        x = strstr(StringReceive, "OK"); //Kiem tra phan hoi tu esp8266
        if (x != NULL)
            break;
        }
    strcpy(StringDisplay, "AT  OK");
    delay(1000);

    /* ATE : Switches echo */
    ClearStringReceive();
    SendCommandAT("ATE0"); //Tat echo,esp8266 chi gui lai ket qua, khong gui lai lenh dươc nhan
    strcpy(StringDisplay, "ATE0");
    while (1)
        {
        x = strstr(StringReceive, "OK");
        if (x != NULL)
            break;
        }
    strcpy(StringDisplay, "ATE0  OK");
    delay(1000);

    /* AT+CWMODE=<mode> : chon che do hoat dong*/
    ClearStringReceive();
    SendCommandAT("AT+CWMODE=3"); //Chon che do Station/clint va Access Point
    while (1)
        {
        x = strstr(StringReceive, "OK");
        if (x != NULL)
            break;
        }
    strcpy(StringDisplay, "... ");
    delay(1000);

    /* AT+CIPMUX=<mode> : cai dat so luong kenh ket noi */
    ClearStringReceive();
    SendCommandAT("AT+CIPMUX=1"); //Che do ket noi da kenh
    while (1)
        {
        x = strstr(StringReceive, "OK");
        if (x != NULL)
            break;
        }
    strcpy(StringDisplay, "....");
    delay(1000);

    /* AT+CWJAP=<ssid>,<pwd>[,<bssid>] : ket noi voi 1 mang wifi */
    ClearStringReceive();
    SendStringSerially("AT+CWJAP=\"");
    SendStringSerially(WifiName);
    SendStringSerially("\",\"");
    SendStringSerially(PassWord);
    SendCommandAT("\""); // Ket noi wifi co ten la gia tri cua bien WifiName, mat khau la gia tri cua bien PassWord
    //SendCommandAT("AT+CWJAP=\"WifiName\",\"PassWord\""); //Ket noi  WIFI
    while (1)
        {
        x = strstr(StringReceive, "OK");
        if (x != NULL)
            break;
        }
    strcpy(StringDisplay, " OK ");
    delay(1000);

    sec = 0;

    /* Gui du lieu lien tuc theo thoi gian */
    while (1)
        {
        //        ADCON0bits.GO_nDONE = 1; //  Khoi dong lai ADC
        //        while (ADCON0bits.GO_nDONE == 1);
        //        adc_lm35 = (ADRESH << 8) + ADRESL; //lay ket qua tu thanh ghi
        //        adc_lm35 = (char) (adc_lm35 * 0.49); // adc_lm35=(int)(adc_lm35*5000.0f/1023)/10); //chuyen doi sang do C
        //        char_ones = adc_lm35 % 10 + 48; //Ma ASCII cua chu so hang don vi cua nhiet do
        //        char_tens = adc_lm35 / 10 + 48; //Ma ASCII cua chu so hang chuc cua nhiet do
        //        //delay(20000);
        LED_3 = 0;
        strcpy(StringDisplay, Adc_lm35String);
        delay1sec();
        if (sec == 30)// 30s gui nhiet do 1 lan
            {
            //delay(60000);

            /* AT+CIPSTART=<link ID>,<type>,<remote IP>,<remote	port>[,<TCP	keep alive>] : ket noi voi sever (mode: AT+CIPMUX=1) */
            ClearStringReceive();
            SendCommandAT("AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80"); //184.106.153.149 la IP link to thingspeak.com .Chon kenh 4, giao thuc TCP, port 80
            sec = 0;
            delay(1000);
            while (1)
                {
                x = strstr(StringReceive, "OK"); //"Linked"
                if (x != NULL)
                    {
                    //StringDisplay = " LOI";
                    flag = 1;
                    break;
                    }
                else
                    {
                    //StringDisplay = " OK ";
                    flag = 0;
                    break;
                    }
                }
            // delay(10000);

            if (flag == 1)
                {
                /* AT+CIPSEND=<link ID>,<length> : Gui chieu dai du lieu can truyen toi server (mode: AT+CIPMUX=1) */
                ClearStringReceive();
                SendCommandAT("AT+CIPSEND=4,46"); //Truyen qua kenh 4, du lieu 46byte (tinh ca 2 byte cuoi "\r\n")
                delay(1000);
                while (1)
                    {
                    x = strstr(StringReceive, ">");
                    if (x != NULL)
                        {
                        //StringDisplay = " LOI";
                        flag = 1;
                        break;
                        }
                    else
                        {
                        //StringDisplay = " OK ";
                        flag = 0;
                        break;
                        }
                    }
                // delay(60000);

                if (flag == 1)
                    {
                    /* GET /update?key=<WriteAPIKey>&<fieldx>=<data> : Cap nhat du lieu len thingspeak dung API */
                    ClearStringReceive();
                    SendStringSerially("GET /update?key=");
                    SendStringSerially(WRITE_API_KEY);
                    SendByteSerially('&');
                    SendStringSerially(FIELD);
                    SendByteSerially('=');
                    //SendStringSerially("GET /update?key=WriteAPIKey&field1=");
                    SendByteSerially((adc_lm35 / 100) + 48);
                    SendByteSerially(((adc_lm35 % 100) / 10) + 48);
                    SendByteSerially((adc_lm35 % 10) + 48);
                    SendCommandAT("");
                    //                    SendByteSerially(0x0d); //\r
                    //                    SendByteSerially(0x0a); //\n
                    //delay(40000);

                    /* AT+CIPCLOSE=<link ID> : ngat ket noi TCP/UDP */
                    delay(1000);
                    ClearStringReceive();
                    LED_3 = 1;
                    //delay(3000);
                    SendCommandAT("AT+CIPCLOSE"); //dong channel
                    delay(1000);
                    }
                }
            //delay(1000);
            sec = 0;
            }
        }
    }
