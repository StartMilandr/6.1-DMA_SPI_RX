#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
//#include <MDR32F9Qx_dma.h>

#include "brdDMA.h"

// ������������� ������ DMA
void BRD_DMA_Init (uint32_t DMA_Channel, DMA_ChannelInitTypeDef*  pDMAChannelInitStruct)
{
  // ��������� ������������ ������ DMA
  // ��� ���������� ������ DMA ����� ���������� �������� ������������ ������ SSP1 � SSP2
  // (���� ����� �� �������, �� ��������� ����� ��������� ���������� �� DMA)
  RST_CLK_PCLKcmd (RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2, ENABLE);

  // ����� ���������� �� DMA
  NVIC_ClearPendingIRQ (DMA_IRQn);

  // ��������������� DMA
  DMA_DeInit();	
 
  // ������������� ������ DMA,
	DMA_Init(DMA_Channel, pDMAChannelInitStruct);

  // ���������� ���������� ���������� ���������� �� DMA
  NVIC_SetPriority (DMA_IRQn, 1);

  // ��������� ���������� ���������� �� DMA
  NVIC_EnableIRQ (DMA_IRQn); 
	
  DMA_Cmd(DMA_Channel, ENABLE);
}

//// �������� - ���������� ���������� �� DMA
//void DMA_IRQHandler (void)
//{
//  // ������������ ������ ����� �������-�������� ��������������
//  DMA_Init (DMA_Channel, &DMAChannelInitStruct);
//}	


