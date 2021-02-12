/******************************************************************************
文件名称: I2C.c
文件标识: STC8A8K64S4A12
摘    要: I2C硬件操作函数
当前版本: V1.0	
完成日期: 2021.01.24
*******************************************************************************/
#define	USER_I2C_GLOBALS
#include "include.h"



#define Interrupt //中断方法
//#define Query     //查询方法

/*****************************************************************************
函数名称 : I2C_init
功能描述 : I2C初始化
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void I2C_init(unsigned char choice)
{
    P_SW2 = 0x80;
   switch(choice)
    {
        case(1):P_SW2 |= 0x00;   break;        //SCL/P1.5, SDA/P1.4
        case(2):P_SW2 |= 0x10;   break;        //SCL_2/P2.5, SDA_2/P2.4 //SHT30
        case(3):P_SW2 |= 0x20;   break;        //SCL_3/P7.7, SDA_3/P7.6
        case(4):P_SW2 |= 0x30;   break;        //SCL_4/P3.2, SDA_4/P3.3 //OLED
    }
    //P_SW2 |= 0x10;
	I2CCFG = 0xC0|MSSPEED;   //使能I2C主机模式
	I2CMSST = 0x00;
}

#ifdef Interrupt
bit busy1;

/*****************************************************************************
函数名称 : I2C_Isr
功能描述 : I2C中断处理函数
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void I2C_Isr() interrupt 24
{
    char store;
    
    store = P_SW2;
    P_SW2 |= 0x80;
    if (I2CMSST & 0x40)
    {
        I2CMSST &= ~0x40;                       //清中断标志
        busy1 = 0;
    }
    P_SW2 = store;
}
/*****************************************************************************
函数名称 : Start
功能描述 : 发送START命令
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Start()  //发送START命令
{
	busy1 = 1;
  I2CMSCR = 0x81;                           
  while (busy1);
}
/*****************************************************************************
函数名称 : SendData
功能描述 : 发送一个字节
输入参数 : dat:待发送数据
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void SendData(char dat)  //发送一个字节
{
	I2CTXD = dat;        //写数据到数据缓冲区                              
	busy1 = 1;
	I2CMSCR = 0x82;        //发送SEND命令                       
	while (busy1);
}
/*****************************************************************************
函数名称 : RecvACK
功能描述 : 发送并检查应答
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
/*void RecvACK()       //发送并检查应答
{
	busy1 = 1;
	I2CMSCR = 0x83;      //发送读ACK命令                             
	while (busy1);
	//return ACK;
}*/
/*****************************************************************************
函数名称 : RecvData
功能描述 : 接收一个字节
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
char RecvData() //接收一个字节
{
	busy1 = 1;
	I2CMSCR = 0x84;    //发送RECV命令
	while (busy1);
	return I2CRXD;
}
/*****************************************************************************
函数名称 : SendACK
功能描述 : 发送应答位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void SendACK() //发送应答位
{
	I2CMSST = 0x00;           //设置ACK信号
	busy1 = 1;
	I2CMSCR = 0x85;           //发送ACK命令
	while (busy1);
}
/*****************************************************************************
函数名称 : SendNAK
功能描述 : 发送非应答位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
/*void SendNAK()  //发送非应答位
{
	I2CMSST = 0x01;       //设置NAK信号
	busy1 = 1;
	I2CMSCR = 0x85;       //发送NAK信号
	while (busy1);
}*/
/*****************************************************************************
函数名称 : Stop
功能描述 : 发送停止位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Stop() //发送停止位
{
	busy1 = 1;
	I2CMSCR = 0x86;     //发送STOP命令
	while (busy1);
}



#elif defined(Query)

/*****************************************************************************
函数名称 : Wait
功能描述 : 检测状态
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Wait()
{
	while(!(I2CMSST&0X40));
	I2CMSST &= ~0X40;
}
/*****************************************************************************
函数名称 : Start
功能描述 : 发送START命令
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Start()  //发送START命令
{
  I2CMSCR = 0x01;                           
  Wait();
}
/*****************************************************************************
函数名称 : SendData
功能描述 : 发送一个字节
输入参数 : dat:待发送数据
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void SendData(char dat)  //发送一个字节
{
	I2CTXD = dat;        //写数据到数据缓冲区
	I2CMSCR = 0x02;        //发送SEND命令                       
	Wait();
}
/*****************************************************************************
函数名称 : RecvACK
功能描述 : 发送并检查应答
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
/*void RecvACK()       //发送并检查应答
{
	I2CMSCR = 0x03;      //发送读ACK命令                             
	Wait();
}*/
/*****************************************************************************
函数名称 : RecvData
功能描述 : 接收一个字节
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
char RecvData() //接收一个字节
{
	I2CMSCR = 0x04;    //发送RECV命令
	Wait();
	return I2CRXD;
}
/*****************************************************************************
函数名称 : SendACK
功能描述 : 发送应答位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void SendACK() //发送应答位
{
	I2CMSST = 0x00;           //设置ACK信号
	I2CMSCR = 0x05;           //发送ACK命令
	Wait();
}
/*****************************************************************************
函数名称 : SendNAK
功能描述 : 发送非应答位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
/*void SendNAK()  //发送非应答位
{
	I2CMSST = 0x01;       //设置NAK信号
	I2CMSCR = 0x05;       //发送NAK信号
	Wait();
}*/
/*****************************************************************************
函数名称 : Stop
功能描述 : 发送停止位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Stop() //发送停止位
{
	I2CMSCR = 0x86;     //发送STOP命令
	Wait();
}
#endif

/*****************************************************************************
函数名称 : WriteCmd
功能描述 : 写指令函数
输入参数 : command：指令
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void WriteCmd(uint8_t command ,uint8_t addr)
{
	Start();
	SendData(addr);//OLED地址
	SendACK();
	SendData(0x00);//寄存器地址
	SendACK();
	SendData(command);
	SendACK();
	Stop();
}
/*****************************************************************************
函数名称 : WriteDat
功能描述 : 写数据函数
输入参数 : Data：数据
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void WriteDat(uint8_t Data,uint8_t addr)
{
	Start();
	SendData(addr);                        //D/C#=0; R/W#=0
	SendACK();       
	SendData(0x40);                        //write data
	SendACK();       
	SendData(Data);
	SendACK();       
	Stop();
}