#include <unistd.h>
#include <stdio.h>

#define PIN_CHAVE 	0	//WiringPi pins
#define PIN_BEEP	7
#define PIN_LED	1	//WiringPi pins

int desliga(){
	if( execl("/usr/bin/sudo","sudo","shutdown","-h","now",NULL) ){
		return 0;
		}
	else return 1;
	}

void Setup_Pins(void){
	pinMode(PIN_CHAVE, INPUT);
	pinMode(PIN_BEEP,OUTPUT);
	pinMode(PIN_LED,OUTPUT);

   	pullUpDnControl(PIN_CHAVE, PUD_UP);
   	}

void ReadChave (int *chave){
	
	
	while(1){	
	chave = digitalRead(PIN_CHAVE);
	printf("\nEstado: %d", chave);	
	delay (1000);
	}	
}

int main(){
	chave;
	ReadChave(&chave);
	
}
