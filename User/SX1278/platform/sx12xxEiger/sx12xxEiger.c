/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx12xxEiger.c
 * \brief        
 *
 * \version    1.0
 * \date       Nov 21 2012
 * \author     Miguel Luis
 */
#include <stdint.h> 
#include "spi.h"
#include "sx12xxEiger.h"
#include "uartPort.h"
#include "bsp_spi_flash.h"
#include "bsp_usart.h"
#include "bsp_timer.h"
#include "sx1276-LoRa.h"
#include "OLED_I2C.h"

extern u8 Battery0[];
extern u8 Battery1[];
extern u8 Battery2[];
extern u8 Battery3[];
extern u8 Battery4[];
extern u8 BatteryC[];
extern u8 BatteryWARN[];

u8 BT_SLEEP[]="AT+SLEEP\r\n";
u8 BT_WAKE[]="AT+WAKE\r\n";
u8 BT_AT[]="AT\r\n";
u8 BT_BAUD[]="AT+BAUD4\r\n"; //baud 9600

u8 BT_STATUS=0;//0:WAKE;1:SLEEP

u8 button_state[3]={0};

// System tick (1ms)
volatile uint32_t TickCounter = 0;

void BoardInit( void )
{
		GPIO_Configuration(); 
    	bsp_InitTimer();
		USART1_Config();
		USART2_Config();
		//	 USART3_Config();

		SpiInit();//lora spi	
	    sf_InitHard();	/* 初始化SPI flash */
        I2C_Configuration();
	    OLED_Init();   
        ADC_Configuration();
	    TIMX_Init(CALC_TYPE_US);  
        NVIC_Configuration();    
}

void Delay (uint32_t delay)
{
    // Wait delay ms
    uint32_t startTick = TickCounter;
    while( ( TickCounter - startTick ) < delay ); 
    
}

void LongDelay (uint8_t delay)
{
    uint32_t longDelay;
    uint32_t startTick;

    longDelay = delay * 1000;

    // Wait delay s
    startTick = TickCounter;
    while( ( TickCounter - startTick ) < longDelay );   
}

void GPIO_Configuration(void)
{ 
    
		GPIO_InitTypeDef GPIO_InitStructure;
		 //禁用JTAG 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 			//激活GPIOB clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 			//激活GPIOB clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 			//激活GPIOB clock
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	

		GPIO_InitStructure.GPIO_Pin = BUZZ_PIN|LED_PIN;  	//设置GPIOA11、设置GPIOA12
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//设置GPIO速度为10MHZ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				  	//设置以上三个GPIO为输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);							  	//将以上设置参数写入

	    GPIO_InitStructure.GPIO_Pin = CHRG_PIN|BUTTON_UP_PIN;  	//设置设置GPIOA15、设置GPIOA8
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//设置GPIO速度为10MHZ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				  	//设置以上三个GPIO为输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);		

	
		GPIO_InitStructure.GPIO_Pin = BUTTON_MID_PIN|BUTTON_DOWN_PIN;  	//设置GPIOB11、GPIOB10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//设置GPIO速度为10MHZ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				  	//设置以上三个GPIO为输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);	

	
		LED_OFF;
		BUZZ_OFF;	 

}

void NVIC_Configuration()
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
	
	
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  // Tax uart port 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  // Tax uart port 1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		

 
}

u8 CHRG_Detect(void)
{
    u8 ret;   
    ret = GPIO_ReadInputDataBit(GPIOA, CHRG_PIN);     
}



void Button_Detect(void)
{
        
    u8 button[3]={0};

 
    for(u8 i=0;i<7;i++)
    {   
         button[0]+= GPIO_ReadInputDataBit( GPIOA, BUTTON_UP_PIN );     
         button[1]+= GPIO_ReadInputDataBit( GPIOB, BUTTON_MID_PIN );
         button[2]+= GPIO_ReadInputDataBit( GPIOB, BUTTON_DOWN_PIN );
        // bsp_DelayMS(10);
    }
    
    for(u8 i=0;i<3;i++)
    {
        if(button[i]<=2)
           button_state[i]=1; 
        else
           button_state[i]=0;  
    }
    
}
    
    
void Button_Menu(u8 *ButtonState)  
{
    extern u32 SecTick;
    extern int temperaure;  
    extern u16 seq_index,RX_cnt;    
    extern u16 Voltage,HistoryRXCnt,FlashErrCnt;
    
    u8 dlen;
    u8 buf[128]={0};
    static u8 sysFlag=1;
    u8 PageNum=4;
    static u8 ButtonStatePre[3]={0};
       
    
    if(ButtonState[0]==1)  
     {
          
        OLED_CLS_B();
        sprintf(buf,"SysInfo:%d-%d",PageNum,sysFlag);
        OLED_ShowStr(0,0,buf,2); 
        memset(buf,0,sizeof(buf));
         
        if(sysFlag==1)
        {
            sysFlag++;
           

            if(BT_STATUS==0)
               OLED_ShowStr(0,4,"BT Status: ON",2); 
            else
               OLED_ShowStr(0,4,"BT Status: OFF",2);  
            
              
              if(SX1276LoRaGetRFState()==RFLR_STATE_IDLE)
              {                    
                  OLED_ShowStr(0,6,"RF Status: OFF",2);  
              }
              else
              {                    
                 OLED_ShowStr(0,6,"RF Status: ON",2);  
              }
                          
        }
        else if(sysFlag==2)
        {
            sprintf(buf,"Temp:%0.2f'",(float)temperaure/10000);
            OLED_ShowStr(0,4,buf,2); 
            sprintf(buf,"Voltage:%dmV",Voltage);             
            OLED_ShowStr(0,6,buf,2);
            sysFlag++;
        } 
        else if(sysFlag==3)
        {
            sprintf(buf,"H_RXCnt:%d",HistoryRXCnt);
            OLED_ShowStr(0,4,buf,2); 
            sprintf(buf,"C_RXCnt:%d",RX_cnt);             
            OLED_ShowStr(0,6,buf,2);
            sysFlag++;
        }
        else
        {
            sprintf(buf,"FlashErrCnt:%d",FlashErrCnt);
            OLED_ShowStr(0,4,buf,2);
            sprintf(buf,"RunTime:%dmin",SecTick/60); 
            OLED_ShowStr(0,6,buf,2); 
            sysFlag=1;            
        }
        
     }
        
        if(ButtonState[1]==1 && ButtonState[2]==0)
        {
            
            OLED_CLS_B();//清屏 
                                
          if(SX1276LoRaGetRFState()==RFLR_STATE_IDLE)
          {    
             SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
             OLED_ShowStr(0,0,"RF ON!",2);  
          }
          else
          {
             SX1276LoRaSetRFState(RFLR_STATE_IDLE);
             OLED_ShowStr(0,0,"RF OFF!",2);  
          }
 
            
        }
        
        if(ButtonState[2]==1 && ButtonState[1]==0)
        {             
            OLED_CLS_B();
            Wireless_Info_Review(seq_index);
            seq_index--;
            if(seq_index==0)
                seq_index=RX_cnt;
           
        }
        
         if(ButtonState[2]==1 && ButtonState[1]==1)
         {
               OLED_CLS_B();//清屏 BT_GETSTATUE
                        
              if(BT_STATUS==0) 
              {
                   
                  
                   uartWriteBuffer(UART_BT,BT_SLEEP,sizeof(BT_SLEEP));
                   uartSendBufferOut(UART_BT);
                  
                 // bsp_DelayMS(500);
                  
//                  dlen=uartGetAvailBufferedDataNum(UART_BT);
//                  if(dlen>=2)  
//                  {                      
//                    uartRead(UART_BT,buf,dlen);
//                    if(buf[dlen-1]=='O' && buf[dlen]=='K')
//                    {
                          OLED_ShowStr(0,0,"BT OFF!",2); 
                           BT_STATUS=1;
                   // }                   
                        
                 // }
                  
              }
              else
              {
                  
                  for(u8 t=0;t<50;t++) 
                  {
                      uartWriteBuffer(UART_BT,BT_WAKE,sizeof(BT_WAKE));
                      uartSendBufferOut(UART_BT);
                  }
                   bsp_DelayMS(500);
               
                  dlen=uartGetAvailBufferedDataNum(UART_BT);
                  if(dlen>=2)  
                  {                      
                    uartRead(UART_BT,buf,dlen);
                    if(buf[dlen-5]=='O' && buf[dlen-4]=='K')
                    {
                          OLED_ShowStr(0,0,"BT ON!",2); 
                          BT_STATUS=0;
                    }
                        
                  }
                  
                  
              }
         }
     
    
}

void Battery_Mgr(void)
{
     s16 usValue;
     static s16 Voltage_temp=0;
     extern u8 CHRG_Flag;
     extern u8 ScreenTest_Flag;
     extern u16 Voltage;
     usValue = GetADC();
     Voltage=((uint32_t)usValue * 3300)*14/4095/11;
    
    if((Voltage-Voltage_temp>=50) || (Voltage_temp-Voltage-Voltage_temp>=50) || CHRG_Flag==1 ||  ScreenTest_Flag==0)
    {    
        CHRG_Flag=0;
        
        if(Voltage>=Battery_4BAR)
            OLED_DrawBMP(98,0,127,2,Battery4);
        else if(Voltage>=Battery_3BAR)
            OLED_DrawBMP(98,0,127,2,Battery3);
        else if(Voltage>=Battery_2BAR)
            OLED_DrawBMP(98,0,127,2,Battery2);
        else if(Voltage>=Battery_1BAR)
            OLED_DrawBMP(98,0,127,2,Battery1);
        else if(Voltage>=Battery_0BAR)
            OLED_DrawBMP(98,0,127,2,Battery0);
        else if(Voltage>=Battery_WARN)
        {
            OLED_DrawBMP(98,0,127,2,BatteryWARN);
          //  OLED_ShowStr(24,4,"Low Power,Please Charge!",2);
        }
        else
        {
            OLED_OFF();
            LED_OFF;
            SX1276LoRaSetRFState(RFLR_STATE_IDLE);
            SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        }
        
        Voltage_temp=Voltage;
    }
    
}

