/*
 * FMConfig.cpp
 *
 *  Created on: Nov 8, 2023
 *      Author: Daramola Oluwafemi
 */
#include <msp430.h>
#include <driverlib.h>
#include "Board.h"

uint32_t myACLK = 0;
uint32_t mySMCLK = 0;
uint32_t myMCLK = 0;



void initClocks(const uint16_t clk) {

    // Initialize the XT1 and XT2 crystal frequencies being used
    // so driverlib knows how fast they are

    const uint32_t XT1_CRYSTAL_FREQUENCY_IN_HZ = 32768;
    const uint32_t MCLK_DESIRED_FREQUENCY_IN_KHZ = clk;
    const uint32_t REFOCLK_FREQUENCY = 32768;
     int  MCLK_FLLREF_RATIO = (MCLK_DESIRED_FREQUENCY_IN_KHZ / (REFOCLK_FREQUENCY / 1024));

    //ACLK set out to pins
       GPIO_setAsPeripheralModuleFunctionOutputPin(
           GPIO_PORT_ACLK,
           GPIO_PIN_ACLK,
           GPIO_FUNCTION_ACLK
           );

       //SMCLK set out to pins
       GPIO_setAsPeripheralModuleFunctionOutputPin(
           GPIO_PORT_SMCLK,
           GPIO_PIN_SMCLK,
           GPIO_FUNCTION_SMCLK
           );
       //MCLK set out to pins
       GPIO_setAsPeripheralModuleFunctionOutputPin(
           GPIO_PORT_MCLK,
           GPIO_PIN_MCLK,
           GPIO_FUNCTION_MCLK
           );



  CS_setExternalClockSource(XT1_CRYSTAL_FREQUENCY_IN_HZ);
  // Verify if the default clock settings are as expected



  myACLK = CS_getACLK();
  mySMCLK = CS_getSMCLK();
  myMCLK = CS_getMCLK();


  // Set the FLL's clock reference clock source
  CS_initClockSignal(
    CS_FLLREF,              // Clock you're configuring
    CS_REFOCLK_SELECT,      // Clock source (REFO)
    CS_CLOCK_DIVIDER_1     // Divide down clock source
  );


  // Setup ACLK to use REFO as its oscillator source
  CS_initClockSignal(
    CS_ACLK,                // Clock you're configuring
    CS_REFOCLK_SELECT,      // Clock source (REFO)
    CS_CLOCK_DIVIDER_1     // Divide down clock source
  );



  // Configure the FLL's frequency and set MCLK & SMCLK to use the FLL
  CS_initFLLSettle(
    MCLK_DESIRED_FREQUENCY_IN_KHZ, // MCLK frequency
    MCLK_FLLREF_RATIO              // Ratio between MCLK and FLL's ref clock source
  );



  // Verify that the modified clock settings are as expected
  myACLK = CS_getACLK();
  mySMCLK = CS_getSMCLK();
  myMCLK = CS_getMCLK();


}






void initTimerUpMode(const uint16_t ccr0){



    Timer_A_initUpModeParam initUpParam = { 0 };
     initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
     initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
     initUpParam.timerPeriod = ccr0; // (calculated in previous question)
     initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
     initUpParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE; // Enable CCR0 compare interrupt
     initUpParam.timerClear = TIMER_A_DO_CLEAR;
     initUpParam.startTimer = false;

     Timer_A_initUpMode(TIMER_A1_BASE, &initUpParam);

     Timer_A_clearTimerInterrupt(TIMER_A1_BASE);

     Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

     Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_UP_MODE);


    // Enter LPM0, enable interrupts
    __bis_SR_register(GIE);

    // For debugger
    __no_operation();


}


void PWMoutput(const uint8_t port,const uint8_t pin,const uint32_t period, uint32_t dutyCycle){

    //P4.0 as PWM output
       GPIO_setAsPeripheralModuleFunctionOutputPin(
               port,
               pin,
           GPIO_PRIMARY_MODULE_FUNCTION
           );

       //Generate PWM - Timer runs in Up-Down mode
       Timer_A_outputPWMParam param = {0};
       param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
       param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
       param.timerPeriod = period;
       param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
       param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
       param.dutyCycle = dutyCycle;
       Timer_A_outputPWM(TIMER_A1_BASE, &param);

       //Enter LPM0
       __bis_SR_register(LPM0_bits);

       //For debugger
       __no_operation();


}


typedef struct {
    uint16_t ms;   // Milliseconds
    uint16_t ms2;     // Seconds
    uint16_t ms3;  // Minutes
} ElapsedTime;

static ElapsedTime elapsedTime = {0, 0, 0};


delay( uint8_t n,void (*toggleFunction)(uint8_t, uint8_t),uint8_t port, uint8_t pin, int msec ){

    if (n == 0) {

        elapsedTime.ms += 1;

               if (elapsedTime.ms >= msec) {
                   toggleFunction(port, pin);
                   elapsedTime.ms -= msec;  // Reset milliseconds
               }


    }

    else if (n == 1) {

        elapsedTime.ms2 += 1;

               if (elapsedTime.ms2 >= msec) {
                   toggleFunction(port, pin);
                   elapsedTime.ms2 -= msec;  // Reset milliseconds
               }


    }

    else if (n == 2) {

         elapsedTime.ms3 += 1;

                if (elapsedTime.ms3 >= msec) {
                    toggleFunction(port, pin);
                    elapsedTime.ms3 -= msec;  // Reset milliseconds
                }


     }



 }
