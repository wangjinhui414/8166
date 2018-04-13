#include "bsp_spi.h"

#define SPI_FLASH_PageSize    0x0200


// SPI FLASH操作指令定义
// Read Commands
#define  CMD_PAGE_READ     0x52        // Main Memory Page Read
#define  CMD_ARRAY_READ    0xE8        // Continuous Array Read (Legacy Command)
#define  CMD_ARRAY_READ_L  0x03        // Continuous Array Read (Low Frequency)
#define  CMD_ARRAY_READ_H  0x0B        // Continuous Array Read (High Frequency)
#define  CMD_BUF1_READ_L   0xD1        // Buffer 1 Read (Low Frequency)
#define  CMD_BUF2_READ_L   0xD3        // Buffer 2 Read (Low Frequency)
#define  CMD_BUF1_READ     0xD4        // Buffer 1 Read
#define  CMD_BUF2_READ     0xD6        // Buffer 2 Read

//Program and Erase Commands
#define  CMD_BUF1_WRITE     0x84       // Buffer 1 Write
#define  CMD_BUF2_WRITE     0x87       // Buffer 2 Write
#define  CMD_BUF1PAGE_PROGE 0x83       // Buffer 1 to Main Memory Page Program with Built In Erase
#define  CMD_BUF2PAGE_PROGE 0x86       // Buffer 2 to Main Memory Page Program with Built In Erase
#define  CMD_BUF1PAGE_PROG  0x88       // Buffer 1 to Main Memory Page Program without Built In Erase
#define  CMD_BUF2PAGE_PROG  0x89       // Buffer 2 to Main Memory Page Program without Built In Erase

#define  CMD_PAGE_ERASE     0x81        // Page Erase 
#define  CMD_BLOCK_ERASE    0x50        // Block Erase
#define  CMD_SECTOR_ERASE   0x7C        // Sector Erase
#define  CMD_CHIP_ERASE     0xC7        // Chip Erase: C7H, 94H, 80H, 9AH

#define  CMD_PAGEBUF1_PROG   0x82        // Main Memory Page Program Through Buffer 1
#define  CMD_PAGEBUF2_PROG   0x85        // Main Memory Page Program Through Buffer 2

// Protection and Security Commands
#define  CMD_SECTOR_PRO_ENA        0xA9 // Enable Sector Protection: 3DH + 2AH + 7FH + A9H
#define  CMD_SECTOR_PRO_DIS        0x9A // Disable Sector Protection: 3DH + 2AH + 7FH + 9AH
#define  CMD_SECTOR_PROREG_ERASE   0xCF // Erase Sector Protection Register: 3DH + 2AH + 7FH + CFH
#define  CMD_SECTOR_PROREG_PROG    0xFC // Program Sector Protection Register: 3DH + 2AH + 7FH + FCH
#define  CMD_SECTOR_PROREG_READ    0x32 // Read Sector Protection Register: 32H

#define  CMD_SECTOR_LOCKDOWN       0x30 // Sector Lockdown: 3DH + 2AH + 7FH + 30H
#define  CMD_SECTOR_LDREG_READ     0x35 // Read Sector Lockdown Register: 35H
#define  CMD_SECURITY_REG_PROG     0x9B // Program Security Register: 9BH + 00H + 00H + 00H
#define  CMD_SECURITY_REG_READ     0x77 // Read Security Register: 77H

// Additional Commands
#define  CMD_PAGE_TOBUF1           0x53 // Main Memory Page to Buffer 1 Transfer: 53H
#define  CMD_PAGE_TOBUF2           0x55 // Main Memory Page to Buffer 2 Transfer: 55H
#define  CMD_PAGEBUF1_COMP         0x60 // Main Memory Page to Buffer 1 Compare:  60H
#define  CMD_PAGEBUF2_COMP         0x61 // Main Memory Page to Buffer 2 Compare:  61H
#define  CMD_BUF1PAGE_REWRITE      0x58 // Auto Page Rewrite through Buffer 1:    58H
#define  CMD_BUF2PAGE_REWRITE      0x59 // Auto Page Rewrite through Buffer 2:    59H
#define  CMD_DEEP                  0xB9 // Deep Power-down:                       B9H
#define  CMD_RESUME_DEEP           0xAB // Resume from Deep Power-down:           ABH
#define  CMD_STATUS_READ           0xD7 // Status Register Read:                  D7H
#define  CMD_ID_READ               0x9F // Manufacturer and Device ID Read:       9FH

//Legacy Commands
#define  CMD_BUF1_READ_LEGACY      0x54 // Buffer 1 Read: 54H
#define  CMD_BUF2_READ_LEGACY      0x56 // Buffer 2 Read: 56H
#define  CMD_PAGE_READ_LEGACY      0x52 // Main Memory Page Read: 52H
#define  CMD_ARRAY_READ_LEGACY     0x68 // Continuous Array Read: 68H
#define  CMD_STATUSREG_READ_LEGACY 0x57 // Status Register Read(2): 57H



// SOFTWARE STATUS REGISTER
#define  BUSY     0x80  //bit7, 1: No busy,         0: busy state
#define  COMP     0x40  //bit6, 1: Compare error,   0: Compare true
#define  PROTECT  0x02  //bit1, 1: Protect enable,  0: Protect disable
#define  PAGESIZE 0x01  //bit0, 1: Page size=512B,  0: Page size=528B



#define  DEF_74HC595_ENABLE           GPIO_ResetBits(GPIOD, BSP_GPIOD_H595EN)
#define  DEF_74HC595_DISENABLE        GPIO_SetBits(GPIOD, BSP_GPIOD_H595EN)
#define  DEF_74HC595_RCLK_LOW        GPIO_ResetBits(GPIOE, BSP_GPIOE_H595RCLK)
#define  DEF_74HC595_RCLK_HIGH       GPIO_SetBits  (GPIOE, BSP_GPIOE_H595RCLK)
#define  DEF_74HC597_RCLK_LOW        GPIO_ResetBits(GPIOD, BSP_GPIOD_HC597_RLCK)
#define  DEF_74HC597_RCLK_HIGH       GPIO_SetBits  (GPIOD, BSP_GPIOD_HC597_RLCK)
#define  DEF_74HC597_LOADIN_LOW      GPIO_ResetBits(GPIOD, BSP_GPIOD_HC597_LOADIN)
#define  DEF_74HC597_LOADIN_HIGH     GPIO_SetBits  (GPIOD, BSP_GPIOD_HC597_LOADIN)

#define  DEF_SPI_FLASH_CS_LOW       GPIO_ResetBits(GPIOA, BSP_GPIOA_SPIFLASH_CS)
#define  DEF_SPI_FLASH_CS_HIGH      GPIO_SetBits(GPIOA, BSP_GPIOA_SPIFLASH_CS)
#define  DEF_SPI_SD_CS_LOW       GPIO_ResetBits(GPIOD, BSP_GPIOD_SPISD_CS)
#define  DEF_SPI_SD_CS_HIGH      GPIO_SetBits(GPIOD, BSP_GPIOD_SPISD_CS)


#define  DEF_DELAY_MS(x)    if(OSRunning > 0){ OSTimeDly(x);} else{SoftDelayMS(1);}      

u8 SPIFlash_Status = 0;

static u8 SPIFlash_ReadStatus(void);
static u32 SPIFlash_BusyWait(void);
/*
*******************************************
     SPI数据读写
*******************************************
*/
u8 SPI_ReadWriteByte(SPI_TypeDef* SPIx, u8 Data)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPIx, Data);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPIx);
}
/*
*******************************************
           SPI1初始化
*******************************************
*/
void BSP_SPI1_Init(void)
{	
  SPI_InitTypeDef  SPI_InitStructure;
	
  /* Enable SPI_MASTER */	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
  SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	
	DEF_SPI_FLASH_CS_HIGH;
	DEF_SPI_SD_CS_HIGH;
  DEF_74HC597_RCLK_HIGH;
	DEF_74HC595_ENABLE;
	
	BSP_Write_DOUT1_24(HC595_DOUT_ALL ,0);
  SPIFlash_Status = SPIFlash_ReadStatus();

}
/*
*******************************************
       级联74HC595输出函数
       DOUT 1-24 输出函数
DOUT1-6   为MCU直接驱动的继电器
DOUT7-24  为OPT电压输出
*******************************************
*/
void BSP_74HC595_Out(u8* src , u8 len)
{
	u8 i;
	// SPI写入数值
	for(i=0;i<len;i++)
	{
		SPI_ReadWriteByte(SPI1, src[i]);
	}
	// RCLK 上升沿  移位锁定输出IO
	DEF_74HC595_RCLK_LOW;
  DEF_DELAY_MS(1)
	DEF_74HC595_RCLK_HIGH;
}
/*
*******************************************
       级联74HC595输出函数
       DOUT 1-24 输出函数
DOUT1-6   为MCU直接驱动的继电器
DOUT7-24  为OPT电压输出
data的bit0 对应 DOUT1
.
.
data的bit23 对应 DOUT24
*******************************************
*/
void BSP_Write_DOUT1_24(u32 data , u8 state)
{
	static u32 outstate = 0;
	if(state)
	{
		outstate |= data;
	}
	else
	{
		outstate &= ~data;
	}
	SPI_ReadWriteByte(SPI1, outstate>>16);
	SPI_ReadWriteByte(SPI1, outstate>>8);
	SPI_ReadWriteByte(SPI1, outstate);
	// RCLK 上升沿  移位锁定输出IO
	DEF_74HC595_RCLK_LOW;
  DEF_DELAY_MS(1)
	DEF_74HC595_RCLK_HIGH;
}
/*
*******************************************
        MCU DIN 数据读取
DIN1-8   为MCU直接读取         通过pin直接读取
DIN9-24  为74HC597             SPI读取
*******************************************
*/

u8 BSP_Read_DIN1_8(u16 pin)	 	// 读取DINX输入值 
{
	return !GPIO_ReadInputDataBit(GPIOE, pin);
}
/*
*******************************************
        MCU DIN 数据读取
DIN1-8   为MCU直接读取         通过pin直接读取
DIN9-24  为74HC597             SPI读取 
*******************************************
*/

u16 BSP_Read_DIN9_24(void)
{
		u16 ret = 0;

		DEF_74HC597_RCLK_LOW;
		DEF_DELAY_MS(1)
		DEF_74HC597_RCLK_HIGH;
		DEF_DELAY_MS(1)

	  DEF_74HC597_LOADIN_LOW;
	  DEF_DELAY_MS(1)  	
	  DEF_74HC597_LOADIN_HIGH;		
    DEF_DELAY_MS(1)
	
		ret |= SPI_ReadWriteByte(SPI1, 0) << 8;		//读取输入IO DI9-> DI16
		ret |= SPI_ReadWriteByte(SPI1, 0) ;		//读取输入IO DI1-> DI8
 
	  return ~ret;
}





/***********************************************************************************
* Function: SPIFlash_ReadStatus;
*
* Description: 读取状态寄存器函数;
*              
* Input:  none;
*
* Output: none;
*
* Return: 状态寄存器值;
*
* Note:   none;
************************************************************************************/
u8 SPIFlash_ReadStatus(void)
{
	u8 rst;

	// 读取状态寄存器
	rst = CMD_STATUS_READ;
	DEF_SPI_FLASH_CS_LOW;	          // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , rst);      // 发送指令
	rst = SPI_ReadWriteByte(SPI1 ,0x00); // 读数据	   
	DEF_SPI_FLASH_CS_HIGH;		  // 禁止芯片操作

	return rst; 		 
}

/***********************************************************************************
* Function: SPIFlash_BusyWait;
*
* Description: 读取状态寄存器函数;
*              
* Input:  none;
*
* Output: ERR_TRUE, 等到不忙;  ERR_FALSE, 等等待不忙超时;
*
* Return: 状态寄存器值;
*
* Note:   none;
************************************************************************************/
u32 SPIFlash_BusyWait(void)
{
  u32 nCount = 1000; 	

	while((BUSY&SPIFlash_ReadStatus())==0)
	{
		for(; nCount != 0; nCount--);
	}
	
	return 0; 		 
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SPI_FLASH_SectorErase(u32 SectorAddr)
{
   u32 addr;
	if(SPIFlash_Status&PAGESIZE)
	{
		addr = SectorAddr&0x001FFE00;
	}
	else
	{
		addr = (SectorAddr<<1)&0x003FFC00;
	}

    DEF_SPI_FLASH_CS_LOW;	                 // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , CMD_PAGE_ERASE);	 // 发送指令
	SPI_ReadWriteByte(SPI1 , (addr>>16)&0xff);
	SPI_ReadWriteByte(SPI1 , (addr>>8)&0xff);	
	SPI_ReadWriteByte(SPI1 , 0);		
	DEF_SPI_FLASH_CS_HIGH;		         // 禁止芯片操作
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SPI_FLASH_BulkErase(void)
{
	DEF_SPI_FLASH_CS_LOW;	                 // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , CMD_CHIP_ERASE);	 // 发送指令
	SPI_ReadWriteByte(SPI1 , 0x94);
	SPI_ReadWriteByte(SPI1 , 0x80);	
	SPI_ReadWriteByte(SPI1 , 0x9A);		
	DEF_SPI_FLASH_CS_HIGH;		     // 禁止芯片操作

}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u32 flag, PageID;
	u8 CmdBuf[4];
	u16 i;
	u32 addr;
	
	flag = SPIFlash_BusyWait();
	if (flag == 1)
	{
		goto SPIFLASH_WRITE_EXIT;
	}		
	
	PageID = WriteAddr/512;
	if(SPIFlash_Status&PAGESIZE)
	{
		PageID <<= 9;
	}
	else
	{
		PageID <<= 10;
	}

	CmdBuf[0] = CMD_PAGE_TOBUF1;
 	CmdBuf[1] = (PageID>>16)&0xFF;
	CmdBuf[2] = (PageID>>8)&0xFF;
	CmdBuf[3] = PageID&0xFF;

	DEF_SPI_FLASH_CS_LOW;	               // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , CmdBuf[0]);      // 发送指令
	SPI_ReadWriteByte(SPI1 , CmdBuf[1]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[2]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[3]);	
	DEF_SPI_FLASH_CS_HIGH;
	
	if((SPIFlash_Status&PAGESIZE)==0)
	{
		addr = ((WriteAddr&0x001FFE00)<<1) + (WriteAddr&0x000001FF);
	}
	else
	{
		addr = WriteAddr;
	}

	CmdBuf[0] = CMD_PAGEBUF1_PROG;
 	CmdBuf[1] = (addr>>16)&0xFF;
	CmdBuf[2] = (addr>>8)&0xFF;
	CmdBuf[3] = addr&0xFF;
	
	flag = SPIFlash_BusyWait();
	if (flag == 1)
	{
		goto SPIFLASH_WRITE_EXIT;
	}	

	DEF_SPI_FLASH_CS_LOW;	               // 使能芯片操作
	
	SPI_ReadWriteByte(SPI1 , CmdBuf[0]);      // 发送指令
	SPI_ReadWriteByte(SPI1 , CmdBuf[1]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[2]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[3]);     
	
	for(i=0; i<NumByteToWrite; i++)
	{
		SPI_ReadWriteByte(SPI1 , *pBuffer++);
	}

	DEF_SPI_FLASH_CS_HIGH;	

	CmdBuf[0] = CMD_BUF1PAGE_REWRITE;
 	CmdBuf[1] = (PageID>>16)&0xFF;
	CmdBuf[2] = (PageID>>8)&0xFF;
	CmdBuf[3] = PageID&0xFF;

	flag = SPIFlash_BusyWait();
	if (flag == 1)
	{
		goto SPIFLASH_WRITE_EXIT;
	}

	DEF_SPI_FLASH_CS_LOW;	               // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , CmdBuf[0]);     // 发送指令
	SPI_ReadWriteByte(SPI1 , CmdBuf[1]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[2]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[3]);     
	DEF_SPI_FLASH_CS_HIGH;

SPIFLASH_WRITE_EXIT:	
	return; 
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        BSP_SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}
void BSP_SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u8 CmdBuf[4], flag;
	u16 i;


    flag = SPIFlash_BusyWait();
    if (flag == 1)
	{
		return;
	}

	if(SPIFlash_Status&PAGESIZE)		  // 512字节
	{
		CmdBuf[0] = CMD_ARRAY_READ_L;              // 装在命令
		CmdBuf[1] = (ReadAddr>>16)&0xFF;
		CmdBuf[2] = (ReadAddr>>8)&0xFF;
		CmdBuf[3] = ReadAddr&0xFF;
	}
	else					          // 528字节
	{
		CmdBuf[0] = CMD_ARRAY_READ_L;              // 装在命令
		CmdBuf[1] = (ReadAddr>>15)&0xFF;
		CmdBuf[2] = ((ReadAddr>>8)&0x01) + ((ReadAddr>>7)&0xFC);
		CmdBuf[3] = ReadAddr&0xFF;
	} 
	//CmdBuf[4] = 0;

	DEF_SPI_FLASH_CS_LOW;	               // 使能芯片操作

	SPI_ReadWriteByte(SPI1 , CmdBuf[0]);      // 发送指令
	SPI_ReadWriteByte(SPI1 , CmdBuf[1]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[2]);
	SPI_ReadWriteByte(SPI1 , CmdBuf[3]); 

	for(i=0; i<NumByteToRead; i++)
	{
		*pBuffer++ = SPI_ReadWriteByte(SPI1 , 0xff);
	}
	DEF_SPI_FLASH_CS_HIGH;		           // 禁止芯片操作

	return;

}
u32 BSP_SPI_FLASH_ReadID(void)
{
	u8 CmdBuf[4];
	u32 rst;

	CmdBuf[0] = CMD_ID_READ;
	DEF_SPI_FLASH_CS_LOW;	              // 使能芯片操作
	SPI_ReadWriteByte(SPI1 , CMD_ID_READ);  // 发送指令
	CmdBuf[0] = SPI_ReadWriteByte(SPI1 , 0xff);
	CmdBuf[1] = SPI_ReadWriteByte(SPI1 , 0xff);
	CmdBuf[2] = SPI_ReadWriteByte(SPI1 , 0xff);
	CmdBuf[3] = SPI_ReadWriteByte(SPI1 , 0xff);	 	
	DEF_SPI_FLASH_CS_HIGH;              // 禁止芯片操作
	rst = ((u32)CmdBuf[0]<<24) + ((u32)CmdBuf[1]<<16) + ((u32)CmdBuf[2]<<8) + CmdBuf[0];
	
	return rst;	  
}



