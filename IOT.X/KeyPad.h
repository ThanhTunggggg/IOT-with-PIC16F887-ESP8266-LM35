/* 
 * File:   KeyPad.h
 * Author: TungHT
 * Comments:
 * Revision history: May 7, 2017, 3:58 PM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef KEYPAD_H
#define	KEYPAD_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// CONFIG1

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#define _XTAL_FREQ              8000000
#define ROWA                    RB0
#define ROWB                    RB1
#define ROWC                    RB2
#define ROWD                    RB3
#define COL1                    RB4
#define COL2                    RB5
#define COL3                    RB6
#define COL4                    RB7
#define ROW_TRIS(x)             (TRISB0=TRISB1=TRISB2=TRISB3=x)
#define COL_TRIS(x)             (TRISB4=TRISB5=TRISB6=TRISB7=x)

    /* Functions */
    void InitKeyPad(void); //ham khi dong Keypad
    char GetKey(void);   //ham lay ki tu
    char ScanKey(void); //Ham quet keypad


    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* KEYPAD_H */


