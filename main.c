#include <LPC21xx.H>
#include "led.h"

#define I2C_PINS 			0x50

#define START 				0x08
#define SLA_W_ACK 		0x18
#define SLA_W_NACK 		0x20
#define BYTE_ACK 			0x28

#define SLOT_CONFIG   (1<<5) | 9
#define CANAL_9 			(1<<9)

#define STA_MASK 			(1<<5)
#define SI_MASK 			(1<<3)
#define STO_MASK 			(1<<4)
#define W_MASK 				0

#define I2C_CLEAR 		(1<<6)|(1<<5)|(1<<3)|(1<<2)
#define I2C_ON 				(1<<6)

#define PCF8574_ADDR 	0x40

unsigned char ucTxData;
unsigned char ucBusy;
unsigned char ucLedFlag;

int onemSecond = 540;
int iLoopCtr;

void Delay(int nDesiredValue){
	int ourTime = onemSecond * nDesiredValue;
	for(iLoopCtr=0; iLoopCtr<ourTime; iLoopCtr++){}
}

void __irq I2C_Interrupt(void) {
	
    switch (I2STAT) {
        case START:
            I2DAT = PCF8574_ADDR;
            I2CONCLR = STA_MASK;
            break;
        case SLA_W_ACK:
            I2DAT = ucTxData;
            break;
        case SLA_W_NACK:
        case BYTE_ACK:
            I2CONSET = STO_MASK;
            ucBusy = 0;
            break;
        default:
            I2CONSET = STO_MASK;
            ucBusy = 0;
						ucLedFlag = 1;
            break;
    }
		
		I2CONCLR = SI_MASK;
		VICVectAddr = 1;
}

void I2C_Init(void) {

    PINSEL0 |= I2C_PINS;

    I2CONCLR = I2C_CLEAR;

    I2CONSET = I2C_ON;

    I2SCLH = 0x80;
    I2SCLL = 0x80;

    VICVectAddr0 = (unsigned long) I2C_Interrupt;
    VICVectCntl0 = SLOT_CONFIG;
    VICIntEnable |= CANAL_9;
	
}

void PCF8574_Write(unsigned char ucData) {
    ucTxData = ucData;
    ucBusy = 1;
    I2CONSET = STA_MASK;
}

int main(){
	
	unsigned char ucCounter = 0;
	I2C_Init();
	LedInit();
	ucLedFlag = 0;
	
	while(1){
		if(!ucBusy) {
			PCF8574_Write(ucCounter);
			ucCounter++;
			Delay(500);
		}
		if(ucLedFlag){
			LedOn(0);
		}
	}
	
	
}



