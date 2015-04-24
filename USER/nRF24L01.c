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

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"
#include "uart1.h"
#include "nRF24L01.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

u8  TxAddr[]={0x34,0x43,0x10,0x10,0x01};//发送地址

/* 一个简单的延时函数 */
void Delay(u32 cnt)
{
	u32 i;
	
	for (; cnt > 0; cnt--) {
		for (i = 0; i < 3000; i++)
			;
	}
}

/*******************************************************************************
 * 名称: NRF24L01_Pin_Conf
 * 功能: nRF24L01控制引脚初始化和SPI模块初始化
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void nRF24L01_Pin_Conf()
{
	/* 初始化SPI--高位在前，6M, 主模式，SCK空闲为低电平，第一个边沿采集数据 */
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,\
				SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, \
				SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
	
	SPI_Cmd(ENABLE);	/* 使能SPI */
	
	GPIO_Init(GPIOC, CSN, GPIO_MODE_OUT_PP_HIGH_FAST);	/* CSN引脚初始化为输出 */
	GPIO_Init(GPIOI, CE, GPIO_MODE_OUT_PP_HIGH_FAST);	/* CE引脚初始化为输出 */
}


/*******************************************************************************
 * 名称: nRF24L01_SPI_RW
 * 功能: nRF24L01 SPI模式下读写操作函数
 * 形参: data -> 要写nRF24L01的数据
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_SPI_RW(u8 date)
{
	/* 等待DR寄存器非空 */
	while (SPI_GetFlagStatus( SPI_FLAG_TXE) == RESET);
	
	/* 通过SPI发送一个字节 */
	SPI_SendData(date);
	
	/* 等待接收一个字节 */
	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
	
	/* 返回SPI总线上的字节 */
	return SPI_ReceiveData();  
}

/*******************************************************************************
 * 名称: nRF24L01_Read_Reg
 * 功能: 读取nRF24L01寄存器函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_Read_Reg(u8 RegAddr)
{
   u8 BackData;
   
   ClrCSN;							/* 启动时序 */
   nRF24L01_SPI_RW(RegAddr);		/* 写寄存器地址 */
   BackData = nRF24L01_SPI_RW(0x00);	/* 写入读寄存器指令 */ 
   SetCSN;
   
   return(BackData); 				/* 返回状态值 */
}

/*******************************************************************************
 * 名称: nRF24L01_Write_Reg
 * 功能: 向nRF24L01寄存器写数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 data -> 写入寄存器的数据
 * 返回: nRF24L01返回的数据
 * 说明: 无 
 ******************************************************************************/
static u8 nRF24L01_Write_Reg(u8 RegAddr,u8 data)
{
   u8 BackData;
   
   ClrCSN;						/* 启动时序 */
   BackData = nRF24L01_SPI_RW(RegAddr);	/* 写入地址 */
   nRF24L01_SPI_RW(data);				/* 写入值 */
   SetCSN;
   
   return(BackData);
}

/*******************************************************************************
 * 名称: nRF24L01_Write_Reg
 * 功能: 读取nRF24L01RX FIFO的数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 Rxdata -> 指向存放数据区的指针
 * 	     DataLen -> 数据长度
 * 返回: nRF24L01返回的数据
 * 说明: 接收模式下使用
 ******************************************************************************/
static u8 nRF24L01_Read_RxData(u8 RegAddr, u8 *RxData, u8 DataLen)
{ 
	u8 BackData,i;
	
	ClrCSN;									/* 启动时序 */
	BackData = nRF24L01_SPI_RW(RegAddr);	/* 写入要读取的寄存器地址 */
	for(i = 0; i < DataLen; i++) 			/* 读取数据 */
	{
		RxData[i] = nRF24L01_SPI_RW(0);
	} 
	
	SetCSN;
	
	return(BackData); 
}

/*******************************************************************************
 * 名称: nRF24L01_Write_TxData
 * 功能: 写入nRF24L01TX FIFO的数据函数
 * 形参: RegAddr -> nRF24L01寄存器地址
 *		 Rxdata -> 指向要写入数据的指针
 * 	     DataLen -> 数据长度
 * 返回: nRF24L01返回的数据
 * 说明: 发送模式下使用
 ******************************************************************************/
static u8 nRF24L01_Write_TxData(u8 RegAddr,u8 *TxData,u8 DataLen)
{ 
	u8 BackData,i;
	
	ClrCSN;
	BackData = nRF24L01_SPI_RW(RegAddr);	/* 写入要写入寄存器的地址 */
	for(i = 0; i < DataLen; i++)			/* 写入数据 */
	{
		nRF24L01_SPI_RW(*TxData++);
	}   
	
	SetCSN;
	return(BackData);
}

/*******************************************************************************
 * 名称: nRF24L01_Set_RxMode
 * 功能: 将nRF24L01TX FIFO设置为接收模式
 * 形参: 无
 * 返回: 无
 * 说明: 
 ******************************************************************************/
void nRF24L01_Set_RxMode()
{
	ClrCE; 
	/* 接收设备接收通道0使用和发送设备相同的发送地址 */
  	nRF24L01_Write_TxData(W_REGISTER+RX_ADDR_P0, TxAddr, TX_ADDR_WITDH);  
	
	/******下面有关寄存器配置**************/
  	nRF24L01_Write_Reg(W_REGISTER+EN_AA,0x01);       /* 使能接收通道0自动应答 */
  	nRF24L01_Write_Reg(W_REGISTER+EN_RXADDR,0x01);   /* 使能接收通道0 */
  	nRF24L01_Write_Reg(W_REGISTER+RF_CH,0x40);       /* 选择射频通道0x40 */
	nRF24L01_Write_Reg(W_REGISTER+RX_PW_P0,TX_DATA_WITDH);  /* 接收通道0选择和发送通道相同有效数据宽度 */
  	nRF24L01_Write_Reg(W_REGISTER+RF_SETUP,0x07);    /* 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益 */
	nRF24L01_Write_Reg(W_REGISTER+CONFIG,0x0f);      /* CRC使能，16位CRC校验，上电 */ 
	SetCE;
	Delay(5);	/* 按照芯片手册要求 保持10us秒以上 */
  
}

/*******************************************************************************
 * 名称: nRF24L01_RevData
 * 功能: 接收数据函数
 * 形参: RevData -> 指向存放数据缓冲区
 * 返回: 1--接收数据失败，0--接收数据成功
 * 说明: 
 ******************************************************************************/
u8 nRF24L01_RevData(u8 *RevData)
{
	u8 RevFlags = 1;
	u8 sta;
	
	sta = nRF24L01_Read_Reg(R_REGISTER+STATUS);	/* 读取状态寄存器的值 */
	if(sta & 0x40)				/* 判断是否接收到数据 */
	{
		ClrCE; 			
		nRF24L01_Read_RxData(R_RX_PAYLOAD,RevData,RX_DATA_WITDH);	/* 从RXFIFO读取4位数据 */
		
		/* 接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志 */
		nRF24L01_Write_Reg(W_REGISTER+STATUS,0xff); 
		
		ClrCSN;
	   	nRF24L01_SPI_RW(FLUSH_RX);	/* 用于清空FIFO */
	   	SetCSN;	
		
		RevFlags = 0;	   
	}
	
	return(RevFlags);
}

/*************************接收数据*********************************************/
void GetDate(u8 *data)				 
{
  u8 sta;
    sta=nRF24L01_Read_Reg(R_REGISTER+STATUS);//发送数据后读取状态寄存器
    if(sta&0x40)				// 判断是否接收到数据
     {
	   ClrCE;//CE=0;//待机
	   nRF24L01_Read_RxData(R_RX_PAYLOAD,data,RX_DATA_WITDH);// 从RXFIFO读取数据 接收4位即可，后一位位结束位
	   
	  
	   nRF24L01_Write_Reg(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标
	   ClrCSN;//CSN=0;
	   nRF24L01_SPI_RW(FLUSH_RX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！ 
	   SetCSN;//CSN=1;		
	   UART1_SendString(data, 4);
	   
     }
    //NRFWriteReg(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标	  
}