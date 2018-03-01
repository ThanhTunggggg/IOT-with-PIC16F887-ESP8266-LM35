/* 
 * File:   KeyPad.c
 * Author: TungHT
 * Comments: 
 * Revision history: May 7, 2017, 3:58 PM
 */

#include "KeyPad.h"

const char KeyPadChar[] = {
                           '1', '2', '3', 'A',
                           '4', '5', '6', 'B',
                           '7', '8', '9', 'C',
                           '*', '0', '#', 'D',
                           '?'
    };

void InitKeyPad(void)
    {
    ROW_TRIS(0);
    COL_TRIS(1);
    }

/* Quet keypad */
//char ScanKey()
//    {
//    char key = 0, row;
//    ROWA = 1;
//    ROWB = 1;
//    ROWB = 1;
//    ROWC = 1;
//    for (row = 0b00000001; row != 0b00010000; row <<= 1) //Quet tung hang
//        {
//        ROWA = (row^0x0001) >> 0;
//        ROWB = !(bit)((row & 0x0002) >> 1);
//        ROWC = !(bit)((row & 0x0004) >> 2);
//        ROWD = !(bit)((row & 0x0008) >> 3);
//        __delay_us(100);
//        if (COL1 == 0)
//            {
//            __delay_us(100);
//            if (COL1 == 0)
//                break;
//            }
//        key++;
//        if (COL2 == 0)
//            {
//            __delay_us(100);
//            if (COL2 == 0)
//                break;
//            }
//        key++;
//        if (COL3 == 0)
//            {
//            __delay_us(100);
//            if (COL3 == 0)
//                break;
//            }
//        key++;
//        if (COL4 == 0)
//            {
//            __delay_us(100);
//            if (COL4 == 0)
//                break;
//            }
//        key++;
//        }
//    return KeyPadChar[key];
//    }
/* Quet keypad */
char ScanKey(void)
    {
    ROWA = 0;
    ROWB = 1;
    ROWC = 1;
    ROWD = 1; //Test Row A
    if (COL1 == 0)
        {
        __delay_us(1000);
        if (COL1 == 0)
            return 'D';
        }
    if (COL2 == 0)
        {
        __delay_us(1000);
        if (COL2 == 0)
            return 'C';
        }
    if (COL3 == 0)
        {
        __delay_us(1000);
        if (COL3 == 0)
            return 'B';
        }
    if (COL4 == 0)
        {
        __delay_us(1000);
        if (COL4 == 0)
            return 'A';
        }

    ROWA = 1;
    ROWB = 0;
    ROWC = 1;
    ROWD = 1; //Test Row B
    if (COL1 == 0)
        {
        __delay_us(1000);
        if (COL1 == 0)
            return '#';
        }
    if (COL2 == 0)
        {
        __delay_us(1000);
        if (COL2 == 0)
            return '9';
        }
    if (COL3 == 0)
        {
        __delay_us(1000);
        if (COL3 == 0)
            return '6';
        }
    if (COL4 == 0)
        {
        __delay_us(1000);
        if (COL4 == 0)
            return '3';
        }

    ROWA = 1;
    ROWB = 1;
    ROWC = 0;
    ROWD = 1; //Test Row C
    if (COL1 == 0)
        {
        __delay_us(1000);
        if (COL1 == 0)
            return '0';
        }
    if (COL2 == 0)
        {
        __delay_us(1000);
        if (COL2 == 0)
            return '8';
        }
    if (COL3 == 0)
        {
        __delay_us(1000);
        if (COL3 == 0)
            return '5';
        }
    if (COL4 == 0)
        {
        __delay_us(1000);
        if (COL4 == 0)
            return '2';
        }

    ROWA = 1;
    ROWB = 1;
    ROWC = 1;
    ROWD = 0; //Test Row D
    if (COL1 == 0)
        {
        __delay_us(1000);
        if (COL1 == 0)
            return '*';
        }
    if (COL2 == 0)
        {
        __delay_us(1000);
        if (COL2 == 0)
            return '7';
        }
    if (COL3 == 0)
        {
        __delay_us(1000);
        if (COL3 == 0)
            return '4';
        }
    if (COL4 == 0)
        {
        __delay_us(1000);
        if (COL4 == 0)
            return '1';
        }
    return '?'; // Khong phim nao duoc bam
    }

/* Lay ki tu */
char GetKey(void)
    {
    char key = '?'; // Gia su chua co nut nao duoc bam
    while (key == '?') // Cho cho den khi 1 nut duoc bam
        key = ScanKey(); // Quet keypad

    return key; // Khi co 1 nut duoc bam thi tra ve gia tri nut bam do
    }

