#include <unistd.h>
//#include <stdio.h>
//#include <wiringPi.h>
//#include <thread>

#define PIN_CHAVE 	7	//WiringPi pins
#define PIN_BEEP	0
#define PIN_LED	1	//WiringPi pins

int desliga(){
	if( execl("/usr/bin/sudo","sudo","shutdown","-h","now",NULL) ){
		return 0;
		}
	else return 1;
	}

void Setup_Pins(void){
	pinMode(PIN_CHAVE, INPUT);
	pinMode(PIN_BEEP, OUTPUT);
	pinMode(PIN_LED, OUTPUT);

   	pullUpDnControl(PIN_CHAVE, PUD_UP);	
   	}

void Test_Interface(){
	digitalWrite(PIN_LED, 1);
	digitalWrite(PIN_BEEP, 1);

	delay(1000);

	digitalWrite(PIN_LED, 0);
	digitalWrite(PIN_BEEP, 0);
}

void Beep (){
	digitalWrite(PIN_BEEP, 1);
	delay (1000);
	digitalWrite(PIN_BEEP, 0);	
}

void ReadChave (int *estado){

	int chave = 0;
	wiringPiSetup();	
	Setup_Pins();
	
	delay (1000);
	Test_Interface();
	
	while(1){	
		chave = digitalRead(PIN_CHAVE);
		if ( !(chave) ){
			*estado = !(*estado);
			digitalWrite(PIN_LED, *estado);
			Beep();
			delay(5000);
		}
	delay (500);
	}	
}
