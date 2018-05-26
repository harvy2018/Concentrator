/*
*********************************************************************************************************
*
*	ģ������ : ����FLASH  SST25VF016B����ģ��
*	�ļ����� :  bsp_SST25VF016B.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-08-21 armfly  ST�̼���������V3.5.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_SST25VF016B_H
#define _BSP_SST25VF016B_H

#include <inttypes.h>

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#endif


#define SPI_FLASH          SPI2
#define SF_CLK             RCC_APB1Periph_SPI2
#define SF_GPIO            GPIOB
#define SF_GPIO_CLK        RCC_APB2Periph_GPIOB
#define SF_PIN_SCK         GPIO_Pin_13
#define SF_PIN_MISO        GPIO_Pin_14
#define SF_PIN_MOSI        GPIO_Pin_15
#define SF_WP_PIN          GPIO_Pin_3

#define SF_WP_GPIO         GPIOB

#define SF_CS             GPIO_Pin_12
#define SF_CS_GPIO        GPIOB
#define SF_CS_GPIO_CLK    RCC_APB2Periph_GPIOB

/* Ƭѡ�����õ�ѡ��  */
#define SF_CS_LOW()       GPIO_ResetBits(SF_CS_GPIO, SF_CS)

/* Ƭѡ�����ø߲�ѡ�� */
#define SF_CS_HIGH()      GPIO_SetBits(SF_CS_GPIO, SF_CS)

#define CMD_AAI       0xAD  	/* AAI �������ָ�� */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_EWRSR	  0x50		/* ����д״̬�Ĵ��������� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_SE        0x20		/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define DUMMY_BYTE    0xA5		/* ���������Ϊ����ֵ�����ڶ����� */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */


/* ���崮��Flash ID */
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID = 0xC22015
};

#define SF_UNKNOW_NAME	"Unknow"



typedef struct
{
	uint32_t ChipID;		/* оƬID */
	char ChipName[16];		/* оƬ�ͺ��ַ�������Ҫ������ʾ */
	uint32_t TotalSize;		/* ������ */
	uint16_t PageSize;		/* ҳ���С */
}SFLASH_T;

extern SFLASH_T g_tSF;

#define SF_TOTAL_SIZE		(2*1024*1024)	/* ��������2M�ֽ� */
#define SF_PAGE_SIZE    	(4*1024)		/* ҳ���С��4K�ֽ� */

void sf_InitHard(void);
uint32_t sf_ReadID(void);
void sf_EraseChip(void);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);

uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_EraseBlock(uint8_t blocksize, uint32_t _uiSectorAddr);


#endif
