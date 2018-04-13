#include "brdClock.h"
#include "brdLed.h"
#include "brdUtils.h"
#include "brdSPI.h"
#include "brdDMA.h"

//  Определения для отладочной платы выбранной в "brdSelect.h"
#include "brdDef.h"
//  Выбор SPI и настроек - модифицируется под реализацию
#include "brdSPI_Select.h"
//  Выбор настроек DMA - модифицируется под реализацию
#include "brdDMA_Select.h"

#define LED_OK    BRD_LED_1
#define LED_ERR   BRD_LED_2

#define DELAY_TICKS     4000000
#define SPI_MASTER_MODE 1

#define DMA_CHANNEL  DMA_Channel_SSP2_RX

#define  DATA_COUNT  10
uint32_t DestBuf[DATA_COUNT];
uint32_t SrcBuf[DATA_COUNT];

uint32_t DMA_Completed = 0;

//----- Выбор варианта работы с DMA -----
//  В текущем варианте в прерывании инициализируется новый цикл DMA
//  при DMA_USE_CASE_DIS обработки идет через отключение источников прерывания

//#define DMA_USE_CASE_DIS

//---------------------------------------


int main(void)
{
  uint32_t i;
  uint32_t result;
  
  //  Clock
  BRD_Clock_Init_HSE_PLL(RST_CLK_CPU_PLLmul10); // 80MHz
  
  //  Controls
  BRD_LEDs_Init();
  
  //  SPI Init
 // BRD_SPI_PortInit(pBRD_SPIx);  // В тестовом режиме SPI выводы не используются, в реальном включении - раскомментировать!
  BRD_SPI_Init(pBRD_SPIx, SPI_MASTER_MODE);  

  //  DMA Init
  DMA_DataCtrl_Pri.DMA_SourceBaseAddr = (uint32_t)&pBRD_SPIx->SPIx->DR;
  DMA_DataCtrl_Pri.DMA_DestBaseAddr   = (uint32_t)&DestBuf;
  DMA_DataCtrl_Pri.DMA_CycleSize      = DATA_COUNT;
  
  BRD_DMA_Init(DMA_CHANNEL, &DMA_ChanCtrl);
  
  //  SSP DMA Enable
  SSP_DMACmd(pBRD_SPIx->SPIx, SSP_DMA_RXE, ENABLE);
   
  // Write and read Ack
  while (1)
  {  
    BRD_LED_Set(LED_ERR, 0);
    
    // Prepare Data
    for (i = 0; i < DATA_COUNT; i++)
    {
      DestBuf[i] = 0;
      SrcBuf[i]  = i + 1; //  Fill with any data
    }
    
    //  Run SPI Cycles
    for (i = 0; i < DATA_COUNT; i++)
    {
      BRD_SPI_Master_WR(pBRD_SPIx, SrcBuf[i]);
    }

    //  Wait DMA Completed
    while (!DMA_Completed);
    
    //  Check Data
    result = 1;
    for (i = 0; i < DATA_COUNT; i++)
      if (DestBuf[i] != SrcBuf[i])
        result = 0;
      
    if (result)  
      BRD_LED_Switch(LED_OK);
    else
      BRD_LED_Set(LED_ERR, 1);      
    
    //  DMA Start
    DMA_Completed = 0;

#ifdef DMA_USE_CASE_DIS
    //  Next DMA Cycle Init
    DMA_Init(DMA_CHANNEL, &DMA_ChanCtrl);
    DMA_Cmd(DMA_CHANNEL, ENABLE);
    SSP_DMACmd(pBRD_SPIx->SPIx, SSP_DMA_RXE, ENABLE);
#endif    
    
    //  Wait for Led show
    Delay(DELAY_TICKS);
  }
}

void DMA_IRQHandler (void)
{
 
#ifdef DMA_USE_CASE_DIS
  // DMA Disable
  SSP_DMACmd(pBRD_SPIx->SPIx, SSP_DMA_RXE, DISABLE);
  DMA_Cmd(DMA_CHANNEL, DISABLE);
#else  
  //  Next DMA Cycle Init
  DMA_Init(DMA_CHANNEL, &DMA_ChanCtrl);  
#endif  
  
  DMA_Completed = 1;
  NVIC_ClearPendingIRQ (DMA_IRQn);
}	


