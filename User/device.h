#ifndef _DEVICE_H
#define _DEVICE_H

#include "stm32f10x.h"


enum Button
{
    NEXT=0,
    OK,
    BACK
};


void SOSSwitchON(void);
void SOSSwitchOFF(void);

void POLLSwitchON(void);
void POLLSwitchOFF(void);

void RFSwitchON(void);
void RFSwitchOFF(void);


void BT_Init(void);
void BTSwitchON(void);
void BTSwitchOFF(void);
void DeviceInfoShow(u8 *ButtonState);
void BUZZSwitchON(void);
void BUZZSwitchOFF(void);
void ClearData(u8 *ButtonState);
void ScreenTest(u8 *ButtonState);
u8 CalcSum(u8* ptr,u8 len);

void Poll_Requst(u8* ID);
#endif