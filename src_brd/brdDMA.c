#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
//#include <MDR32F9Qx_dma.h>

#include "brdDMA.h"

// Инициализация модуля DMA
void BRD_DMA_Init (uint32_t DMA_Channel, DMA_ChannelInitTypeDef*  pDMAChannelInitStruct)
{
  // Включение тактирования модуля DMA
  // Для корректной работы DMA также необходимо включить тактирование портов SSP1 и SSP2
  // (если этого не сделать, то постоянно будут возникать прерывания от DMA)
  RST_CLK_PCLKcmd (RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2, ENABLE);

  // Сброс прерывания от DMA
  NVIC_ClearPendingIRQ (DMA_IRQn);

  // Деинициализация DMA
  DMA_DeInit();	
 
  // Инициализация канала DMA,
	DMA_Init(DMA_Channel, pDMAChannelInitStruct);

  // Назначение приоритета аппаратных прерываний от DMA
  NVIC_SetPriority (DMA_IRQn, 1);

  // Включение аппаратных прерываний от DMA
  NVIC_EnableIRQ (DMA_IRQn); 
	
  DMA_Cmd(DMA_Channel, ENABLE);
}

//// Прототип - Обработчик прерываний от DMA
//void DMA_IRQHandler (void)
//{
//  // Формирование нового цикла аналого-цифровых преобразований
//  DMA_Init (DMA_Channel, &DMAChannelInitStruct);
//}	


