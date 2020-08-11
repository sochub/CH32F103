/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 
#include "debug.h"
#include "string.h"

/*****************************************
*@Note
  Winbond W25Qxx SPIFLASH	
pins:   
	CS   ！！ PA2
	DO   ！！ PA6(SPI1_MISO)
	WP   ！！ 3.3V
	DI   ！！ PA7(SPI1_MOSI)
  CLK  ！！ PA5(SPI1_SCK)
  HOLD ！！ 3.3V 
*******************************************/

/* Winbond SPIFalsh ID */
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

/* Winbond SPIFalsh Instruction List */
#define W25X_WriteEnable		  0x06 
#define W25X_WriteDisable		  0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			    0x03 
#define W25X_FastReadData		  0x0B 
#define W25X_FastReadDual		  0x3B 
#define W25X_PageProgram		  0x02 
#define W25X_BlockErase			  0xD8 
#define W25X_SectorErase		  0x20 
#define W25X_ChipErase			  0xC7 
#define W25X_PowerDown			  0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			    0xAB 
#define W25X_ManufactDeviceID	0x90   
#define W25X_JedecDeviceID		0x9F 

/* Global define */
#define	SPI_FLASH_CS   PAout(2)  

/* Global Variable */ 
u8 SPI_FLASH_BUF[4096];
const u8 TEXT_Buf[]={"CH32F103 SPI FLASH W25Qxx"};
#define SIZE sizeof(TEXT_Buf)

/*******************************************************************************
* Function Name  : SPI1_ReadWriteByte
* Description    : SPI1 read or write one byte.
* Input          : TxData: write one byte data.                                                                                    
* Return         : Read one byte data.
*******************************************************************************/  
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 i=0;		
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) 
	{
		i++;
		if(i>200)return 0;
	}	
	
	SPI_I2S_SendData(SPI1, TxData); 
	i=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		i++;
		if(i>200)return 0;
	}	  
	
	return SPI_I2S_ReceiveData(SPI1); 				    
}

/*******************************************************************************
* Function Name  : SPI_Flash_Init
* Description    : Configuring the SPI for operation flash.
* Input          : None                                                                                          
* Return         : None
*******************************************************************************/  
void SPI_Flash_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init( GPIOA, &GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(SPI1, &SPI_InitStructure); 
 
	SPI_Cmd(SPI1, ENABLE); 
}  

/*******************************************************************************
* Function Name  : SPI_Flash_ReadSR
* Description    : Read W25Qxx status register.
*       ！！BIT7  6   5   4   3   2   1   0
*       ！！SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
* Input          : None                                                                                          
* Return         : byte: status register value.
*******************************************************************************/  
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0; 
  
	SPI_FLASH_CS=0;                              
	SPI1_ReadWriteByte(W25X_ReadStatusReg);      
	byte=SPI1_ReadWriteByte(0Xff);           
	SPI_FLASH_CS=1;                              
	
	return byte;   
} 

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_SR
* Description    : Write W25Qxx status register.
* Input          : sr:status register value.                                                                                       
* Return         : None
*******************************************************************************/ 
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                              
	SPI1_ReadWriteByte(W25X_WriteStatusReg);     
	SPI1_ReadWriteByte(sr);                    
	SPI_FLASH_CS=1;                               	      
}  

/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : Wait flash free.
* Input          : None
* Return         : None 
*******************************************************************************/
void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01)==0x01);   
}  

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Enable
* Description    : Enable flash write.
* Input          : None                                                                                       
* Return         : None
*******************************************************************************/ 
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                              
  SPI1_ReadWriteByte(W25X_WriteEnable);        
	SPI_FLASH_CS=1;              	      
} 
 
/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Disable
* Description    : Disable flash write.
* Input          : None                                                                                      
* Return         : None
*******************************************************************************/
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                               
  SPI1_ReadWriteByte(W25X_WriteDisable);      
	SPI_FLASH_CS=1;                 	      
} 			    

/*******************************************************************************
* Function Name  : SPI_Flash_ReadID
* Description    : Read flash ID. 
* Input          : None                                                                                      
* Return         : Temp: FLASH ID. 
*******************************************************************************/
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	
	SPI_FLASH_CS=0;				    
	SPI1_ReadWriteByte(W25X_ManufactDeviceID);   
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;	
	
	return Temp;
}   		    

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : Erase one sector(4Kbyte).
* Input          : Dst_Addr:  0 ！！ 2047 
* Return         : None 
*******************************************************************************/
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
  SPI_FLASH_Write_Enable();                  
  SPI_Flash_Wait_Busy();   
  SPI_FLASH_CS=0;                             
  SPI1_ReadWriteByte(W25X_SectorErase);      
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));     
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                      	      
  SPI_Flash_Wait_Busy();   		
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Read
* Description    : Read data from flash.
* Input          : pBuffer: 
*                  ReadAddr:Initial address(24bit).
*                  size: Data length.
* Return         : None 
*******************************************************************************/
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 size)   
{ 
 	u16 i;   
	
	SPI_FLASH_CS=0;                              
  SPI1_ReadWriteByte(W25X_ReadData);           
  SPI1_ReadWriteByte((u8)((ReadAddr)>>16));    
  SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI1_ReadWriteByte((u8)ReadAddr);  
	
  for(i=0;i<size;i++)
	{ 
		pBuffer[i]=SPI1_ReadWriteByte(0XFF);       
  }
	
	SPI_FLASH_CS=1;                          	      
}  

/*******************************************************************************
* Function Name  : SPI_Flash_Write_Page
* Description    : Write data by one page.
* Input          : pBuffer:
*                  WriteAddr:Initial address(24bit).
*                  size:Data length.
* Return         : None 
*******************************************************************************/
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 size)
{
 	u16 i;
  
  SPI_FLASH_Write_Enable();                  
	SPI_FLASH_CS=0;                            
  SPI1_ReadWriteByte(W25X_PageProgram);       
  SPI1_ReadWriteByte((u8)((WriteAddr)>>16));   
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);
	
  for(i=0;i<size;i++)
	{
		SPI1_ReadWriteByte(pBuffer[i]);       
	}
	
	SPI_FLASH_CS=1;                          
	SPI_Flash_Wait_Busy();					 
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Write_NoCheck
* Description    : Write data to flash.(need Erase)
*                  All data in address rang is 0xFF.
* Input          : pBuffer:
*                  WriteAddr: Initial address(24bit).
*                  size: Data length.
* Return         : None 
*******************************************************************************/
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 size)   
{ 			 		 
	u16 pageremain;	 
  
	pageremain=256-WriteAddr%256; 
	
	if(size<=pageremain)pageremain=size;
	
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		
		if(size==pageremain)
		{
			break;
		}
	 	else 
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			size-=pageremain;	
		  
			if(size>256)pageremain=256; 
			else pageremain=size; 	  
		}
	}    
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Write
* Description    : Write data to flash.(no need Erase)
* Input          : pBuffer:
*                  WriteAddr: Initial address(24bit).
*                  size: Data length.
* Return         : None 
*******************************************************************************/
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 size)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;   

	if(size<=secremain)secremain=size;
	
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);
		
		for(i=0;i<secremain;i++)
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;   
		}
		
		if(i<secremain)
		{
			SPI_Flash_Erase_Sector(secpos);
			
			for(i=0;i<secremain;i++)
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);  

		}
		else{ 
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);				   
		}
		
		if(size==secremain){
			break;
		}
		else
		{
			secpos++;
			secoff=0;	 

		  pBuffer+=secremain;  
			WriteAddr+=secremain;	   
		  size-=secremain;		
			
			if(size>4096)
			{
				secremain=4096;	
			}
			else 
			{
				secremain=size; 
			}
		}	 
	}	 	 
}

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : Erase all FLASH pages. 
* Input          : None
* Return         : None 
*******************************************************************************/
void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();            
  SPI_Flash_Wait_Busy();   
  SPI_FLASH_CS=0;                       
  SPI1_ReadWriteByte(W25X_ChipErase);     
	SPI_FLASH_CS=1;                       	      
	SPI_Flash_Wait_Busy();   				   
}   

/*******************************************************************************
* Function Name  : SPI_Flash_PowerDown
* Description    : Enter power down mode.
* Input          : None
* Return         : None 
*******************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
  SPI_FLASH_CS=0;                               
  SPI1_ReadWriteByte(W25X_PowerDown);        
	SPI_FLASH_CS=1;                                	      
  Delay_Us(3);                          
}   

/*******************************************************************************
* Function Name  : SPI_Flash_WAKEUP
* Description    : Power down wake up.
* Input          : None
* Return         : None 
*******************************************************************************/
void SPI_Flash_WAKEUP(void)   
{  
	SPI_FLASH_CS=0;                              
	SPI1_ReadWriteByte(W25X_ReleasePowerDown);    
	SPI_FLASH_CS=1;                               	      
	Delay_Us(3);                   
}   

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{	
	u8 datap[SIZE];
	u16 Flash_Model;
	
	Delay_Init();
	USART_Printf_Init(9600);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	SPI_Flash_Init();  	
	
	Flash_Model = SPI_Flash_ReadID();
	
	switch(Flash_Model)
	{
		case W25Q80:
			printf("W25Q80 OK!\r\n");
		
			break;
		
		case W25Q16:
			printf("W25Q16 OK!\r\n");
		
			break;		
		
		case W25Q32:
			printf("W25Q32 OK!\r\n");
	
			break;	

		case W25Q64:
			printf("W25Q64 OK!\r\n");
		
			break;	

		case W25Q128:
			printf("W25Q128 OK!\r\n");
		
			break;			
		
		default:
			printf("Fail!\r\n");
		  
		  break;
		 
	}
	
	printf("Start Write W25Qxx....\r\n");   
	SPI_Flash_Write((u8*)TEXT_Buf,0,SIZE);
	printf("W25Qxx Write Finished!\r\n"); 	

	Delay_Ms(500);
	printf("Start Read W25Qxx....\r\n"); 			
	SPI_Flash_Read(datap,0x0,SIZE);		
	printf("%s\r\n", datap );

	Delay_Ms(500);
	printf("Start Erase W25Qxx....\r\n"); 			
	SPI_Flash_Erase_Sector(0);	
	printf("W25Qxx Erase Finished!\r\n");
	
	Delay_Ms(500);
	printf("Start Read W25Qxx....\r\n"); 			
	SPI_Flash_Read(datap,0x0,SIZE);		
	printf("%s\r\n", datap );

	while(1);
}




