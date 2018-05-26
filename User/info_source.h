
#ifndef _INFOSOURCE_H_
#define _INFOSOURCE_H_

#include "sys_config.h"

#define BT_Sync_Time_CMD            0x01
#define BT_BeaconID_Notice_CMD      0x02
#define BT_BeaconID_Dispatch_CMD    0x03
#define BT_BeaconInfo_Upload_CMD    0x04
#define BT_Beacon_CheckIn_CMD       0x05
#define BT_Beacon_CheckOut_CMD      0x06
#define BT_Beacon_CheckOutAll_CMD   0x07


#define BT_CONFIG_CMD       0x20
#define BT_COORDINATE_CMD   0x21


#define Upload_Coordinate  0x11
#define RF_Beacon_CheckIn  0x12

typedef enum _Watch_Type
{
    WATCH_15,
    WATCH_WA,
    WATCH_X,
    Beacon,
    WATCH_MAX   
    
}Watch_Type;

#pragma pack(show)
#pragma pack(1)

typedef struct _Watch_WaNew
{
     u8 Header;
	 u16 ID;
	 u32 longitude;
	 u32 latitude;
	 u8  HeartRatio;
	 u16 Crc16; 
	
}Watch_WaNew;


typedef struct _Watch_15
{
     u16 Header;
	 u8 Length;
	 u8 Mask[9];
	 u8 Random[2];
	 u8 CMD;
	 u8 SensorType;
	 u32 longitude;
	 u32 latitude;
	 u8  HeartRatio;
	 u16 CRC16; 
	
}Watch_15;

typedef struct _Watch_WangAn
{
     u8 Header;
	 u32 ID;
	 u32 longitude;
	 u32 latitude;
	 u8  HeartRatio;
	 u8  Tail; 
	
}Watch_WangAn;

typedef struct _Watch_WangAnResp
{
     u8 Header;
	 u32 ID;
	 u8  res;
	 u8 Tail; 
	
}Watch_WangAnResp;

typedef struct _Watch_Info_TX
{
     u8  Preamble;
	 u8  ID[9];
	 u32 longitude;//经度
	 u32 latitude;//纬度
	 s8  Snr;
	 s16 Rssi; 
     u16 RxCnt; //本ID信息接收计数
	 u16 TotalRxCnt;//总接收计数
     u8  cc;
     u8  Tail;
	
}Watch_Info_TX;

typedef struct _Watch_Info_Data
{
	 u8  Head[3];
     u8  ID[9];	 
	 u32 longitude;//经度
	 u32 latitude;//纬度
	 s8  Snr;
	 s16 Rssi; 
     u16 RxCnt; //本ID信息接收计数
	 u16 TotalRxCnt;//总接收计数
     u32 timestamp;
     u8  cc;
    
}Watch_Info_Data;


typedef struct _DataStorage_Info
{
    
    u8   FlashVer[4];
  //u16  TotalRxCnt;//总接收计数
    
}DataStorage_Info;

/*----------------------------------------------------------------------------------------------------------------------*/
#pragma pack(show)
#pragma pack(4)
typedef struct _BeaconID
{   
    u32 ID[3];
    u8  valid;
  
}BeaconID;

typedef struct _BTHeader
{
    u8  Preamble;
    u8  len;
    u8  cmd; 
    u8  ver;
   
}BTHeader;

typedef struct _RealTime
{
    
    BTHeader head;
	u32 timestamp;
    u8 cc;
	u8 Tail;
  
}RealTime;

typedef struct _BeaconID_Notice
{
    
    BTHeader head;
	u8 ID_Num;
    u8 cc;
	u8 Tail;
  
}BeaconID_Notice;


typedef struct _BeaconID_Dispatch
{
    
    BTHeader head;
    u8 CurrentID_Num;
	u8 TotalID_Num;
    u32 id[3];
    u8 cc;
	u8 Tail;
  
}BeaconID_Dispatch;

typedef struct _BeaconID_DispatchResp
{
    
    BTHeader head;
    u8 CurrentID_Num;
	u8 TotalID_Num;
    u8 cc;
	u8 Tail;
  
}BeaconID_DispatchResp;

typedef struct _CheckInResp
{
    
    BTHeader head;
	u32 id[3];
    u8 status;
    u8  cc;
	u8 Tail;
    
}CheckInResp;


typedef struct _BTResp
{
    
    BTHeader head;
    u8 status;
    u8  cc;
	u8 Tail;
    
}BTResp;

typedef struct _BT_Config
{
    
    BTHeader head;
	
    u8 minute;
    u8 sec;
    u8 times;
    u8 cc;
	u8 Tail;
  
}BT_Config;

typedef struct _BT_CoordinateSend
{
    
    BTHeader head;
	
    u32 longitude;//经度
	u32 latitude;//纬度
    u8 cc;
	u8 Tail;
  
}BT_CoordinateSend;



typedef struct _BeaconCoordinate
{
    
    BTHeader head;
	u32 id[3];
    u32 longitude;//经度
	u32 latitude;//纬度 
    u16 voltage; //电压   
    s16 Rssi; //信号强度
	s8  Snr; //信噪比
    u8  HeartRatio;//心率
	u8  cc;
	u8  tail;
    
}BeaconCoordinate;

typedef struct _ConcentratorRequst
{
    
    BTHeader head;
	u32 id[3];
    u8  cc;
	u8  tail;
    
}ConcentratorRequst;



typedef struct _CheckIn //入网请求
{
    
    BTHeader head;
 
	u32 id[3];
    u16 voltage; //电压   
    u8  cc;
	u8 tail;
    
}CheckIn;

typedef struct _CheckOut //退网请求
{
    
    BTHeader head;
	u32 id[3];
    u8  cc;
	u8 Tail;
    
}CheckOut;

typedef struct _CheckOutAll //退网请求
{
    
    BTHeader head;
    u8  cc;
	u8 Tail;
    
}CheckOutAll;

typedef struct _CheckOutResp //退网请求应答
{
    
    BTHeader head;
	u32 id[3];
    u8  status;
    u8  cc;
	u8 Tail;
    
}CheckOutResp;

#endif

