#include "driverlib.h"
#include "Board.h"
#include <msp430.h>



void servo_write(const uint8_t port,const uint8_t pin,float angle){

       GPIO_setAsPeripheralModuleFunctionOutputPin(
               port,
               pin,
           GPIO_PRIMARY_MODULE_FUNCTION
           );

       PMM_unlockLPM5();

       if (angle < 0) angle = 0;
       else if (angle > 180) angle = 180;

       uint16_t dutyCycle = 577 + ((angle/180) * 1968);


       //Generate PWM - Timer runs in Up-Down mode
       Timer_A_outputPWMParam param = {0};
       param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
       param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
       param.timerPeriod = 21210;
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


       servo_write(GPIO_PORT_P1, GPIO_PIN7,90);




	return 0;
}
