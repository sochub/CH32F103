/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : Main program body.
*******************************************************************************/ 
#include "debug.h"

/* Global define */


/* Global Variable */    


/*******************************************************************************
* Function Name  : EXTI_INT_INIT
* Description    : Initializes EXTI0 collection.
* Input          : None
* Return         : None
*******************************************************************************/
void EXTI0_INT_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);	    
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  

  /* GPIOA.0 ----> EXTI_Line0 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			        
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  NVIC_Init(&NVIC_InitStructure);  
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_Init();
	USART_Printf_Init(115200);
	printf("Start @Chip_ID:%08x\r\n", DBGMCU->IDCODE );
	printf("Stop Mode Test\r\n");
	EXTI0_INT_INIT();

	printf("\r\n ********** \r\n");
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	printf("\r\n ########## \r\n");
	
	while(1)
  {	
   Delay_Ms(1000);
	 printf("Run in main\r\n");	
	}
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles EXTI0 Handler.
* Input          : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
  {
		SystemInit();
    printf("EXTI0 Wake_up\r\n");		
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */  
  }
}
