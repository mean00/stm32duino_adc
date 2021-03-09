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
        simpleAdc(int pin);
        
        ~simpleAdc();
        
        float getVcc();
       
        void clearSamples();
       
        
        /* Set the input pin in single pin mode*/
        bool changePin(int newPin);
        
        /**
         *  \fn setPins
         * \brief set the pins to capture from. This is using a single ADC,
         * sampling each pins in sequence
         * /!\ this only works with timeSample (and MAYBE sample) but definitively not with the dual ones
         */
        bool setPins(int nb, int *pins);
        /**
         * Acquire nb samples using ADC rate/scale as timebase
         * nb : in/out, ,nb samples asked, nb samples available
         */
        
        bool sample(int &nb, uint16_t **data,adc_smp_rate rate,DSOADC::Prescaler scale);
        /**
         * Acquire nb samples using ADC rate/scale as timebase, using both ADCS
         * @param otherPin
         * @param nb
         * @param data
         * @param rate
         * @param scale
         * @return 
         */
        bool dualSample(int otherPin, int &nb, uint16_t **data,adc_smp_rate rate,DSOADC::Prescaler scale);
        
        /**
         * \brief acquire nb samples using a timer at frequency frequency as time base
         * \brief : Warning this is using Timer4 !, dont use timer4 for PWM or other
         * @param nb
         * @param data
         * @param frequency
         * @return 
         */
        bool timeSample(int &nb, uint16_t **data,int frequency);
        /**
         * \brief do a dual ADC capture using a timer at frequency hz as timebase
         * * \brief : Warning this is using Timer4 !, dont use timer4 for PWM or other
         * @param otherPin
         * @param nb
         * @param data
         * @param frequency
         * @return 
         */
        bool dualTimeSample(int otherPin, int &nb, uint16_t **data,int frequency);
        
        /**
            \brief returns ADC 1 Registers. Regs must be at least 16 entries
         * \brief returns # of entries
         * 
         */
        int getRegisters(uint32_t *regs){ return DSOADC::getRegisters(regs);}
       
protected:
        DSOADC *proxy;
        float vcc;
};