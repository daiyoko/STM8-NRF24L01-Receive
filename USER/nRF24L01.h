/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：nRF24L01.c
 * 描述    ：nRF24L01无线模块实验   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-12-23

  风驰STM8开发板硬件连接
    |----------------------------|
	|  CE  - PI0             	 |
	|  SPI_CLK - PC5(SPI+SCK)	 |
    |  SPI_MOSI - PC6(SPI_MOSI)  |
    |  SPI_MISO - PC7(SPI_MISO)  |
	|  CSN - PC1             	 |
	|  IRQ - PC2             	 |
    |----------------------------|

*******************************************************************************/
#ifndef _NRF24L01_H_
#define _NRF24L01_H_
/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"

/* 自定义新类型 */

/* 自定义宏 */
/*
 * nRF24L01控制引脚宏定义
 */
#define MISO	GPIO_PIN_7
#define MOSI	GPIO_PIN_6
#define	SCK	    GPIO_PIN_5
#define	CE	    GPIO_PIN_0	/* RX/TX模式选择端 */
#define	CSN		GPIO_PIN_1	/* SPI片选端 */
#define IRQ		GPIO_PIN_2	/* 可屏蔽中断 */

#define SetCE	GPIO_WriteHigh(GPIOI, CE)
#define ClrCE	GPIO_WriteLow(GPIOI, CE)

#define SetCSN	GPIO_WriteHigh(GPIOC, CSN)
#define ClrCSN	GPIO_WriteLow(GPIOC, CSN)

#define TX_ADDR_WITDH 	5		/* 发送地址宽度设置为5个字节 */
#define RX_ADDR_WITDH 	5		/* 接收地址宽度设置为5个字节 */
#define TX_DATA_WITDH 	32		/* 发送数据宽度4个字节 */
#define RX_DATA_WITDH 	32		/* 接收数据宽度4个字节 */


/* 
 *命令寄存器
 */
#define  R_REGISTER      0x00//读取配置寄存器
#define  W_REGISTER      0x20//写配置寄存器
#define  R_RX_PAYLOAD 	 0x61//读取RX有效数据
#define  W_TX_PAYLOAD	 0xa0//写TX有效数据
#define  FLUSH_TX		 0xe1//清除TXFIFO寄存器
#define  FLUSH_RX		 0xe2//清除RXFIFO寄存器
#define  REUSE_TX_PL     0xe3//重新使用上一包有效数据
#define  NOP             0xff//空操作

/*
 *寄存器地址
 */
#define  CONFIG          0x00//配置寄存器
#define  EN_AA			 0x01//使能自动应答
#define  EN_RXADDR       0x02//接收通道使能0-5个通道
#define  SETUP_AW        0x03//设置数据通道地址宽度3-5
#define  SETUP_RETR      0x04//建立自动重发
#define  RF_CH           0x05//射频通道设置
#define  RF_SETUP        0x06//射频寄存器
#define  STATUS          0x07//状态寄存器
#define  OBSERVE_TX      0x08//发送检测寄存器
#define  CD              0x09//载波
#define  RX_ADDR_P0      0x0a//数据通道0接收地址
#define  RX_ADDR_P1      0x0b//数据通道1接收地址
#define  RX_ADDR_P2      0x0c//数据通道2接收地址
#define  RX_ADDR_P3      0x0d//数据通道3接收地址
#define  RX_ADDR_P4      0x0e//数据通道4接收地址
#define  RX_ADDR_P5      0x0f//数据通道5接收地址
#define  TX_ADDR         0x10//发送地址
#define  RX_PW_P0        0x11//P0通道数据宽度设置
#define  RX_PW_P1        0x12//P1通道数据宽度设置
#define  RX_PW_P2        0x13//P2通道数据宽度设置
#define  RX_PW_P3        0x14//P3通道数据宽度设置
#define  RX_PW_P4        0x15//P4通道数据宽度设置
#define  RX_PW_P5        0x16//P5通道数据宽度设置
#define  FIFO_STATUS     0x17//FIFO状态寄存器


/* 全局变量定义 */


/* 函数声明 */



/*******************************************************************************
 * 名称: NRF24L01_Pin_Conf
 * 功能: nRF24L01控制引脚初始化和SPI模块初始化
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void nRF24L01_Pin_Conf();

/*******************************************************************************
 * 名称: nRF24L01_SPI_RW
 * 功能: nRF24L01 SPI模式下读写操作函数
 * 形参: data -> 要写nRF24L01的数据
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_SPI_RW(u8 date);

/*******************************************************************************
 * 名称: nRF24L01_Read_Reg
 * 功能: 读取nRF24L01寄存器函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_Read_Reg(u8 RegAddr);

/*******************************************************************************
 * 名称: nRF24L01_Write_Reg
 * 功能: 向nRF24L01寄存器写数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 data -> 写入寄存器的数据
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_Write_Reg(u8 RegAddr,u8 data);

/*******************************************************************************
 * 名称: nRF24L01_Write_Reg
 * 功能: 读取nRF24L01RX FIFO的数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 Rxdata -> 指向存放数据区的指针
 * 	     DataLen -> 数据长度
 * 返回: nRF24L01返回的数据
 * 说明: 接收模式下使用
 ******************************************************************************/
static u8 nRF24L01_Read_RxData(u8 RegAddr, u8 *RxData, u8 DataLen);

/*******************************************************************************
 * 名称: nRF24L01_Write_TxData
 * 功能: 写入nRF24L01TX FIFO的数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 Rxdata -> 指向要写入数据的指针
 * 	     DataLen -> 数据长度
 * 返回: nRF24L01返回的数据
 * 说明: 发送模式下使用
 ******************************************************************************/
static u8 nRF24L01_Write_TxData(u8 RegAddr,u8 *TxData,u8 DataLen);

/*******************************************************************************
 * 名称: nRF24L01_Set_RxMode
 * 功能: 将nRF24L01TX FIFO设置为接收模式
 * 形参: 无
 * 返回: 无
 * 说明: 
 ******************************************************************************/
void nRF24L01_Set_RxMode();

/*******************************************************************************
 * 名称: nRF24L01_RevData
 * 功能: 接收数据函数
 * 形参: RevData -> 指向存放数据缓冲区
 * 返回: 1--接收数据失败，0--接收数据成功
 * 说明: 
 ******************************************************************************/
u8 nRF24L01_RevData(u8 *RevData);

#endif