
#include "simpleADC.h"

/**
 *  \file simpleADC.h
 * \brief this is a simplified API version of the ADC one. it is slightly slower but
 * most of the time it does not matter
 * The calls are blocking, but it is spent waiting on a semaphore, other tasks can 
 * do something else meanwhile
 */
#pragma once
#include "dso_adc.h"
#include "MapleFreeRTOS1000_pp.h"

#define SKIP_N_SAMPLES 4            

/**
 * Acquire nb samples using ADC rate/scale as timebase
 * nb : in/out, ,nb samples asked, nb samples available
 */

bool simpleAdc::sample(int &nb, uint16_t **data,adc_smp_rate rate,DSOADC::Prescaler scale)
{
    proxy->setupDmaSampling();
    if(!proxy->prepareDMASampling(rate,scale)) return false;
    proxy->startDMASampling(nb+1);
    if(!proxy->getSamples(data,nb)) return false;
    (*data)++;
    if(nb) nb--;
    return true;
}
/**
 * 
 * @param otherPin
 * @param nb
 * @param data
 * @param rate
 * @param scale
 * @return 
 */
bool simpleAdc::dualSample(int otherPin, int &nb, uint16_t **data,adc_smp_rate rate,DSOADC::Prescaler scale)
{
    proxy->setupTimerSampling();
    if(!proxy->prepareFastDualDMASampling (otherPin,rate,scale))
        return false;
    proxy->startDualDMASampling(otherPin,nb+SKIP_N_SAMPLES);
    if(!proxy->getSamples(data,nb)) 
        return false;
    // skip the first samples, they are wrong
    if(nb>SKIP_N_SAMPLES) 
    {
        nb=nb-SKIP_N_SAMPLES;
        *data=*data+SKIP_N_SAMPLES;
    }
    return true;
} 


/**
 * \brief acquire nb samples using a timer at frequency frequency as time base
 * @param nb
 * @param data
 * @param frequency
 * @return 
 */
bool simpleAdc::timeSample(int &nb, uint16_t **data,int frequency)
{
    DSOADC::Prescaler scaler;
    adc_smp_rate rate;
    xAssert(proxy->frequencyToRateScale (frequency, scaler,rate));
    proxy->setupTimerSampling();
    //if(!proxy->prepareTimerSampling(frequency, false, ADC_SMPR_28_5 , DSOADC::ADC_PRESCALER_4)) return false;                       
    if(!proxy->prepareTimerSampling(frequency, 1, rate,scaler)) return false;
    proxy->startTimerSampling(nb);
    if(!proxy->getSamples(data,nb)) return false;
    return true;
}
/**
 * \brief do a dual ADC capture using a timer at frequency hz as timebase
 * @param otherPin
 * @param nb
 * @param data
 * @param frequency
 * @return 
 */
bool simpleAdc::dualTimeSample(int otherPin, int &nb, uint16_t **data,int frequency)
{

    DSOADC::Prescaler scaler;
    adc_smp_rate rate;
    xAssert(proxy->frequencyToRateScale (frequency, scaler,rate));
    proxy->setupDualTimerSampling();
    if(!proxy-> prepareDualTimeSampling (frequency,otherPin, rate,scaler))
        return false;
    proxy->startDualTimeSampling(otherPin,nb+SKIP_N_SAMPLES,0);
    if(!proxy->getSamples(data,nb)) 
        return false;
    // skip the first samples, they are wrong
    if(nb>SKIP_N_SAMPLES) 
    {
        nb=nb-SKIP_N_SAMPLES;
        *data=*data+SKIP_N_SAMPLES;
    }
    return true;
}
/**
 * 
 * @param pin
 */
 simpleAdc::simpleAdc(int pin)
{
    proxy=new DSOADC(pin);
    proxy->setupADCs ();  
    vcc=proxy->getVCCmv();
    proxy->clearSamples();
    proxy->setADCPin(pin);
}
 /**
  * 
  * @return 
  */
float simpleAdc::getVcc()
{
    return vcc;
}
/**
 * 
 */
void simpleAdc::clearSamples()
{
    proxy->clearSamples();
}
/**
 * 
 */
simpleAdc::~simpleAdc()
{
    delete proxy;
    proxy=NULL;
}
/**
 * 
 * @param newPin
 * @return 
 */
bool simpleAdc::changePin(int newPin)
{
    return proxy->setADCPin(newPin);
}
/**
 * 
 * @param nb
 * @param pins
 * @return 
 */
bool simpleAdc::setPins(int nb, int *pins)
{
      return proxy->setADCPins(nb,pins);
}

// EOF