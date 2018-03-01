/* 
 * File:   LedMatrix.h
 * Author: TungHT
 * Comments:
 * Revision history: May 7, 2017, 3:58 PM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LEDMATRIX_H
#define	LEDMATRIX_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
   
#define _XTAL_FREQ                  8000000
#define SH                          RD0
#define DS                          RD1
#define ST                          RD2
#define SH_ROW                      RD3
#define DS_ROW                      RD4
#define IC74HC595_PORT              PORTD
#define IC74HC595_TRIS(x)           (TRISD0=TRISD1=TRISD2=TRISD3=TRISD4=x)
    //#define LED                         PORTD
    //#define LED_TRIS                    TRISD
#define LED_MARIX_AMOUNT            4
    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 
    void InitLedMatrix(void); //Ham khoi dong Led matran dung IC74C595
    //void display(unsigned char*); //Ham hien thi xau  ki tu co so ki tu bang LED_MARIX_AMOUNT
    void Ic74595_Write(char, char, char); //Ham dua du lieu vao IC 74HC595
    void display(char*); //Ham hien thi
    signed char count;
    unsigned int next = 0, LenghString; // bien vi tri va do dai chuoi hien thi
    unsigned char i; //Bien chay ham RunString
    unsigned char TempFont, mode = 1; //Du lieu font chu hien thi va che do hien thi
    unsigned char SelFont; //biến dùng để chọn font chữ
    unsigned char ColFont; //biến dùng để chọn cột của font chữ
    char* OldString; //Xau hien thi cu
    //void DisplayTempOnLedMatrix(unsigned char, unsigned char); //Ham hien thi 2 ki tu len LED ma tran

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* LEDMATRIX_H */

