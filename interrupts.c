#include <msp430.h>
#include <driverlib.h>
#include "Board.h"

#define XT1_CRYSTAL_FREQUENCY_IN_HZ 32768 // Replace with the XT1 crystal frequency in Hz
#define MCLK_DESIRED_FREQUENCY_IN_KHZ 16000 // Replace with your desired MCLK frequency in KHz
#define REFOCLK_FREQUENCY 32768
#define MCLK_FLLREF_RATIO (MCLK_DESIRED_FREQUENCY_IN_KHZ / (REFOCLK_FREQUENCY / 1024))
#define ONE_SEC 16000000



uint32_t myACLK = 0;
uint32_t mySMCLK = 0;
uint32_t myMCLK = 0;

void initClocks(void) {


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

       GPIO_setAsOutputPin(
              GPIO_PORT_LED2,
              GPIO_PIN_LED2
              );

       // Configure Port 1, Pin 3 as an output
       GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

       GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);


       PMM_unlockLPM5();

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


void initGPIOInterrupt(void){


     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN2 );

     GPIO_selectInterruptEdge ( GPIO_PORT_P1, GPIO_PIN2, GPIO_LOW_TO_HIGH_TRANSITION);

    GPIO_clearInterrupt ( GPIO_PORT_P1, GPIO_PIN2 );

    GPIO_enableInterrupt ( GPIO_PORT_P1, GPIO_PIN2 );


}

#pragma vector = PORT1_VECTOR

__interrupt void myisr(void){

    GPIO_setOutputHighOnPin(
                GPIO_PORT_P1,
                GPIO_PIN0
                );

  // Clear the interrupt flag for Pin 2
       GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN2);   
}







int main(void)
{
    WDT_A_hold(WDT_A_BASE);

    initClocks();

    initGPIOInterrupt();

   __bis_SR_register(GIE);

    while(1){

        GPIO_toggleOutputOnPin(
              GPIO_PORT_LED2,
              GPIO_PIN_LED2);

        //Delay
         __delay_cycles(16000000);

    }
	
}
