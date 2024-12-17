#include <driverlib.h>
#include "Board.h"
#define TIMER_PERIOD 32768

#define XT1_CRYSTAL_FREQUENCY_IN_HZ 32768 // Replace with the XT1 crystal frequency in Hz
#define MCLK_DESIRED_FREQUENCY_IN_KHZ 16000 // Replace with your desired MCLK frequency in KHz
#define REFOCLK_FREQUENCY 32768
#define MCLK_FLLREF_RATIO (MCLK_DESIRED_FREQUENCY_IN_KHZ / (REFOCLK_FREQUENCY / 1024))
#define ONE_SEC 16000000


uint32_t myACLK = 0;
uint32_t mySMCLK = 0;
uint32_t myMCLK = 0;


typedef struct {
    uint16_t ms;   // Milliseconds
    uint16_t ms2;     // Seconds
    uint16_t ms3;  // Minutes
} ElapsedTime;

ElapsedTime elapsedTime = {0, 0, 0};

void delay( uint8_t n,void (*toggleFunction)(uint8_t, uint8_t),uint8_t port, uint8_t pin, int msec ){

   if (n == 1) {

       elapsedTime.ms += 1;  // Increment by 500 ms

              if (elapsedTime.ms >= msec) {
                  toggleFunction(port, pin);
                  elapsedTime.ms -= msec;  // Reset milliseconds
                          // Increment seconds
              }


   }

   else if (n == 2) {

       elapsedTime.ms2 += 1;  // Increment by 500 ms

              if (elapsedTime.ms2 >= msec) {
                  toggleFunction(port, pin);
                  elapsedTime.ms2 -= msec;  // Reset milliseconds
                          // Increment seconds
              }


   }

   else if (n == 3) {

        elapsedTime.ms3 += 1;  // Increment by 500 ms

               if (elapsedTime.ms3 >= msec) {
                   toggleFunction(port, pin);
                   elapsedTime.ms3 -= msec;  // Reset milliseconds
                           // Increment seconds
               }


    }



}


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




void initTimer(void){



       Timer_A_initUpModeParam initUpParam = { 0 };
        initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
        initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        initUpParam.timerPeriod = 32; // (calculated in previous question)
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



int main(void){


    WDT_A_hold(WDT_A_BASE);

    initClocks();

    PMM_unlockLPM5();


    initTimer();

    GPIO_setAsOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN7
                   );


    GPIO_setAsOutputPin(
               GPIO_PORT_P1,
               GPIO_PIN5
                      );

    while(1){


    }

}



#pragma vector = TIMER1_A0_VECTOR

__interrupt void ccr0_ISR(void){



    delay(1, GPIO_toggleOutputOnPin, GPIO_PORT_P1,GPIO_PIN5, 1000);

    delay(2, GPIO_toggleOutputOnPin, GPIO_PORT_P1,GPIO_PIN7, 2000);



}

#pragma vector = TIMER1_A1_VECTOR

__interrupt void ccr0_ISR2(void){

    switch (__even_in_range(TA1IV, TA1IV_TAIFG)){

    case TA1IV_NONE: break;
    case TA1IV_TACCR1: _no_operation(); break;
    case TA1IV_TACCR2: _no_operation(); break;
    case TA1IV_3: break;
    case TA1IV_4: break;
    case TA1IV_5: break;
    case TA1IV_6: break;
    case TA1IV_TAIFG:
        break;

    default: _never_executed();

    }
}


