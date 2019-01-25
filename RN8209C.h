/*************** (C) COPYRIGHT  杭州中深电力技术有限公司有限公司 **************
 * 文件名  ：RN8209C.c
 * 描述    ：RN8209C的驱动函数
 * 实验平台：X965M X964A X963B
 * 库版本  ：V2.1.0
 * 作者    ：MoYi  QQ：29577332
 * 博客    ：
 * 淘宝    ：
 * 修改时间 ：2018-03-25
 * 注意    ：RN8209C 本身是采用串口驱动的，
                        但是因为cpu只有一组串口所以本驱动是采用io模拟的方式驱动
  X963MV1 硬件连接
    |--------------------|
    |  TXD0-PB0          |
    |  RXD0-PB1          |
    |--------------------|

*******************************************************************************/

#ifndef __RN8209C_H
#define __RN8209C_H

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"
#include "main.h"

/* 自定义新类型 */

/* 自定义宏 */
/* 1629 芯片所接的GPIO端口定义 */
#define RN8209C_PORTB   GPIOB   /* 定义1629芯片所接GPIO端口 */
/* 1629所接的GPIO引脚定义 */
#define RN8209CTXD      GPIO_PIN_1
#define RN8209CRXD      GPIO_PIN_0

/* 全局变量定义 */

/* 8209C内部寄存器的地址定义 */
#define SYSCON      0x00
#define EMUCON      0x01
#define HFConst      0x02
#define PStart         0x03
#define DStart         0x04
#define GPQA          0x05
#define GPQB          0x06
#define PhsA           0x07
#define PhsB            0x08
#define QPhsCal     0x09
#define APOSA       0x0A
#define APOSB       0x0B
#define RPOSA       0x0C
#define RPOSB       0x0D
#define IARMSOS     0x0E
#define IBRMSOS     0x0F
#define IBGain        0x10
#define D2FPL         0x11
#define D2FPH        0x12
#define DCIAH        0x13
#define DCIBH        0x14
#define DCUH         0x15
#define DCL            0x16
#define EMUCON2   0x17
/* 计量参数和状态寄存器 */
#define PFCnt       0x20
#define DFCnt       0x21
#define IARMS      0x22
#define IBRMS       0x23
#define URMS        0x24
#define UFreq       0x25
#define PowerPA     0x26
#define PowerPB     0x27
#define PowerQ      0x28
#define EnergyP     0x29
#define EnergyP2   0x2A
#define EnergyD     0x2B
#define EnergyD2    0x2C
#define EMUStatus   0x2D
#define SPL_IA       0x30
#define SPL_IB       0x31
#define SPL_U        0x32
/* 中断寄存器 */
#define IE8209      0x40
#define IF8209      0x41
#define RIF            0x42
/* 系统状态寄存器 */
#define SysStatus     0x43
#define RData           0x44
#define WData          0x45
#define DeviceID      0x7F

/* 特殊命令寄存器 */
#define CMD8209     0xEA

/* 声明给外部使用的函数 */

/*******************************************************************************
 * 名称: RN8209C_Init
 * 功能: RN8209C外设GPIO引脚初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无
           1 对8209 rx 加入一个约 60ms的延时
           2 初始化uart3
           3 初始化8209的配置
 ******************************************************************************/
void RN8209C_Init(void);

/*******************************************************************************
 * 名称: USART_TXITConfig
 * 功能: 开始 tx 或者 关闭tx
 * 形参: newstate  disable enable
 * 返回: 无
 * 说明: 1 开始发送时 直接就发送第一个字节并开中断
 *         2 关闭发送时 直接关中断
 *
  ******************************************************************************/
void USART_TXITConfig(FunctionalState NewState);

/*******************************************************************************
 * 名称: fnRN8209_Write
 * 功能: 向8209 的对应地址写入相应的数据并验证
 * 形参: dst 地址; src 数据缓冲; datlen ;数据长度 repeat 重复次数
 * 返回: 无
 * 说明: 1 注意stm8是大端模式
 *         2 高位数据放在[0]
 *         3 暂时没有验证
 *
  ******************************************************************************/
ErrorStatus fnRn8209_Write(u8 Dst , u8 *Src , u8 DatLen);

/*******************************************************************************
 * 名称: fnRN8209_read1
 * 功能: 向8209 的对应地址读出数据
 * 形参: dst 地址; src 数据缓冲; datlen ;数据长度
 * 返回: success error
 * 说明: 1 注意stm8是大端模式
 *         2 高位数据放在[0]
 *         3 采用效验和验证
 *
  ******************************************************************************/
ErrorStatus fnRn8209_Read1(u8 Dst, u8 *data , u8 ucLen);

#endif

/*************** (C) COPYRIGHT 杭州浙宝微电脑技术有限公司 *****END OF FILE****/

