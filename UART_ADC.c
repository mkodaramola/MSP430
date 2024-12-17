
// https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html

#include "driverlib.h"
#include "Board.h"
#include <stdio.h>
#include <stdlib.h>

#define TIMER_PERIOD 80

int n;
void ADC_setup();
void UART_TX(char[]);
char * concat(char[], char[]);
uint16_t result =0;
uint16_t i;
uint8_t RXData = 0, TXData = 0;
uint8_t check = 0;

void main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    //Set ACLK = REFOCLK with clock divider of 1
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);


    ADC_setup();


    //Configure UART pins
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_UCA0TXD,
        GPIO_PIN_UCA0TXD,
        GPIO_FUNCTION_UCA0TXD
    );
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_UCA0RXD,
        GPIO_PIN_UCA0RXD,
        GPIO_FUNCTION_UCA0RXD
    );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Configure UART
    //SMCLK = 1MHz, Baudrate = 115200
    //UCBRx = 8, UCBRFx = 0, UCBRSx = 0xD6, UCOS16 = 0
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 0;
    param.secondModReg = 0xD6;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable global interrupts
    __enable_interrupt();
    while (1)
    {
                __delay_cycles(1000000);


                ADC_startConversion(ADC_BASE,
                        ADC_SINGLECHANNEL);

                __bis_SR_register(CPUOFF + GIE);

                __no_operation();

                char str[20];

                   snprintf(str, sizeof(str), "%d", result);


                  UART_TX(str);


                  UART_TX("\n\r");




    }
}



// ---- Interrupt Sub-Routine

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void EUSCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            RXData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

            UART_TX("You pressed something\n\r");

            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
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
            //ADCMEM = A0 > 0.5V?
            result = ADC_getResults(ADC_BASE);

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






// -------- User Defined Functions

void UART_TX(char * text){
        int i = 0;

        while (text[i] != '\0'){

            n = text[i];
            EUSCI_A_UART_transmitData(EUSCI_A0_BASE, n);
            __delay_cycles(1000);

            i++;
        }

}


char * concat(char * text1, char * text2){
        int l1 = 0;
        int l2 = 0;



        while (text1[l1] != '\0'){
            l1++;
        }
        while (text2[l2] != '\0'){
            l2++;
        }

        char * ntext = (char*)malloc(l1+l2+1);


        l1 = 0;
        l2 = 0;

        while (text1[l1] != '\0'){
             ntext[l1] = text1[l1];
            l1++;
        }
        while (text2[l2] != '\0'){
            ntext[l1+l2] = text2[l2];
            l2++;
        }


        return ntext;
}




void ADC_setup(){

        GPIO_setAsPeripheralModuleFunctionInputPin(
                   GPIO_PORT_ADC7,
                   GPIO_PIN_ADC7,
                   GPIO_FUNCTION_ADC7);


           PMM_unlockLPM5();

           ADC_init(ADC_BASE,
               ADC_SAMPLEHOLDSOURCE_SC,
               ADC_CLOCKSOURCE_ADCOSC,
               ADC_CLOCKDIVIDER_1);

           ADC_enable(ADC_BASE);

           ADC_setupSamplingTimer(ADC_BASE,
               ADC_CYCLEHOLD_16_CYCLES,
               ADC_MULTIPLESAMPLESDISABLE);

           ADC_configureMemory(ADC_BASE,
               ADC_INPUT_A7,
               ADC_VREFPOS_INT,
               ADC_VREFNEG_AVSS);

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

           __bis_SR_register(CPUOFF + GIE);

    }
