#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <math.h>

#ifndef PITOT_IN 
#define PITOT_IN "AirSpeed"
#endif

#ifndef PITOT_OUT
#define PITOT_OUT "AirSpeed-out"
#endif

float CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C = 1.225;
float CONSTANTS_AIR_GAS_CONST = 287.1;
float CONSTANTS_ABSOLUTE_NULL_CELSIUS = -273.15;

const float P_min = -1.0f;
const float P_max = 1.0f;
const float PSI_to_Pa = 6894.757f;


int dp_raw = 0;
int dT_raw = 0;
int Timer= 0;
int time_zero=0;
float time_out;
float temperature;
float airspeed;


float calc_indicated_airspeed(float differential_pressure) {
       if (differential_pressure > 0.0){
		return sqrtf((2.0 * differential_pressure) / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
       }
	else{
		return -sqrtf((2.0 * fabsf(differential_pressure)) / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
       }
}


void process_airspeed () {
 		
    temperature = ((200.0f * dT_raw) / 2047) - 50;
    float diff_press_PSI = -((dp_raw - 0.1f * 16383) * (P_max - P_min) / (0.8f * 16383) + P_min);
    float diff_press_pa_raw = diff_press_PSI * PSI_to_Pa;
    airspeed = calc_indicated_airspeed(diff_press_pa_raw);

    time_out = ( double(Timer) - double(time_zero) ) / 1000000;	
	
}
   
void process_pitot (){    

    FILE *fp_in;
    FILE *fp_out;
    char file_in[] = PITOT_IN;
    char file_out[] = PITOT_OUT;
    
    fp_in = fopen(NameProcess(file_in), "rb");
    fp_out = fopen(NameProcess(file_out),"wb");
    int k;
    k = fscanf (fp_in,"%d", &Timer);
    time_zero = Timer;

    while(k != -1){

    	fscanf (fp_in,"%d", &dp_raw);
    	fscanf (fp_in,"%d", &dT_raw);
        process_airspeed();
        fprintf (fp_out ,"%f %.4f %.2f\n", time_out, airspeed, temperature);
	k = fscanf (fp_in, "%d", &Timer);
    }
    time_zero = 0;
    fclose (fp_in);
    fclose (fp_out);
}
