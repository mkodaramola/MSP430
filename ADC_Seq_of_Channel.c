
#include "driverlib.h"
#include "Board.h"
#define TIMER_PERIOD 80

uint16_t result =0;

uint16_t arr[8] = {0,0,0,0,0,0,0,0};

int i = 7;


void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set A7 as an input pin.
    //Set appropriate module function
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_ADC7,
            GPIO_PIN_ADC7,
            GPIO_FUNCTION_ADC7);

    GPIO_setAsPeripheralModuleFunctionInputPin(
                GPIO_PORT_P1,
                GPIO_PIN6,
                GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionInputPin(
                GPIO_PORT_P1,
                GPIO_PIN5,
                GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionInputPin(
                GPIO_PORT_P1,
                GPIO_PIN4,
                GPIO_PRIMARY_MODULE_FUNCTION);


    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize the ADC Module
    /*
     * Base Address for the ADC Module
     * Use internal ADC bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC_init(ADC_BASE,
        ADC_SAMPLEHOLDSOURCE_SC,
        ADC_CLOCKSOURCE_ADCOSC,
        ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    /*
     * Base Address for the ADC Module
     * Sample/hold for 16 clock cycles
     * Do not enable Multiple Sampling
     */
    ADC_setupSamplingTimer(ADC_BASE,
        ADC_CYCLEHOLD_16_CYCLES,
        ADC_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input A7
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */

    //Start storing from A7 to A0
    ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A7,
        ADC_VREFPOS_INT,
        ADC_VREFNEG_AVSS);

//    ADC_configureMemory(ADC_BASE,
//            ADC_INPUT_A7,
//            ADC_VREFPOS_INT,
//            ADC_VREFNEG_AVSS);
//
//    ADC_configureMemory(ADC_BASE,
//            ADC_INPUT_A7,
//            ADC_VREFPOS_INT,
//            ADC_VREFNEG_AVSS);
//
//    ADC_configureMemory(ADC_BASE,
//            ADC_INPUT_A7,
//            ADC_VREFPOS_INT,
//            ADC_VREFNEG_AVSS);

    ADC_clearInterrupt(ADC_BASE,
        ADC_COMPLETED_INTERRUPT);

    //Enable Memory Buffer interrupt
    ADC_enableInterrupt(ADC_BASE,
        ADC_COMPLETED_INTERRUPT);

    //Internal Reference ON
    PMM_enableInternalReference();

    //Configure internal reference
    //If ref voltage no ready, WAIT
    while (PMM_REFGEN_NOTREADY == PMM_getVariableReferenceVoltageStatus());


    // Configure TA0 to provide delay for reference settling ~75us
    Timer_A_initUpModeParam initUpModeParam = {0};
    initUpModeParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initUpModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpModeParam.timerPeriod = TIMER_PERIOD;
    initUpModeParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initUpModeParam.timerClear = TIMER_A_DO_CLEAR;
    initUpModeParam.startTimer = true;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpModeParam);

    __bis_SR_register(CPUOFF + GIE);           // LPM0, TA0_ISR will force exit

    for (;;)
    {
        //Delay between conversions
        __delay_cycles(5000);

        //Enable and Start the conversion
        //in Single-Channel, Single Conversion Mode
        ADC_startConversion(ADC_BASE,
                            ADC_SEQOFCHANNELS);

        //LPM0, ADC_ISR will force exit
        __bis_SR_register(CPUOFF + GIE);
        //For debug only
        __no_operation();


    }
}

//ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR (void)
{
    switch (__even_in_range(ADCIV,12)){
        case  0: break; //No interrupt
        case  2: break; //conversion result overflow
        case  4: break; //conversion time overflow
        case  6: break; //ADCHI
        case  8: break; //ADCLO
        case 10: break; //ADCIN
        case 12:        //ADCIFG0

            //Automatically clears ADCIFG0 by reading memory buffer
            result = ADC_getResults(ADC_BASE);


            arr[i] = result;

            i--;


            if (i < 0) i = 7;






            //Clear CPUOFF bit from 0(SR)
            //Breakpoint here and watch ADC_Result
            __bic_SR_register_on_exit(CPUOFF);
            break;
        default: break;
    }
}



#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
      TA0CTL = 0;
      LPM0_EXIT;                                // Exit LPM0 on return
}
