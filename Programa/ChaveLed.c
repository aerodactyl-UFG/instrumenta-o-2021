#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <thread>

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
	pinMode(PIN_BEEP, OUTPUT);
	pinMode(PIN_LED, OUTPUT);

   	pullUpDnControl(PIN_CHAVE, PUD_UP);
   	}

void Beep (){

	digitalWrite(PIN_BEEP, 1);
	delay (1000);
	digitalWrite(PIN_BEEP, 0);
		
}

void ReadChave (int *estado){
		
	wiringPiSetup();	
	Setup_Pins();
	int chave=0;
	
	while(1){	
		chave = digitalRead(PIN_CHAVE);
		if ( !(chave) ){
			*estado = !(*estado);
			Beep();
			delay(5000);
		}
	delay (500);
	}	
}


int main(){
	
	int estado = 0;
	printf("\nEstado: %d", estado);
	
	std::thread t1 (ReadChave, &estado );

	while(1){
		printf("\nEstado: %d", estado);
		delay(400);
	}

	t1.join();

}
