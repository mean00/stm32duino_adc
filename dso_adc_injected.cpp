
#include "dso_adc.h"
#include "dso_adc_priv.h"
#include "fancyLock.h"
#include "dma.h"
#include "adc.h"

extern void dumpAdcRegs();
/**
 * 
 * @param pin
 * @return 
 */
extern void Logger(const char *fmt...);

/**
 * 
 * @param adc
 * @param channel
 * @return 
 */

 uint16_t directADCRead(adc_dev *adc, int channel)
 {

    volatile adc_reg_map *regs=  adc->regs;
    
    noInterrupts();
    regs->SR &=~ ADC_SR_JEOC;
    regs->JOFR1=0;   // Offset
    regs->JSQR=channel<<15;   // 1 sample on that channel
    
    regs->CR1 &= ~ADC_CR1_JEOCIE;
    regs->CR2 |= 7<<12; // JEXTSEL=SW START
    regs->CR2 |= ADC_CR2_JEXTTRIG;
    regs->CR2 |= ADC_CR2_JSWSTART;
    while (1)
    {
        volatile uint32_t status=regs->SR;
        if(status & ADC_SR_JEOC) 
            break;
       // dumpAdcRegs();
    }
    
    uint16_t value= (uint16)(regs->JDR1 & 0x3FFF);
    regs->SR &=~ ADC_SR_JEOC;
    interrupts();
    
    //Logger("CPL=%d\n",value);
    
    return value;
 }
 /**
  * 
  * @param channel
  * @return 
  */
 uint16_t directADC1Read(int channel)
 {
     return directADCRead(ADC1,channel);    
 }
 /**
  * 
  * @param channel
  * @return 
  */
 uint16_t directADC2Read(int channel)
 {
     return directADCRead(ADC2,channel);    
 }
 
 // EOF
 