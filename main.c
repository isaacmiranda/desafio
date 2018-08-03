#include <stdio.h>
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "OneWireCRC.h"

//Atividade: Plotar numa porta COM o ID do iButton

//Variaveis globais para a bilbioteca da Application Note 187 da MAXXIM
unsigned char ROM_NO[8];
int LastDiscrepancy;
int LastFamilyDiscrepancy;
int LastDeviceFlag;
unsigned char crc8;

//Macros para o port da biblioteca OneWireCRC
#define ONEWIRE_PIN_NUM					9
#define ONEWIRE_PIN_MASK                (9<<ONEWIRE_PIN_NUM)
#define ONEWIRE_PORT					GPIOA
#define ONEWIRE_CLK						RCC_AHBPeriph_GPIOA

#define  ONEWIRE_INPUT_READ				ONEWIRE_PORT->IDR&ONEWIRE_PIN_MASK
#define  ONEWIRE_OUTPUT_HIGH			ONEWIRE_PORT->BSRR=ONEWIRE_PIN_MASK
#define  ONEWIRE_OUTPUT_LOW				ONEWIRE_PORT->BSRR=ONEWIRE_PIN_MASK
#define  ONEWIRE_CONFIG_OUTPUT			ONEWIRE_PORT->MODER|=(GPIO_Mode_OUT<<(ONEWIRE_PIN_NUM*2))
#define  ONEWIRE_CONFIG_INPUT			ONEWIRE_PORT->MODER&=~(GPIO_MODER_MODER0<<(ONEWIRE_PIN_NUM*2))


#define  READ_ROM			0x33
#define  SKIP_ROM			0xcc

GPIO_InitTypeDef			GPIO_InitStructure;


//Funçoes portadas - OneWireCRC para o stm32f042k6
void MyPinInit(){

	    RCC_AHBPeriphClockCmd(ONEWIRE_CLK,ENABLE);
	    GPIO_InitStructure.GPIO_Pin = ONEWIRE_PIN_NUM;					// select the pin to modify
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				// set the mode to output
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;			// set the I/O speed to 100 MHz
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				// set the output type to open-drain
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// set the pull-up to none
	    GPIO_Init(ONEWIRE_PORT, &GPIO_InitStructure);				// do the init

}
void MyPinValueSet(int value){
	ONEWIRE_CONFIG_OUTPUT;
	if(value==1){
		ONEWIRE_OUTPUT_HIGH;
	}
	if(value==0){
		ONEWIRE_OUTPUT_LOW;
	}
}
int  MyPinValueGet(void){
	ONEWIRE_CONFIG_INPUT;
	return ONEWIRE_INPUT_READ;

}

int main(void)
{
	SystemInit();
	RCC_DeInit();
	printf("Sistema inicializado!\r\n");
	OneWireCRC_init();
	uint8_t novo_endereco;

	printf("OK!\r\n");

while(1)
{

	if(OneWireCRC_search(novo_endereco)){
		for (int i = 7; i >= 0; i--)
		     printf("%02X", ROM_NO[i]);
		    	}else{
		    		printf("Nada encontrado!");
		    	}
}
}
