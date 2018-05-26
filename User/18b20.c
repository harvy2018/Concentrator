

#include "stm32f10x.h"
#include "bsp_timer.h"
#include "18b20.h"


//float temperaure;
unsigned int a1,a2,a3,a4,a5,a6,a7,a8,a9,a10; 
unsigned int conf[10];
unsigned char id[9];
int a[9];
int temperaure;   
char Tsymbol;//�¶ȷ���
u8 Tem_int;
u16 Tem_dec;


//void DQ_CLR()   {GPIO_PORT_DS18B20->BRR&=~(0x0100); GPIO_PORT_DS18B20->BRR|=GPIO_PIN_DS18B20;}   //GPIO_PORT_DS18B20->BRR = GPIO_PIN_DS18B20
//void DQ_SET()   {GPIO_PORT_DS18B20->BSRR&=~(0x0100);GPIO_PORT_DS18B20->BSRR|=GPIO_PIN_DS18B20;}     //GPIO_PORT_DS18B20->BSRR = GPIO_PIN_DS18B20


//void  DQ_OUT()	{ GPIO_PORT_DS18B20->CRH &=~(0x0f);	GPIO_PORT_DS18B20->CRH|=0x03;}  //
//void  DQ_IN()	{ GPIO_PORT_DS18B20->CRH &=~(0x0f);	GPIO_PORT_DS18B20->CRH|=0x04;}




void DQ_CLR()   {GPIO_PORT_DS18B20->BRR&=~(0x0100); GPIO_PORT_DS18B20->BRR|=GPIO_PIN_DS18B20;}   //GPIO_PORT_DS18B20->BRR = GPIO_PIN_DS18B20
void DQ_SET()   {GPIO_PORT_DS18B20->BSRR&=~(0x0100);GPIO_PORT_DS18B20->BSRR|=GPIO_PIN_DS18B20;}     //GPIO_PORT_DS18B20->BSRR = GPIO_PIN_DS18B20


void  DQ_OUT()	{ GPIO_PORT_DS18B20->CRH &=~(0x0f);	GPIO_PORT_DS18B20->CRH|=0x03;}  //
void  DQ_IN()	{ GPIO_PORT_DS18B20->CRH &=~(0x0f);	GPIO_PORT_DS18B20->CRH|=0x04;}

unsigned char GetDQ(void)
{
	unsigned short temp;
	DQ_IN();
	temp = GPIO_PORT_DS18B20->IDR;	 //IDR  
	if((temp&0x0100)==0x0100)	//PB8
	   return 1;
	else
	   return 0;

}


/*
void DQ_SET()  {GPIO_SetBits(GPIO_PORT_DS18B20,GPIO_PIN_DS18B20);  } 
void DQ_CLR()  {GPIO_ResetBits(GPIO_PORT_DS18B20,GPIO_PIN_DS18B20);}

void  DQ_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DS18B20;  	//����DS18B20�ӿ�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//����GPIO�ٶ�Ϊ10MHZ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				  	//������������GPIOΪ���
	GPIO_Init(GPIO_PORT_DS18B20, &GPIO_InitStructure);							  	//���������ò���д��

}

void  DQ_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DS18B20;  	//����DS18B20�ӿ�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//����GPIO�ٶ�Ϊ10MHZ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				  	//������������GPIOΪ���
	GPIO_Init(GPIO_PORT_DS18B20, &GPIO_InitStructure);							  	//���������ò���д��
}
*/
unsigned char init_1820()
{
	unsigned char presence=0;

	DQ_OUT();
	DQ_SET(); 
	DQ_CLR(); //pull DQ line low
	TIMX_US_CALC(480); // leave it low for 480��s  
	DQ_SET();    
	TIMX_US_CALC(60);  // wait for presence
	//DQ_IN;   
	presence = !GetDQ();    // get presence signal
//	while(!GetDQ());
	DQ_OUT();
	DQ_SET();  

	TIMX_US_CALC(140); // wait for end of timeslot

	return(presence);  // presence signal returned
}  

//void DQ_CLR()
//{
//  rGPNDAT &=~(0x10);
//}
//
//void DQ_SET()
//{
//  rGPNDAT |=0x10;
//
//}


/*
unsigned char GetDQ(void)
{
	unsigned short temp;
	DQ_IN();
	return GPIO_ReadInputDataBit( GPIO_PORT_DS18B20, GPIO_PIN_DS18B20 );
	
//	temp = GPIO_PORT_DS18B20->IDR;	 //IDR  
//	
//	if((temp&0x0080)==0x0080)	//PB7
//	   return 1;
//	else
//	   return 0;

}

*/
//unsigned char init_1820()
//{
//	unsigned char presence=0;
//
//	//DQ_OUT;
//	DQ_SET; 
//	DQ_CLR; //pull DQ line low
//	uDelay(480); // leave it low for 480��s
//	DQ_SET;   
//	
//        
//	uDelay(60);  // wait for presence
//	//DQ_IN;   
//	presence = !GetDQ();    // get presence signal
////	while(!GetDQ());
//	//DQ_OUT;
//	DQ_SET;  
//
//	uDelay(140); // wait for end of timeslot
//
//	return(presence);  // presence signal returned
//}  

//***********************************
//���ܣ�дһBYTE
//˵����

void write_1820(unsigned char data) 
{    
     unsigned char m; 
    for(m=0;m<8;m++) 
    { 
       DQ_CLR(); 
       if(data &(1<<m))    
          DQ_SET(); 
       else 
         DQ_CLR(); 
       TIMX_US_CALC(60);  
	     
       DQ_SET(); 
    } 
       DQ_SET(); 
} 

//***********************************
//���ܣ���һBYTE
//˵����
unsigned char read_1820() 
{     
    unsigned char temp,k,n; 
    temp=0; 
    for(n=0;n<8;n++) 
    { 
      DQ_CLR() ;
      DQ_SET();
     // in_dat;
	  	//DQ_IN;    
      k=GetDQ();    //read data
      if(k) 
        temp|=(1<<n); 
      else 
        temp&=~(1<<n);
	   
	   
      TIMX_US_CALC(40);     
     // out_dat; 
	 	DQ_OUT();
   } 
   return (temp); 
} 

void Get_id()
{    
    unsigned char i;   
    // init_1820();        //resert
     write_1820(0x33); 
    
     for(i=0;i<8;i++)
     {
	     id[i]=read_1820();
     }
}

void Gettemp_Start()                   
{   
	 init_1820();        //resert
     write_1820(0xcc);   //overlook 64bit address
     write_1820(0x44);   //start convert 
}

void Gettemp_OK()                   
{   
	   unsigned char TL,TH;
	
     temperaure=0; 

	   
     init_1820();        //resert
     write_1820(0xcc);   // overlook 64bit address
     write_1820(0xbe);   //read memory     
    
     TL=read_1820();    //read temp low
     TH=read_1820();    //read temp hign
     a3=read_1820();    //read th
     a4=read_1820();    //read tl
     a5=read_1820();    //read config
     a6=read_1820();    //read reserved
     a7=read_1820();    //.............
     a8=read_1820();    //..............
     a9=read_1820();    //read crc
 

		if(TH>7)	  //12λ�¶����ݣ�TL�İ�λ��THǰ��λ
		{
			Tsymbol=1;//�¶�Ϊ��  
			
			temperaure=((TH<<8)|TL); 
			temperaure=~temperaure+1;    
			temperaure*=625;
            temperaure-=50000;//����
			
		}
		else
		{
			Tsymbol=0;//�¶�Ϊ��							
			temperaure=((TH<<8)|TL); 
			temperaure*=625;
            temperaure-=50000;//����
		}                    
/*                                      
     a[0]=temperaure/1000000;             //100 
     a[1]=temperaure%1000000/100000;      //10  
     a[2]=temperaure/10000%100%10;        //1 
     
     a[4]=temperaure%10000/1000;          //.1
     a[5]=temperaure%1000/100;            //0.01
   //  a[6]=temperaure%100/10;               //0.001
   //  a[7]=temperaure%10;                   //0.0001  
     
     a[3]=a[0]*100+a[1]*10+a[2];            //int part
     a[8]=a[4]*1000+a[5]*100+a[6]*10+a[7];  //decimal fraction 
		
		 Tem_int=a[3];
		 Tem_dec=a[8];
 */
}

void gettemp()                   
{         

	   unsigned char TL,TH;
	   extern u32 unixtime;
	 //  u32 T_S=0,T_E=0;
     temperaure=0; 

     init_1820();        //resert
     write_1820(0xcc);   //overlook 64bit address
     write_1820(0x44);   //start convert    
     
	   
	 bsp_DelayMS(1000);  
	  
	   
     init_1820();        //resert
     write_1820(0xcc);   // overlook 64bit address
     write_1820(0xbe);   //read memory     
    
     TL=read_1820();    //read temp low
     TH=read_1820();    //read temp hign
     a3=read_1820();    //read th
     a4=read_1820();    //read tl
     a5=read_1820();    //read config
     a6=read_1820();    //read reserved
     a7=read_1820();    //.............
     a8=read_1820();    //..............
     a9=read_1820();    //read crc

  

		if(TH>7)	  //12λ�¶����ݣ�TL�İ�λ��THǰ��λ
		{
			Tsymbol=1;//�¶�Ϊ��  
			
			temperaure=((TH<<8)|TL); 
			temperaure=~temperaure+1;    
			temperaure*=625;
			temperaure-=50000;//����
		}
		else
		{
			Tsymbol=0;//�¶�Ϊ��
							
			temperaure=((TH<<8)|TL); 
			temperaure*=625;
			temperaure-=50000;//����
		}                    
                                      
 /*    a[0]=temperaure/1000000;             //100 
     a[1]=temperaure%1000000/100000;      //10  
     a[2]=temperaure/10000%100%10;        //1 
     
     a[4]=temperaure%10000/1000;          //.1
     a[5]=temperaure%1000/100;            //0.01
   //  a[6]=temperaure%100/10;               //0.001
   //  a[7]=temperaure%10;                   //0.0001  
     
     a[3]=a[0]*100+a[1]*10+a[2];            //int part
     a[8]=a[4]*1000+a[5]*100+a[6]*10+a[7];  //decimal fraction 
		
		 Tem_int=a[3];
		 Tem_dec=a[8];
*/		
	//	printf("Temperature:%0.2f\r\n",(float)temperaure/10000);
               
}   

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
void get1820temper(void)
{
    char symbol,size;
    unsigned char TL,TH;
	  u32 tem;
	
     init_1820();        //resert
     write_1820(0xcc);   //overlook 64bit address
     write_1820(0x44);   //start convert    

	// uDelay(800000);
	  bsp_DelayMS(800);   
	   
	 init_1820();        //resert
     write_1820(0xcc);   // overlook 64bit address
     write_1820(0xbe);   //read memory     

     TL=read_1820();    //read temp low
     TH=read_1820();    //read temp hign
	   a3=read_1820();    //read th
     a4=read_1820();    //read tl
     a5=read_1820();    //read config
     a6=read_1820();    //read reserved
     a7=read_1820();    //.............
     a8=read_1820();    //..............
     a9=read_1820();    //read crc   	
	   
    if(TH>7)	  //12λ�¶����ݣ�TL�İ�λ��THǰ��λ
    {
      symbol=-1;//�¶�Ϊ��  
			
			tem=((TH<<8)|TL); //��ø߰�λ
			tem=~tem+1;    
			tem*=625;
      
    }
	else
	{
	  symbol=1;//�¶�Ϊ��
	  	  	  
    tem=((TH<<8)|TL); //��ø߰�λ
		tem*=625;
	}
	   

	//	SEGGER_RTT_printf(0,"��ǰ�¶ȣ�%d' \r\n",tem);
	//zhengshu=L_18B20/16+H_18B20*16; //��������
	//xiaoshu1=(L_18B20&0x0f)*10/16; //С����һλ
	//xiaoshu2=(L_18B20&0x0f)*100/16%10;//С���ڶ�λ
	   
} 
 

