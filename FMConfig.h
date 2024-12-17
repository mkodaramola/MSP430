/*
 * FMConfig.h
 * Created on: Nov 8, 2023
 * Author: user
 *
 */



#ifndef FMCONFIG_H_
#define FMCONFIG_H_


FMConfig();
void initClocks(const uint16_t);
void initTimerUpMode(const uint16_t);
void delay( uint8_t n,void (*toggleFunction)(uint8_t, uint8_t),uint8_t port, uint8_t pin, int msec )
void PWMoutput(const uint8_t,const uint8_t,const uint32_t, uint32_t);



#endif /* FMCONFIG_H_ */
