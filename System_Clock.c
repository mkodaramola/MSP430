
#include <driverlib.h>
#include "myClocks.h"
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

    // Initialize the XT1 and XT2 crystal frequencies being used
    // so driverlib knows how fast they are


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


       /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
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



int main(void){


    WDT_A_hold(WDT_A_BASE);

    initClocks();


    while(1){

        //Toggle LED2
              GPIO_toggleOutputOnPin(
                  GPIO_PORT_LED2,
                  GPIO_PIN_LED2
                  );


              __delay_cycles(ONE_SEC);

    }

}







/*
 * myClocks.h
 *
 *  Created on: Oct 9, 2023
 *      Author: user
 */

#ifndef MYCLOCKS_H_
#define MYCLOCKS_H_


void initClocks(void);


#endif /* MYCLOCKS_H_ */
