#include "driverlib.h"
#include "Board.h"
#include <msp430.h>


void PWMoutput2(const uint8_t port,const uint8_t pin,const uint32_t period,const uint32_t dutyCycle){

       GPIO_setAsPeripheralModuleFunctionOutputPin(
               port,
               pin,
           GPIO_PRIMARY_MODULE_FUNCTION
           );

       PMM_unlockLPM5();

       //Generate PWM - Timer runs in Up-Down mode
       Timer_A_outputPWMParam param = {0};
       param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
       param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
       param.timerPeriod = period;
       param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
       param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
       param.dutyCycle = dutyCycle;
       Timer_A_outputPWM(TIMER_A0_BASE, &param);

       //For debugger
      __no_operation();

}





int main(void)
{

    //Stop WDT
       WDT_A_hold(WDT_A_BASE);


       PWMoutput2(GPIO_PORT_P1, GPIO_PIN7,524,500);




	return 0;
}
