/***************************************************
 Battery Tester
 * GPL v2
 * (c) mean 2018 fixounet@free.fr
 ****************************************************/

#include <Wire.h>
#include "SPI.h"
#include "MapleFreeRTOS1000_pp.h"
#include "adc.h"
#include "dso_adc.h"

#define mySetup setup
#define myLoop loop

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];


char st[60];
/*
 */
void myLoop(void) ;
/**
 * 
 */
class MainTask : public xTask
{
public:
            MainTask() : xTask("MainTask",10,400)
            {

            }
            void    run(void);
            void    loop(void) ;
            float   analogLoop(int pin);
            float   dmaLoop(int pin);
            float   timeLoop(int pin);
protected:
            DSOADC               *adc;
            float                vcc;
};

/**
 * 
 */
void mySetup() 
{
  afio_cfg_debug_ports( AFIO_DEBUG_SW_ONLY); // get PB3 & PB4
  // switch to uart ASAP    
  Serial.end();
  Serial1.begin(115200);  //Wire.begin();
  Serial.end();
  Serial1.begin(115200);  
    
  // Start freeRTOS
  MainTask *mainTask=new MainTask();
  vTaskStartScheduler();        
}
/**
 * 
 * @param pin
 */
float MainTask::dmaLoop(int pin)
{   
    adc->setupDmaSampling();     
    float volt;
        adc->prepareDMASampling(ADC_SMPR_28_5, DSOADC::ADC_PRESCALER_4);
        adc->startDMASampling(16);
        uint16_t *samples;
        int nb;

        if(adc->getSamples(&samples,nb))
        {
          int sum=0;
          for(int i=0;i<nb;i++) 
              sum+=samples[i];
          sum=(sum+nb/2)/nb;        
          volt=((float)sum)*vcc/4095.;
          volt*=2./1000.;
         
          return volt;
        }
        return -1;
}

/**
 * 
 * @param pin
 */
float MainTask::timeLoop(int pin)
{    
    float volt;    
    adc->setupTimerSampling();

        adc->prepareTimerSampling (5000,false,ADC_SMPR_28_5 , DSOADC::ADC_PRESCALER_4);
        adc->startTimerSampling(16);
        uint16_t *samples;
        int nb;

        if(adc->getSamples(&samples,nb))
        {
          int sum=0;
          for(int i=0;i<nb;i++) 
              sum+=samples[i];
          sum=(sum+nb/2)/nb;        
          volt=((float)sum)*vcc/4095.;
          volt*=2./1000.;
          return volt;
        }
        return -1;
}
/**
 * 
 * @param pin
 */
float MainTask::analogLoop(int pin)
{
    float volt;
    float f=0;
    for(int i=0;i<16;i++)
        f+=analogRead(pin);
    f=(f+9.)/16.;
    volt=2*f*vcc/4095.;
    volt=volt/1000.;
    return volt;
}
/**
 * 
 */

#define PWM_PIN PB0
#define ADC_VOLT_PIN PA3
void    MainTask::run(void)
{  
  char s[200];

    pinMode(ADC_VOLT_PIN,INPUT_FLOATING);

    adc=new DSOADC(ADC_VOLT_PIN);    
    adc->setupADCs ();
  
    vcc=adc->readVCCmv();
    float volt;
    
    
    adc->clearSamples();
    adc->setADCPin(ADC_VOLT_PIN);

    // do a dummy one to setup things
    timeLoop(ADC_VOLT_PIN);    // OFFset    
    timeLoop(ADC_VOLT_PIN);    // OFFset    
    timeLoop(ADC_VOLT_PIN);    // OFFset    
    
    float vd,va,vt;
    while(1)
    {    
      va=analogLoop(ADC_VOLT_PIN);   // OFFSET
      vd=dmaLoop(ADC_VOLT_PIN);     // OK
      vt=timeLoop(ADC_VOLT_PIN);    // OFFset    

      sprintf(s,"Analog: %d dmaLoop: %d timeLoop : %d\n",(int)(va*1000),(int)(vd*1000), (int)(vt*1000));
      Serial1.print(s);
      xDelay(1000);
    }

           
           
}
   
/**
 * 
 */
void MainTask::loop(void) 
{
       xDelay(50);
}
/**
 * 
 */
void myLoop()
{
    
}

//--EOF
