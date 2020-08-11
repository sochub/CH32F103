/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 
#include "debug.h"

/* Global define */
#define WWDG_CNT	0X7F

/* Global Variable */    

/*******************************************************************************
 * Function Name  : WWDG_NVIC_Config
 * Description    : WWDG INT init.
 * Input          : None
 * Return         : None
 *******************************************************************************/
static void WWDG_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/*******************************************************************************
* Function Name  : WWDG_Config
* Description    : Configure WWDG.
* Input          : tr : The value of the decrement counter(0x7f~0x40)
*                  wr : Window value(0x7f~0x40)
*                  prv: Prescaler value
*                       WWDG_Prescaler_1
*                       WWDG_Prescaler_2
*                       WWDG_Prescaler_4
*                       WWDG_Prescaler_8
* Return         : None
*******************************************************************************/
void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	WWDG_SetCounter( tr );
	WWDG_SetPrescaler( prv );
	WWDG_SetWindowValue( wr );
  WWDG_Enable(WWDG_CNT);	
	WWDG_ClearFlag();	
	WWDG_NVIC_Config();	
	WWDG_EnableIT();
}

/*******************************************************************************
* Function Name  : WWDG_Feed
* Description    : Feed WWDG.
* Input          : None
* Return         : None
*******************************************************************************/
void WWDG_Feed(void)
{
	WWDG_SetCounter( WWDG_CNT );
}


/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG Handler.
* Input          : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
  WWDG_ClearFlag();
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	u8 wwdg_tr,wwdg_wr;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("WWDG Test\r\n");
	WWDG_Config(0x7f,0x5f,WWDG_Prescaler_8);      /* 36M/8/4096 */
  wwdg_wr=WWDG->CFGR & 0x7F; 
	while(1)
  {	
    Delay_Ms(55);                              
		
	  printf("**********\r\n");
    wwdg_tr=WWDG->CTLR & 0x7F;	
		if(wwdg_tr<wwdg_wr)
		{
		  WWDG_Feed();
		}
		printf("##########\r\n");
	}
}




