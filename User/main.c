
#include <stdlib.h>
#include "time.h"
#include "sys_config.h"
#include "sx12xxEiger.h"
#include "18B20.h"
#include "bsp_spi_flash.h"
#include "info_source.h"
#include "OLED_I2C.h"
#include "bsp_timer.h"
#include "uartPort.h"
#include "sx1276-LoRa.h"
//#include "codetab.h"
#include "flashmgr.h"
#include "menu.h"
#include "device.h"

#define BUFFER_SIZE     128                          // Define the payload size here
#define SAMP_COUNT     20
#define BeaconNumMax    20


u32 E=1163789050;
u32 N=400279950;
u32 Unix_Time=0;

u8 FW_VER[4]={1,1,0,0};	

BeaconID BeaconIdMgr[BeaconNumMax];

extern u8 Battery0[];
extern u8 Battery1[];
extern u8 Battery2[];
extern u8 Battery3[];
extern u8 Battery4[];
extern u8 BatteryC[];
extern u8 BT_BMP[];
extern u8 Wireless_BMP[];
extern u8 WirelessR_BMP[];

extern u8 UNSIG_BMP[];

static uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
static uint8_t  Buffer[BUFFER_SIZE];				// RF buffer
static uint8_t EnableMaster = true; 				// Master/Slave selection

tRadioDriver *Radio = NULL;
uint16_t g_usAdcValue;	/* ADC 采样值的平均值 */



uint8_t MY_TEST_Msg[] = "SX1278_TEST!!!";

u32 HW_ID[3];//cpu id
u32 SecTick=0;
u16 HistoryRXCnt=0,FlashErrCnt=0;
u8  Flash_Full_Flag=0;
u16 RX_cnt=0,RX_cntT=0,seqno0=0,seqno1=0,seqno2=0,seqno3=0,seq_index;
u16 BeaconRX1=0;
u8  BeaconID_Cnt=0;
u8  BeaconNum=0;

u16 usValue,Voltage;
u16 year;
u8  month,day,hour,minute,sec;
u8  Buzz_Flag=0;
u8  BT_Flag=1;
u8  CHRG_Flag=0;
u8  ScreenTest_Flag=0;
u8  BT_RXCPLT=0;
u8  ReadyToGetID=0;
u8  POLL_Flag=0;
u8  SOS_Flag=0;

u8  BeaconPollCnt=0;

Watch_Info_Data watch_info_data[MAX_STORAGE_NUM];
u8 WatchWA_Resp[7]={0xAA,0,0,0,0,1,0xAA};

u16 BC_ID;
u16 BroadcastPeriod=10;//单位秒
u8  BroadcastTime=3;//一次连续发送次数
u8  BC_Cnt=0;
u32 BC_LO,BC_LA;



Watch_WaNew BC2Watch;


void OnMaster( void );
void OnSlave( void );
void PrintfLogo(void);
void DispLogo(void);
void Get_SerialNum(unsigned int* SerialID);
u16 GetADC(void);

void SendToBT(u8*ID,u32 longitude,u32 latitude,s8 SNR,double RSSI,u16 cnt,u16 Totalcnt,Watch_Type type);
void Wireless_Info_Review(u32 seq_index);
void PositionData_Check(void);
void Data_Analyse(u8* databuf,u16 cnt);
void BT_Rx_Process(void);
void update_time(u32 clock);
u16  CRC16_Calc( u8 *arr_buff, u16 len);

int main(void)
{
	 
    u8 i;
    extern  int8_t RxPacketSnrEstimate;
    extern float RxPacketRssiValue;
    extern char Tsymbol;
    extern u8 Tem_int;
    extern u16 Tem_dec;
    extern int a[9];
    extern int temperaure;   
    extern u8 button_state[3];
    extern u8 WaitTX_Flag;

    u8 buf[50]={0};

    u8 header[3]={0xAA,0x0F,1};
    u8 tail[2]={0,0x16};

   
    u8 cc=0;
    u8 sx1276_state=0;
    u8 CntS=0;
    
    Watch_15 *dptr=(Watch_15 *)Buffer;
    
    Watch_WaNew *Nwptr=(Watch_WaNew *)Buffer;
   
    Watch_WangAn *Wptr=(Watch_WangAn *)Buffer;
    BeaconCoordinate *Gpstr=(BeaconCoordinate *)Buffer;
    CheckIn *Checkptr=(CheckIn *)Buffer;
    
    uartBufferInitialize();
    BoardInit();
    
    Get_SerialNum(HW_ID);
    PrintfLogo();	
     
    OLED_CLS();//清屏        
    DispLogo();
    
    bsp_DelayMS(2000);
   // OLED_ROLL(4700);

    OLED_CLS();//清屏 
         
  //  PositionData_Check();
    
   // bsp_DelayMS(1000);   
    
    LED_ON;
    
    BUZZ_ON;   
    bsp_DelayMS(200);
    BUZZ_OFF;  
    
    LED_OFF;
    
    gettemp();     

    Radio = RadioDriverInit();   
    Radio->Init();  
    Radio->StartRx();   
   // SX1276LoRaSetRFState( RFLR_STATE_CAD_INIT ); 

    OLED_CLS();//清屏 

    ShowMenu();
    Battery_Mgr(); 
    Gettemp_Start();  
    BT_Init();
	
	while(1)
	{
        
        Button_Detect();
        
        Menu_Process(button_state);
              
		BT_Rx_Process();
                                                        
        if(TickCounter>1000)
        {		    
          
            Gettemp_OK();   
            CntS++;   
            
            if(CntS%5==0)
            {             
//                if(SOS_Flag==1)
//                {
//                                    
//                    Watch_WangAn watest;
//                    watest.Header=0xAA;
//                    watest.Tail=0xAA;
//                    watest.ID=0x12345678;
//                    watest.latitude=0;
//                    watest.longitude=0;
//                    watest.HeartRatio=100;
//                   
//                    OLED_CLS_B();//清屏 
//                    OLED_ShowStr(0,4,"SOS Send!",2);
//                    printf("SOS SEND");  
//                    Radio->SetTxPacket((u8*)&watest, sizeof(Watch_WangAn)); 
//                   
//                   
//                    
//                    
//                }
                
                
            }
            
            if(CntS % BroadcastPeriod==0)
            {                 
                
                BC_Cnt=0;
                OLED_CLS_B();//清屏 
//                OLED_ShowStr(0,4,"BroadCast Send!",2);
//                printf("BroadCast Send\r\n");  
                 
                BC2Watch.Header = 0xAA;
                BC2Watch.ID = BC_ID;
                BC2Watch.latitude = BC_LA;
                BC2Watch.longitude = BC_LO;
                BC2Watch.HeartRatio=0;
                BC2Watch.Crc16=CRC16_Calc((u8*)&BC2Watch,sizeof(BC2Watch)-2);
                            
               
                Radio->SetTxPacket((u8*)&BC2Watch, sizeof(BC2Watch)); 
                BC_Cnt++;

                
/*
                if(POLL_Flag==1 && BeaconNum!=0)
                {
                   if(BeaconIdMgr[BeaconPollCnt].valid==1)
                   {    
                      Poll_Requst((u8*)BeaconIdMgr[BeaconPollCnt].ID);
                   }
                   
                   BeaconPollCnt++;
                   
                   if(BeaconPollCnt>=BeaconNum)
                       BeaconPollCnt=0; 
                                            
                    LED_ON;
                } 
*/
                
            }                
            
            Gettemp_Start();  

            
            if(!CHRG_Detect() && ScreenTest_Flag==0)
            {
                OLED_DrawBMP(98,0,127,2,BatteryC);
               
                if(BT_Flag==1)
                  OLED_DrawBMP(87,0,95,2,BT_BMP);
                else
                   OLED_ClearBMP(87,0,95,2);                    
                
                if(SX1276LoRaGetRFState()!=RFLR_STATE_IDLE)
                {
                   if(POLL_Flag!=1)
                    OLED_DrawBMP(68,0,84,2,Wireless_BMP); 
                   else
                    OLED_DrawBMP(68,0,84,2,WirelessR_BMP);    
                }
                else
                   OLED_ClearBMP(68,0,84,2);                 
                
                CHRG_Flag=1;
            }
            else if(ScreenTest_Flag==0)
            {   
                         
                Battery_Mgr(); 
               
                if(BT_Flag==1)
                {
                  OLED_DrawBMP(87,0,95,2,BT_BMP);
                }
                else
                   OLED_ClearBMP(87,0,95,2);                    
                
                if(SX1276LoRaGetRFState()!=RFLR_STATE_IDLE)
                {  
                    if(POLL_Flag!=1)
                    OLED_DrawBMP(68,0,84,2,Wireless_BMP); 
                   else
                    OLED_DrawBMP(68,0,84,2,WirelessR_BMP);   
                }
                else
                   OLED_ClearBMP(68,0,84,2);              
            }
            
            TickCounter =0;            
             
        }
        		

		switch( Radio->Process())
		{
		    case RF_RX_DONE:
            {    
                seq_index=RX_cnt; 
                
                if(Buzz_Flag)
                {
                    BUZZ_ON;   
                    bsp_DelayMS(100);
                    BUZZ_OFF;  
                }
                
                Radio->GetRxPacket(Buffer, (uint16_t* )&BufferSize );
				
				
				if((dptr->Header==0x55AA||dptr->Header==0x66BB) && dptr->SensorType==0x60)//15手表__REV
				{
					RX_cnt++;
                    BeaconCoordinate beacon_info;
                    memset((u8*)&beacon_info,0,sizeof(BeaconCoordinate));                    
                    
                    printf("Watch_15: E=%f,N=%f ",(float)(__REV(dptr->longitude))/10000000,(float)(__REV(dptr->latitude))/10000000);
				//	printf("Temperature:%d.%d' ",Tem_int,Tem_dec);	
					printf("SNR:%ddB, RSSI:%0.2fdBm\r\n",RxPacketSnrEstimate,RxPacketRssiValue);
                    seqno0++;                  
                    OLED_CLS_B();//清屏 
                    
                    if(dptr->Header==0x66BB)
                      OLED_ShowStr(0,0,"Relay15:",2);
                    else 
                      OLED_ShowStr(0,0,"Watch15:",2);
                    
                    sprintf(buf,"RxCnt: %d",seqno0);     
                    OLED_ShowStr(0,2,buf,1);   
                    sprintf(buf,"E: %f",(float)(__REV(dptr->longitude))/10000000);
                    OLED_ShowStr(0,3,buf,1);
                    sprintf(buf,"N: %f",(float)(__REV(dptr->latitude))/10000000);
                    OLED_ShowStr(0,4,buf,1);   
                    sprintf(buf,"SNR:%ddB / %0.2fdBm",RxPacketSnrEstimate,RxPacketRssiValue);           
                    OLED_ShowStr(0,5,buf,1); 
                    sprintf(buf,"ID: %08X %08X",*(u32*)dptr->Mask,*(u32*)(dptr->Mask+4));           
                    OLED_ShowStr(0,6,buf,1);

                    beacon_info.head.Preamble=0xFF;
                    beacon_info.tail=0xF0;
                    beacon_info.head.cmd=BT_BeaconInfo_Upload_CMD;   
                    beacon_info.latitude=dptr->latitude;
                    beacon_info.longitude=dptr->longitude;
                    
                    beacon_info.Snr=RxPacketSnrEstimate;
                    beacon_info.Rssi=RxPacketRssiValue;  
                    
                    beacon_info.HeartRatio=dptr->HeartRatio;  //心率
                    
                    beacon_info.head.len=sizeof(BeaconCoordinate)-2;
                    
                    memcpy((u8*)beacon_info.id,(u8*)dptr->Mask,9);                 
                   
                    beacon_info.cc=CalcSum((u8*)&(beacon_info.head.len),beacon_info.head.len-1);                                                                                             
                    uartWriteBuffer(UART_BT,(u8*)&beacon_info,sizeof(BeaconCoordinate));
                    uartSendBufferOut(UART_BT);


           //         SendToBT(dptr->Mask,__REV(dptr->longitude),__REV(dptr->latitude),RxPacketSnrEstimate,RxPacketRssiValue,seqno0,RX_cnt,WATCH_15);                   
                                    
                    
				}	                                  
				else if((Wptr->Header==0xAA && Wptr->Tail==0xAA)||(Wptr->Header==0xBB && Wptr->Tail==0xBB))//网安手表
				{
					Watch_WangAnResp WAresp;
                    BeaconCoordinate beacon_info;
                    memset((u8*)&beacon_info,0,sizeof(BeaconCoordinate));
                    
                    RX_cnt++;
                                        
                    printf("Watch_WA ID:%X,E=%f,N=%f ",Wptr->ID,(float)(Wptr->latitude)/100000,(float)(Wptr->longitude)/100000);
				//	printf("Temperature:%d.%d' ",Tem_int,Tem_dec);	
					printf("SNR:%ddB, RSSI:%0.2fdBm\r\n",RxPacketSnrEstimate,RxPacketRssiValue);
                    seqno1++;
                    OLED_CLS_B();//清屏 
                    if(Wptr->Header==0xBB)
                      OLED_ShowStr(0,0,"RelayWA:",2);
                    else
                      OLED_ShowStr(0,0,"WatchWA:",2);
                    
                    sprintf(buf,"RxCnt: %d",seqno1);     
                    OLED_ShowStr(0,2,buf,1);
                    
                    sprintf(buf,"E: %f",(float)((Wptr->longitude))/100000);
                    OLED_ShowStr(0,3,buf,1);
                    sprintf(buf,"N: %f",(float)((Wptr->latitude))/100000);
                    OLED_ShowStr(0,4,buf,1);
                    
                    sprintf(buf,"SNR:%ddB / %0.2fdBm",RxPacketSnrEstimate,RxPacketRssiValue);           
                    OLED_ShowStr(0,5,buf,1);
                    
                    sprintf(buf,"ID: %08X",Wptr->ID);           
                    OLED_ShowStr(0,6,buf,1);
                    
                    beacon_info.head.Preamble=0xFF;
                    beacon_info.tail=0xF0;
                    beacon_info.head.cmd=BT_BeaconInfo_Upload_CMD;   
                    beacon_info.latitude=(Wptr->latitude)*100;
                    beacon_info.longitude=(Wptr->longitude)*100;
                    
                    beacon_info.Snr=RxPacketSnrEstimate;
                    beacon_info.Rssi=RxPacketRssiValue;
                    
                    beacon_info.HeartRatio=Wptr->HeartRatio;  //心率
                    
                    beacon_info.head.len=sizeof(BeaconCoordinate)-2;
                    
                 
                    beacon_info.id[0]=Wptr->ID;
                   
                    beacon_info.cc=CalcSum((u8*)&(beacon_info.head.len),beacon_info.head.len-1);                    
                                                                          
                     uartWriteBuffer(UART_BT,(u8*)&beacon_info,sizeof(BeaconCoordinate));
                     uartSendBufferOut(UART_BT);
                    
                  //-----------------------------------///对网安手表的应答 
                    WatchWA_Resp[0]=0xAA;
                    WatchWA_Resp[5]=1;
                    WatchWA_Resp[6]=0xAA;
                    
//                    WAresp.Header=0xAA;
//                    WAresp.ID=Wptr->ID;
//                    WAresp.res=1;
//                    WAresp.Tail=0xAA;  
//                    Radio->SetTxPacket((u8*)&WAresp, sizeof(WAresp)); 

                  //   bsp_DelayMS(1000);
                  /*
                     OLED_ShowStr(0,7,"RESP Send!",1);
                     printf("Watch_WA ID:%X,RESP Send!\r\n",Wptr->ID);
                     Radio->SetTxPacket((u8*)WatchWA_Resp, 7); 
                  */
                    
                   
                 //------------------------------------*/  


                    
            //        SendToBT((u8*)&Wptr->ID,Wptr->longitude,Wptr->latitude,RxPacketSnrEstimate,RxPacketRssiValue,seqno1,RX_cnt,WATCH_WA);  
                    
				}
                else if(Nwptr->Header==0xBB)//网安新款手表
				{ 
                    
                    if(Nwptr->Crc16!=CRC16_Calc((u8*)Nwptr,sizeof(Watch_WaNew)-2))
                    {
                        OLED_ShowStr(0,0,"WatchWAx",2);
                        OLED_ShowStr(0,4,"CRC ERR!",2);
                                               
                    }
                    else
                    {
                    
                        BeaconCoordinate beacon_info;
                        memset((u8*)&beacon_info,0,sizeof(BeaconCoordinate));
                        
                        RX_cnt++;
                                            
                        printf("Watch_WAnew ID:%X,E=%f,N=%f ",Nwptr->ID,(float)(Nwptr->latitude)/100000,(float)(Nwptr->longitude)/100000);
                        //printf("Temperature:%d.%d' ",Tem_int,Tem_dec);	
                        printf("SNR:%ddB, RSSI:%0.2fdBm\r\n",RxPacketSnrEstimate,RxPacketRssiValue);
                        seqno3++;
                        OLED_CLS_B();//清屏 
                       
                        OLED_ShowStr(0,0,"WatchWAx",2);
                        
                        sprintf(buf,"RxCnt: %d",seqno3);     
                        OLED_ShowStr(0,2,buf,1);
                        
                        sprintf(buf,"E: %f",(float)((Nwptr->longitude))/100000);
                        OLED_ShowStr(0,3,buf,1);
                        sprintf(buf,"N: %f",(float)((Nwptr->latitude))/100000);
                        OLED_ShowStr(0,4,buf,1);
                        
                        sprintf(buf,"SNR:%ddB / %0.2fdBm",RxPacketSnrEstimate,RxPacketRssiValue);           
                        OLED_ShowStr(0,5,buf,1);
                        
                        sprintf(buf,"ID: %04X",Nwptr->ID);           
                        OLED_ShowStr(0,6,buf,1);
                        
                        beacon_info.head.Preamble=0xFF;
                        beacon_info.tail=0xF0;
                        beacon_info.head.cmd=BT_BeaconInfo_Upload_CMD;   
                        beacon_info.latitude=(Nwptr->latitude)*100;
                        beacon_info.longitude=(Nwptr->longitude)*100;

//                        beacon_info.latitude=(0xB9697A)*100;
//                        beacon_info.longitude=(0x2F6CD7)*100;
                        
                        beacon_info.Snr=RxPacketSnrEstimate;
                        beacon_info.Rssi=RxPacketRssiValue;
                        
                        beacon_info.HeartRatio=Nwptr->HeartRatio;  //心率
                        
                        beacon_info.head.len=sizeof(BeaconCoordinate)-2;                       
                     
                        beacon_info.id[0]=Nwptr->ID;
                       
                        beacon_info.cc=CalcSum((u8*)&(beacon_info.head.len),beacon_info.head.len-1);                    
                                                                              
                        uartWriteBuffer(UART_BT,(u8*)&beacon_info,sizeof(BeaconCoordinate));
                        uartSendBufferOut(UART_BT);
                        
                    }
                     
                } 
                else if(Gpstr->head.Preamble==0xFF || Gpstr->head.Preamble==0xFE)                  
                {                  
                    
                   cc=CalcSum((u8*)&(Gpstr->head.len),sizeof(BeaconCoordinate)-3); 
                    
                    switch(Gpstr->head.cmd)
                    {                                               
                        case Upload_Coordinate:
                        {                                                      
                           if((Gpstr->tail==0xF0 || Gpstr->tail==0xF1) && cc==Gpstr->cc) 
                           {
                               
                                BeaconCoordinate beacon_info;
                                BeaconRX1++;

                                OLED_CLS_B();//清屏 
                                if(Gpstr->tail==0xF1)
                                  OLED_ShowStr(0,0,"RelayB:",2);
                                else
                                  OLED_ShowStr(0,0,"Beacon:",2);
                                
                                sprintf(buf,"RxCnt: %d",BeaconRX1);     
                                OLED_ShowStr(0,2,buf,1);
                                
                                sprintf(buf,"E: %f",(float)((Gpstr->longitude))/10000000);
                                OLED_ShowStr(0,3,buf,1);
                                sprintf(buf,"N: %f",(float)((Gpstr->latitude))/10000000);
                                OLED_ShowStr(0,4,buf,1);
                                
                                sprintf(buf,"SNR:%ddB / %0.2fdBm",RxPacketSnrEstimate,RxPacketRssiValue);           
                                OLED_ShowStr(0,5,buf,1);
                                
                                sprintf(buf,"Voltage:%dmV",Gpstr->voltage);           
                                OLED_ShowStr(0,6,buf,1);
                                
                                sprintf(buf,"ID: %08X %08X",(Gpstr->id[0]),(Gpstr->id[1]));           
                                OLED_ShowStr(0,7,buf,1);
                             
                                memcpy((u8*)&beacon_info,(u8*)Gpstr,sizeof(BeaconCoordinate));
                                
                                beacon_info.head.cmd=BT_BeaconInfo_Upload_CMD;   
                                beacon_info.head.Preamble=0xFF;
                                beacon_info.tail=0xF0;
                                                       
                                beacon_info.Snr=RxPacketSnrEstimate;
                                beacon_info.Rssi=RxPacketRssiValue;
                                
                                beacon_info.id[0]=__REV(beacon_info.id[0]);
                                beacon_info.id[1]=__REV(beacon_info.id[1]);
                                beacon_info.id[2]=__REV(beacon_info.id[2]);
                                beacon_info.cc=CalcSum((u8*)&(beacon_info.head.len),beacon_info.head.len-1);

                                printf("ID: %08X %08X\r\n",(Gpstr->id[0]),(Gpstr->id[1]));
                                printf("RxCnt: %d ",BeaconRX1); 
                                printf("E: %f ",(float)((Gpstr->longitude))/10000000);
                                printf("N: %f ",(float)((Gpstr->latitude))/10000000);
                                printf("SNR:%ddB / %0.2fdBm ",RxPacketSnrEstimate,RxPacketRssiValue);   
                                printf("Voltage:%dmV\r\n",Gpstr->voltage);  
                                                                                          
                                uartWriteBuffer(UART_BT,(u8*)&beacon_info,sizeof(BeaconCoordinate));
                                uartSendBufferOut(UART_BT);
                                
                               
                           }
                           else
                           {
                               OLED_CLS_B();//清屏 
                               OLED_ShowStr(0,4,"RF Data CC err!",2);
                           }
                        }
                        break;
                        case RF_Beacon_CheckIn:
                        {
                          
                            if(Checkptr->tail==0xF0 && cc==Checkptr->cc)
                            {
                                BeaconID_Cnt++;
                                OLED_CLS_B();//清屏 
                                OLED_ShowStr(0,0,"Beacon:",2);
                                sprintf(buf,"ID: %08X %08X %08X",(Checkptr->id[0]),(Checkptr->id[1]),(Checkptr->id[2]));           
                                OLED_ShowStr(0,2,buf,1);
                                sprintf(buf,"Voltage: %dmV",Checkptr->voltage);           
                                OLED_ShowStr(0,4,buf,1);                               
                                sprintf(buf,"Beacon CheckIn %d cnt",BeaconID_Cnt);
                                OLED_ShowStr(0,5,buf,1); 
                                
                                
                                                              
                                memcpy((u8*)BeaconIdMgr[BeaconNum].ID,(u8*)Checkptr->id,12);
                                BeaconIdMgr[BeaconNum].valid=1;
                                BeaconNum++;
                                
                            }
                            else
                            {
                                OLED_CLS_B();//清屏 
                                OLED_ShowStr(0,4,"RF Data CC err!",2);
                            }
                                                                                                            
                        }
                        break;
                        
                    }                   
                   
                }
                else if(!memcmp(Buffer,"SX1278_TEST",11))
                {
                    
                    seqno2++;
                    
                    OLED_CLS_B();//清屏 116.378905,40.027995
            
                    OLED_ShowStr(0,0,"WatchX:",2);                 
                    sprintf(buf,"RxCnt: %d",seqno2); 
                    OLED_ShowStr(0,2,buf,2);                       
                    sprintf(buf,"E: %f",(float)E/10000000); 
                    OLED_ShowStr(0,4,buf,2);                       
                    sprintf(buf,"N: %f",(float)N/10000000);
                    OLED_ShowStr(0,6,buf,2);
                                             
                    SendToBT((u8*)&RX_cnt,E,N,RxPacketSnrEstimate,RxPacketRssiValue,seqno2,RX_cnt,WATCH_X);                   
                    E+=1000;
                    N+=1000;
                }
                else
                {
                     // OLED_CLS_B();  UNSIG
                      if(BufferSize==0)
                      {
//                           OLED_ShowStr(32,2,"RF Module",2);
//                           OLED_ShowStr(24,4,"Connect Bad!",2);
                        
                           OLED_ShowStr(0,7,"RFconnect Bad!",1);
                          
                      }
                      else
                      {
                          // OLED_ShowStr(0,7,"unknow signal!",1);   
                           OLED_DrawBMP(51,0,67,2,UNSIG_BMP); 
                          UART1_Send_Buf(Buffer,BufferSize);
                      }                   
                      
                     // UART1_Send_Buf(Buffer,BufferSize);
                      
                }
            				                           
				memset(Buffer,0,BUFFER_SIZE);
			
			  //  Radio->StartRx();
            
		    }
            break;
     
         case RF_CHANNEL_EMPTY:
         {
             if(WaitTX_Flag==1)
             {
                 WaitTX_Flag=0;
                 SX1276LoRaSetRFState( RFLR_STATE_TX_INIT ); 
                 OLED_ShowStr(0,4,"BroadCast Send!",2);
                 printf("BroadCast Send %d\r\n",BC_Cnt);  
             }
             else
             {
                 SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );  
             }
             
         }
         break;  
         
         case RF_TX_DONE:
		 {
           
             LED_OFF;
//             if(SOS_Flag!=1)
//               printf("Watch_WA ID:%X,RESP Send Done!\r\n",Wptr->ID);
             if(BC_Cnt<BroadcastTime)
             {
                 Radio->SetTxPacket((u8*)&BC2Watch, sizeof(BC2Watch)); 
                 BC_Cnt++;
               //  printf("Broadcast:%d\r\n",BC_Cnt);
             }
             else
             {
                Radio->StartRx(); 
                  OLED_CLS_B();
             }                 
                  
		 }
		 break;
         
         default:
           break;
               
         debugCmdHandler();
		 bsp_DelayMS(1);
	
      }
  }
    
}


void Get_SerialNum(unsigned int* SerialID)
{
    SerialID[0] = *(unsigned int*)(0x1FFFF7E8);
    SerialID[2] = *(unsigned int*)(0x1FFFF7F0);	
    SerialID[1] = *(unsigned int*)(0x1FFFF7EC);
    
    BC_ID=SerialID[0]+SerialID[1]+SerialID[2];
    
   // SerialID[3] = 0; //预留
}

void PrintfLogo(void)
{	
	
	
	printf("**************************************************\r\n");
	printf("* Release Date : %s %s\r\n", __DATE__,__TIME__);	
	printf("* FW Rev: %d.%d.%d.%d\r\n", FW_VER[0],FW_VER[1],FW_VER[2],FW_VER[3]);	
	printf("* HW ID: %08X %08X %08X\r\n",(HW_ID[0]),(HW_ID[1]),(HW_ID[2]));	
	printf("**************************************************\r\n");
}
void DispLogo(void)
{
    u8 buf[30]={0};
    
//   for(u8 i=0;i<6;i++)
//       OLED_ShowCN(16+i*16,0,i);//测试显示中文
   
    OLED_ShowStr(36,0,"S.A.R.T",2);
    
    OLED_ShowStr(0,3,"*********************",1);
    sprintf(buf,"* FW Rev: %d.%d.%d.%d", FW_VER[0],FW_VER[1],FW_VER[2],FW_VER[3]);	
    OLED_ShowStr(0,4,buf,1);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"* ReDate: %s", __DATE__);
    OLED_ShowStr(0,5,buf,1);

    OLED_ShowStr(0,6,"*********************",1);
	
}

void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

    /* 使能 ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);

	/* 配置PC4为模拟输入(ADC Channel9) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 配置ADC1, 不用DMA, 用软件触发 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 配置ADC1 规则通道14 channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);

	/* 使能ADC1 DMA功能 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 软件启动ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: AdcPro
*	功能说明: ADC采样处理，插入1ms systick中断进行调用
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AdcPro(void)
{
	static uint16_t buf[SAMP_COUNT];
	static u16 write;
	uint32_t sum;//,max=0,min=0xFFFFFFFF;
	u16 i;
    

	buf[write] = ADC_GetConversionValue(ADC1);
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* 下面这段代码采用求平均值的方法进行滤波
		也可以改善下，选择去掉最大和最下2个值，使数据更加精确
	*/
	 sum = 0; 
//    for (i = 0; i < SAMP_COUNT; i++)
//    {
//         if(buf[i]>max)
//            max=buf[i];
//    }
//    
//    for (i = 0; i < SAMP_COUNT; i++)
//    {
//          if(buf[i]<min)
//            min=buf[i];
//    }
    
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf[i];            
	}
//    sum-=max;
//    sum-=min;
    
	g_usAdcValue = sum / (SAMP_COUNT);	/* ADC采样值由若干次采样值平均 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* 软件启动下次ADC转换 */
    
}

/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/


u16 GetADC(void)
{
	uint16_t ret;

	/* 因为	g_AdcValue 变量在systick中断中改写，为了避免主程序读变量时被中断程序打乱导致数据错误，因此需要
	关闭中断进行保护 */

	/* 进行临界区保护，关闭中断 */
	__set_PRIMASK(1);  /* 关中断 */

	ret = g_usAdcValue;

	__set_PRIMASK(0);  /* 开中断 */

	return ret;
    
}

//void SendToAPP(BeaconCoordinate *dptr)
//{
//    
//}

void SendToBT(u8*ID,u32 longitude,u32 latitude,s8 SNR,double RSSI,u16 cnt,u16 Totalcnt,Watch_Type type)
{
   
    Watch_Info_TX  watch_info;
    FlashSectionStruct *pFlash,*pFlash1;
    BeaconCoordinate beacon_info;
    
    u8 DataStorageFlag='X';
    
    u8 i,res,cc=0;
    u8 *buf=(u8*)&watch_info;
    u8 *databuf;
    
    watch_info.Preamble=0xFF;
    
    if(type==WATCH_15)      
    {
        memcpy(watch_info.ID,ID,9);
    }   
    else if(type==WATCH_WA)      
    {
        memcpy(watch_info.ID,ID,4);
        for(i=4;i<9;i++)
          watch_info.ID[i]=0x30;
    }
    else
    {
         for(i=0;i<9;i++)
          watch_info.ID[i]=0x00+i;
    }
    
    watch_info.longitude=longitude;                
    watch_info.latitude=latitude;
	watch_info.Rssi=(s16)RSSI;
 //   sprintf(watch_info.Rssi,"%0.1f",RSSI);   
    watch_info.Snr=SNR;
    watch_info.RxCnt=cnt;
    watch_info.TotalRxCnt=Totalcnt;
    
    for(i=0;i<sizeof(watch_info)-3;i++)//去掉协议头，尾和校验位
       cc^=*(buf+1+i);

    watch_info.cc=cc;
    watch_info.Tail=0xF0;
    
    
    uartWriteBuffer(UART_BT,buf,sizeof(watch_info));
    uartSendBufferOut(UART_BT);
    
    uartWriteBuffer(UART_DEBUG,buf,sizeof(watch_info));
    uartSendBufferOut(UART_DEBUG);
    
  
    memcpy(watch_info_data[cnt%MAX_STORAGE_NUM].Head,"FQH",3);
    memcpy(watch_info_data[cnt%MAX_STORAGE_NUM].ID, watch_info.ID,9);
    watch_info_data[cnt%MAX_STORAGE_NUM].longitude=longitude;
    watch_info_data[cnt%MAX_STORAGE_NUM].latitude=latitude;
    watch_info_data[cnt%MAX_STORAGE_NUM].Snr=SNR;
	watch_info_data[cnt%MAX_STORAGE_NUM].Rssi=(s16)RSSI;
  //  sprintf(watch_info_data[cnt%MAX_STORAGE_NUM].Rssi,"%0.1f",RSSI);  
    watch_info_data[cnt%MAX_STORAGE_NUM].RxCnt=cnt;     
    watch_info_data[cnt%MAX_STORAGE_NUM].TotalRxCnt=Totalcnt;  
	watch_info_data[cnt%MAX_STORAGE_NUM].timestamp=Unix_Time;

    cc=0;
    databuf=(u8*)&watch_info_data[cnt%MAX_STORAGE_NUM];
    for(i=0;i<sizeof(Watch_Info_Data)-1;i++)//去掉校验位
       cc^=*(databuf+i);
       
    watch_info_data[cnt%MAX_STORAGE_NUM].cc=cc;
    
    if(Flash_Full_Flag==0)
    {    
        pFlash=uf_RetrieveFlashSectionInfo(FLASH_SECTION_POSITION_DATA);
        pFlash1=uf_RetrieveFlashSectionInfo(FLASH_SECTION_DATA_INDEX);
        
        res=uf_WriteBuffer(FLASH_EX,(u8*)&watch_info_data[cnt%MAX_STORAGE_NUM],pFlash->wptr,sizeof(Watch_Info_Data));
        
        if(!res)
        { 
            bsp_DelayMS(200);        
            res=uf_WriteBuffer(FLASH_EX,(u8*)&watch_info_data[cnt%MAX_STORAGE_NUM],pFlash->wptr,sizeof(Watch_Info_Data));
            if(!res)
            {
                BUZZ_ON;    
                bsp_DelayMS(200);
                BUZZ_OFF;  
                FlashErrCnt++;
            }
            
        }
        else
        {
            pFlash->wptr+=sizeof(Watch_Info_Data);
            res=uf_WriteBuffer(FLASH_EX, &DataStorageFlag, pFlash1->wptr,sizeof(DataStorageFlag));
            if(!res)
            {
                   bsp_DelayMS(200);     
                   res=uf_WriteBuffer(FLASH_EX, &DataStorageFlag, pFlash1->wptr,sizeof(DataStorageFlag));
                    if(!res)
                    {
                        BUZZ_ON;    
                        bsp_DelayMS(1000);
                        BUZZ_OFF;  
                        FlashErrCnt++;
                    }
            }                           
            else
            {
                 pFlash1->wptr++;
                if(pFlash1->wptr==4096+pFlash1->base)
                {
                      OLED_ShowStr(0,4,"Flash Storage Full!",2); 
                      Flash_Full_Flag=1;
                    
                }    
            }
            
            
        }
    }
     
    
}   



void Wireless_Info_Review(u32 seq_index)
{
    u8 buf[128]={0};
    u8 j=0;   
	struct tm *t;
    u32 timestick;
	
	
	timestick=watch_info_data[seq_index%MAX_STORAGE_NUM].timestamp;
	t = localtime((time_t *)&timestick);
		
	sprintf(buf," %d-%02d-%02d",t->tm_year+REFERENCE_YEAR_BASE,t->tm_mon+1,t->tm_mday);	
	OLED_ShowStr(0,0,buf,1);
	
	sprintf(buf,"  %02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);	
	OLED_ShowStr(0,1,buf,1);
	
    memset(buf,0,sizeof(buf));  

    for(u8 i=0;i<9;i++) 
    {    
      sprintf(buf+j,"%02X",watch_info_data[seq_index%MAX_STORAGE_NUM].ID[i]);
      j+=2;   
    }
    OLED_ShowStr(0,2,"ID:",1);
    OLED_ShowStr(18,2,buf,1);
                             
    sprintf(buf,"RxCnt: %d",watch_info_data[seq_index%MAX_STORAGE_NUM].RxCnt);     
    OLED_ShowStr(0,3,buf,1);    

    sprintf(buf,"SNR: %ddB",watch_info_data[seq_index%MAX_STORAGE_NUM].Snr);                            
    OLED_ShowStr(0,4,buf,1);  

       
  
	
	sprintf(buf,"RSSI:%ddBm",watch_info_data[seq_index%MAX_STORAGE_NUM].Rssi);
    //memcpy(buf,watch_info_data[seq_index%MAX_STORAGE_NUM].Rssi,sizeof(watch_info_data[seq_index%MAX_STORAGE_NUM].Rssi));	
    //strcat(buf,"dBm");
    OLED_ShowStr(0,5,buf,1);
    memset(buf,0,sizeof(buf));

    sprintf(buf,"E: %f",(float)watch_info_data[seq_index%MAX_STORAGE_NUM].longitude/10000000);
    OLED_ShowStr(0,6,buf,1);
    sprintf(buf,"N: %f",(float)watch_info_data[seq_index%MAX_STORAGE_NUM].latitude/10000000);            
    OLED_ShowStr(0,7,buf,1);
  
}

void PositionData_Check(void)
{
    FlashSectionStruct *pFlash,*pFlash1,*pFlash2;
    DataStorage_Info datastorage_info;
    u8 Index_check[4096]={0};
    u8 buf[50];
    u32 t0=0;

    pFlash=&flashSectionStruct[FLASH_SECTION_DATA_INFO];
    pFlash1=&flashSectionStruct[FLASH_SECTION_DATA_INDEX];
    pFlash2=&flashSectionStruct[FLASH_SECTION_POSITION_DATA];
    
    uf_ReadBuffer(FLASH_EX,(u8*)&datastorage_info,pFlash->base,sizeof(DataStorage_Info));
    if(memcmp(datastorage_info.FlashVer,FlashVer,4))
    {     
        t0=TickCounter;
        OLED_ShowStr(0,2,"Format Flash...",2);
        uf_EraseChip(FLASH_EX);
        printf("Erase flash time:%dms.\r\n",TickCounter-t0);
        
        memcpy(datastorage_info.FlashVer,FlashVer,4);
      //  datastorage_info.TotalRxCnt=0;
        uf_WriteBuffer(FLASH_EX,(u8*)&datastorage_info.FlashVer,pFlash->base,sizeof(DataStorage_Info));
        printf("Init flash storage space.\r\n");
        OLED_ShowStr(0,2,"Format Flash OK!",2);
	//	OLED_ShowStr(25,4,"Storage Space.",2);
        
    }
    else
    {
        uf_ReadBuffer(FLASH_EX,(u8*)&Index_check,pFlash1->base,sizeof(Index_check));
        
        for(u16 i=0;i<sizeof(Index_check);i++)
        {
            if(Index_check[i]=='X')
                HistoryRXCnt++;
        }
        
        pFlash1->wptr+=HistoryRXCnt;
        pFlash2->wptr+=HistoryRXCnt*sizeof(Watch_Info_Data);
          
        printf("HistoryRXCnt:%d\r\n",HistoryRXCnt);
        
     
//        sprintf(buf,"H_RXCnt:%d",HistoryRXCnt);      
//        OLED_ShowStr(0,4,buf,2); 
        
         if(pFlash1->wptr==4096+pFlash1->base)
         {
              OLED_ShowStr(0,6,"Flash Storage Full!",2); 
             
              Flash_Full_Flag=1;
            
         }       
        
    }
    
}

void Data_Analyse(u8* databuf,u16 cnt)
{
    Watch_Info_Data *WdataPtr;
	struct tm *t;
    u32 timestick;
	
    u8 buf[50]={0};
    u8 j;
    
    for(u16 i=0;i<cnt;i++)
    {
        WdataPtr=(Watch_Info_Data *)(databuf+i*sizeof(Watch_Info_Data));
        printf("***************************\r\nHistoryRXCnt:%d\r\n",i+1);
        printf("ID:");
        for(j=0;j<9;j++)
          printf("%02X",WdataPtr->ID[j]);
        
        printf("\r\n");
        
        printf("E: %f\r\n",(float)(WdataPtr->longitude)/10000000);                  
        printf("N: %f\r\n",(float)(WdataPtr->latitude)/10000000);
        printf("SNR: %ddB\r\n",WdataPtr->Snr); 
                
        printf("RSSI:%ddBm\r\n",WdataPtr->Rssi);
//        for(j=0;j<6;j++)
//        {
//            if(WdataPtr->Rssi[j]!=0)
//              printf("%c",WdataPtr->Rssi[j]);
//            else
//              break;
//            
//        }
//             
//        printf("dBm\r\n");
        printf("RxCnt:%d\r\n",WdataPtr->RxCnt);       
        printf("TotalRxCnt:%d\r\n",WdataPtr->TotalRxCnt);
		
		timestick=WdataPtr->timestamp;
		t = localtime((time_t *)&timestick);		
		printf("Time:%d-%02d-%02d %02d:%02d:%02d\r\n",t->tm_year+REFERENCE_YEAR_BASE,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);		
        printf("***************************\r\n");
        
        
    }
    
    
}

void BT_Resp(u8 cmd,u8 res)
{
	 BTResp resp;
    
     resp.head.Preamble=0xFF;
     resp.head.cmd=cmd;
     resp.head.len=sizeof(BTResp)-2;//去除1字节前导码和1字节len
     resp.status=res;

     resp.cc= CalcSum((u8*)&resp.head.len,sizeof(resp)-3);
     resp.Tail=0xF0;
  
	 uartWriteBuffer(UART_BT,(u8*)&resp,sizeof(BTResp));
	 uartSendBufferOut(UART_BT);
}

void BT_CheckOutResp(u8 *id,u8 res)
{
	 CheckOutResp resp;
    
     resp.head.Preamble=0xFF;
     resp.head.cmd=BT_Beacon_CheckOut_CMD;
     resp.head.len=sizeof(BTResp)-2;//去除1字节前导码和1字节len
     memcpy((u8*)(resp.id),id,12);
     resp.status=res;
  
         
     resp.cc=CalcSum((u8*)&resp.head.len,sizeof(resp)-3);
     resp.Tail=0xF0;
  
	 uartWriteBuffer(UART_BT,(u8*)&resp,sizeof(CheckOutResp));
	 uartSendBufferOut(UART_BT);
}


void BT_BeaconIDResp(u8 CurrentID_Num,u8 TotalID_Num)
{
	 BeaconID_DispatchResp resp;
    
     resp.head.Preamble=0xFF; 
     resp.head.cmd=BT_BeaconID_Dispatch_CMD;
     resp.head.len=sizeof(BeaconID_DispatchResp)-2;//去除1字节前导码和1字节len
    
     resp.CurrentID_Num=CurrentID_Num;
     resp.TotalID_Num=TotalID_Num;

     resp.cc=CalcSum((u8*)&resp.head.len,sizeof(resp)-3);
     resp.Tail=0xF0;
         
	 uartWriteBuffer(UART_BT,(u8*)&resp,sizeof(BeaconID_DispatchResp));
	 uartSendBufferOut(UART_BT);
}

void BT_Rx_Process(void)
{
	u8 dlen,cc;   
	u8 buf[50]={0};
    u8 Lbuf[50]={0};
    BTHeader *head;
    BeaconID_Notice *notice;
       	
	if(BT_RXCPLT==1)
	{
		BT_RXCPLT=0;
        dlen=uartGetAvailBufferedDataNum(UART_BT);
        uartRead(UART_BT,buf,dlen);
		head=(BTHeader *)buf;
		if(head->Preamble==0xFF)
		{
		  cc=CalcSum((u8*)&(head->len),head->len-1);
            
          if(cc!=buf[head->len])
          {
               OLED_CLS_B();//清屏  
               OLED_ShowStr(0,4,"BT Data CC err!",2); 
               return;
          }
          
          switch(head->cmd)
          {
                 
              case BT_CONFIG_CMD:
              {
                  BT_Config *btconfig=(BT_Config *)buf;
                  
                  if(btconfig->Tail==0xF0)
                  {                     
                      BroadcastPeriod=btconfig->minute*60;
                      BroadcastPeriod+=btconfig->sec;
                      BroadcastTime=btconfig->times;                     
                      BT_Resp(BT_CONFIG_CMD,0);
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
              
              
               case BT_COORDINATE_CMD:
              {
                  BT_CoordinateSend *btcoordinate=(BT_CoordinateSend *)buf;
                  
                  if(btcoordinate->Tail==0xF0)
                  {
                      BC_LO=btcoordinate->longitude;
                      BC_LA=btcoordinate->latitude;
                      
                      BT_Resp(BT_COORDINATE_CMD,0);
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
              
                           
              
              case BT_Sync_Time_CMD:
              {
                  RealTime *time=(RealTime *)buf;
                  
                  if(time->Tail==0xF0)
                  {
                      Unix_Time=__REV(time->timestamp)+LOCAL_TIME_ZONE_SECOND_OFFSET;
                      BT_Resp(BT_Sync_Time_CMD,0);
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
              
              case BT_BeaconID_Notice_CMD:
              {
                  notice=(BeaconID_Notice *)buf;
                  if(notice->Tail==0xF0)
                  {
                       OLED_CLS_B();//清屏  
                      if(notice->ID_Num<=BeaconNumMax && BeaconNum<=BeaconNumMax)
                     {                  

                        BT_Resp(BT_BeaconID_Notice_CMD,0);
                        ReadyToGetID=1; 
                        OLED_ShowStr(0,3,"Get BeaconID_Notice!",1);
                     }
                     else
                     {
                         BT_Resp(BT_BeaconID_Notice_CMD,1);
                         printf("Total beacon num can't >20!\r\n");
                     }
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
              
              case BT_BeaconID_Dispatch_CMD:
              {
                  BeaconID_Dispatch *getid=(BeaconID_Dispatch *)buf;
                  if(getid->Tail==0xF0)
                  {
                     if(ReadyToGetID==1)
                     {  
                       // memcpy((u32*)BeaconIdMgr[BeaconNum].ID,(u32*)getid->id,3); 
                         
                        BeaconIdMgr[BeaconNum].ID[0]=__REV(getid->id[0]);
                        BeaconIdMgr[BeaconNum].ID[1]=__REV(getid->id[1]);
                        BeaconIdMgr[BeaconNum].ID[2]=__REV(getid->id[2]);
                         
                        BeaconIdMgr[BeaconNum].valid=1;
                         
                        BeaconNum++;   
                                     
                        OLED_CLS_B();//清屏  
                         
                        sprintf(Lbuf,"BeaconNum: %d",BeaconNum);           
                        OLED_ShowStr(0,5,Lbuf,1);
                         
                        sprintf(Lbuf,"ID: %08X %08X %08X",__REV(getid->id[0]),__REV(getid->id[1]),__REV(getid->id[2]));           
                        OLED_ShowStr(0,6,Lbuf,1);
                         
                        if(BeaconNum>=BeaconNumMax) 
                          ReadyToGetID=0;  
                        
                        BT_BeaconIDResp(getid->CurrentID_Num,getid->TotalID_Num);
                     }
                     else
                     {
                         BT_Resp(BT_BeaconID_Notice_CMD,1);//未准备好？？
                         printf("Total beacon num can't >100!\r\n");
                     }
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
              
             
              case BT_Beacon_CheckOut_CMD:
              {
                  CheckOut *checkout=(CheckOut *)buf;
                  u8 i;
                  if(checkout->Tail==0xF0)
                  {
                     for(i=0;i<BeaconNumMax;i++)
                     {
                         if(!memcmp((u8*)BeaconIdMgr[i].ID,(u8*)checkout->id,12))
                         {
                              BeaconIdMgr[i].valid=0;
                              BT_CheckOutResp((u8*)checkout->id,0);    
                              break;
                         }
                         
                     }
                     
                     if(i==BeaconNumMax) //未查询到此ID
                        BT_CheckOutResp((u8*)checkout->id,1);                                                              
                    
                  }
                  else
                     printf("tail err!\r\n"); 
                  
              }
              break;
              
              case BT_Beacon_CheckOutAll_CMD:
              {
                  CheckOutAll *checkoutall=(CheckOutAll *)buf;
                  if(checkoutall->Tail==0xF0)
                  {
                     for(u8 i=0;i<BeaconNumMax;i++)
                     {
                         BeaconIdMgr[i].valid=0;
                         
                     }
                     
                      BT_Resp(BT_Beacon_CheckOutAll_CMD,0);
                                       
                  }
                  else
                     printf("tail err!\r\n");  
              }
              break;
          }
          
          
		}
		
	}
	
}

void update_time(u32 clock)
{
	struct tm *t;
    u32 timestick;
    timestick=clock;
    t = localtime((time_t *)&timestick);
	
	year=t->tm_year+REFERENCE_YEAR_BASE;
    month=t->tm_mon+1;
    day=t->tm_mday;
    hour=t->tm_hour;
    minute=t->tm_min;
    sec=t->tm_sec;
	
}

u16  CRC16_Calc( u8 *arr_buff, u16 len)
{
 u8 i;
 u16 crc=0,j;
 u16 Polynomial=0xA001;
 for (j=0; j<len;j++)
 {
   crc^=*arr_buff++;
   for ( i=0; i<8; i++)
   {
       if( ( crc&0x0001) >0)
       {
           crc=crc>>1;
           crc^=Polynomial;
        }
      else
          crc=crc>>1;
   }   
 }
  return crc;
}

/*********************************************END OF FILE**********************/
