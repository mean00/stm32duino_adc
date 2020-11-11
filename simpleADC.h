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
/**
 * 
 * @param pin
 */
class simpleAdc
{
public:
        simpleAdc(int pin)
        {
            proxy=new DSOADC(pin);
            proxy->setupADCs ();  
            vcc=proxy->readVCCmv();
            proxy->clearSamples();
            proxy->setADCPin(pin);
        }
        float getVcc()
        {
            return vcc;
        }
        void clearSamples()
        {
            proxy->clearSamples();
        }
        ~simpleAdc()
        {
            delete proxy;
            proxy=NULL;
        }
        bool changePin(int newPin)
        {
            return proxy->setADCPin(newPin);
        }
        /**
         * Acquire nb samples using ADC rate/scale as timebase
         * nb : in/out, ,nb samples asked, nb samples available
         */
        
        bool sample(int nb, uint16_t **data,adc_smp_rate rate,DSOADC::Prescaler scale)
        {
            proxy->setupDmaSampling();
            if(!proxy->prepareDMASampling(rate,scale)) return false;
            proxy->startDMASampling(nb);
            if(!proxy->getSamples(data,nb)) return false;
            return true;
        }
        /**
         * \brief acquire nb samples using a timer at frequency frequency as time base
         * @param nb
         * @param data
         * @param frequency
         * @return 
         */
        bool timeSample(int nb, uint16_t **data,int frequency)
        {
            DSOADC::Prescaler scaler;
            adc_smp_rate rate;
            xAssert(proxy->frequencyToRateScale (frequency, scaler,rate));
            proxy->setupTimerSampling();
            //if(!proxy->prepareTimerSampling(frequency, false, ADC_SMPR_28_5 , DSOADC::ADC_PRESCALER_4)) return false;                       
            if(!proxy->prepareTimerSampling(frequency, false, rate,scaler)) return false;
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
        bool dualTimeSample(int otherPin, int nb, uint16_t **data,int frequency)
        {
            DSOADC::Prescaler scaler;
            adc_smp_rate rate;
            xAssert(proxy->frequencyToRateScale (frequency, scaler,rate));
            proxy->setupDualTimerSampling();
            if(!proxy-> prepareDualTimeSampling (frequency,otherPin, rate,scaler)) return false;
            proxy->startDualTimeSampling(otherPin,frequency,0);
            if(!proxy->getSamples(data,nb)) return false;
            return true;
        }
protected:
        DSOADC *proxy;
        float vcc;
};