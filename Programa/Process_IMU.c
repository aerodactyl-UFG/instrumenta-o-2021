#include <stdio.h>
#include <math.h>

/* Source: https://github.com/TKJElectronics/KalmanFilter */
#include "Kalman.h" 

/* Different math and print constants */
#define RAD_TO_DEG                     (180.0 / M_PI)
#define DRIFT_MAX_DEGREES              180

#define LABEL_REPEAT_RATE              30

/* To restrict roll instead of pitch to ±90 degrees, comment out the following line */
#define PITCH_RESTRICT_90_DEG

#ifndef IMU
#define IMU "IMU"
#endif

#ifndef IMU_OUT
#define IMU_OUT "IMU-out"
#endif

FILE *fp_imuin;

/* MPU6050 variables */
//volatile int timer0; 
int volatile timer0;
int volatile timer;
int volatile time_SetZero;
double accX;
double accY;
double accZ;
double gyroX;
double gyroY;
double gyroZ;
double temp_raw;

/* Variables used for printing */
int counter = 0;
double temp_degrees_c;
double roll_gyro;
double roll;
double roll_kalman;         /* Angle exposed to a Kalman filter */
double roll_complementary;  /* Angle exposed to a Complementary filter */
double pitch;
double pitch_gyro;
double pitch_kalman;        /* Angle exposed to a Kalman filter */
double pitch_complementary; /* Angle exposed to a Complementary filter */


int ReadSensorData(){    
    int k;
    k = fscanf (fp_imuin,"%d", &timer);

    fscanf (fp_imuin,"%lf", &accX);
    fscanf (fp_imuin,"%lf", &accY);
    fscanf (fp_imuin,"%lf", &accZ);
    fscanf (fp_imuin,"%lf", &gyroX);
    fscanf (fp_imuin,"%lf", &gyroY);
    fscanf (fp_imuin,"%lf", &gyroZ);
    fscanf (fp_imuin,"%lf", &temp_raw); 
    
    return (k);
}

double convert_to_deg_per_sec(double a)
{
    return a / 131.0;
}

double distance(double a, double b)
{
    return sqrt((a*a) + (b*b));
}

double atan2_deg(double a, double b)
{
    return atan2(a,b) * RAD_TO_DEG;
}

double atan_deg(double a, double b, double c)
{
    return atan(a / distance(b, c)) * RAD_TO_DEG;
}

double max_drift_correction(double gyro, double kalman)
{
    if (gyro < -DRIFT_MAX_DEGREES || gyro > DRIFT_MAX_DEGREES)
        return kalman;
    else
        return gyro;
}

double max_90_deg_correction(double rate, double kalman)
{
    if (abs(kalman) > 90)
        return -rate;
    else
        return rate;
}






void process_imu() {

    char file_in[] = IMU;
    char file_out[] = IMU_OUT;
    
    FILE *fp_out;
    fp_imuin = fopen( NameIN(file_in), "rb");
    fp_out = fopen( NameOUT(file_out),"ab");
    int k; 
    
    double seconds_passed;
    double roll_gyro_rate_deg_per_sec;
    double pitch_gyro_rate_deg_per_sec;

    Kalman kalman_roll;
    Kalman kalman_pitch;
    

    /* Set the gyro starting angles */
    k = ReadSensorData();
    time_SetZero = timer0 = timer;

#ifdef PITCH_RESTRICT_90_DEG
    roll  = atan2_deg(accY, accZ);
    pitch = atan_deg(-accX, accY, accZ);
#else
    roll  = atan_deg(accY, accX, accZ);
    pitch = atan2_deg(-accX, accZ);
#endif

    /* Set some more initial values */
    kalman_roll.setAngle(roll);
    roll_gyro           = roll;
    roll_complementary  = roll;  /* Angle exposed to a complementary filter */
    kalman_pitch.setAngle(pitch);
    pitch_gyro          = pitch;
    pitch_complementary = pitch; /* Angle exposed to a complementary filter */

    while(k != -1)
    { 
	k = ReadSensorData();
	
        temp_degrees_c              = ((double)temp_raw / 340.0) + 36.53;
        seconds_passed              = ((double)(timer) - (double)(timer0) )  / 1000000;

        roll_gyro_rate_deg_per_sec  = convert_to_deg_per_sec(gyroX);
        pitch_gyro_rate_deg_per_sec = convert_to_deg_per_sec(gyroY);
	

    #ifdef PITCH_RESTRICT_90_DEG
        
        /* Eq. 25 and 26 from source for equations */
        roll  = atan2_deg(accY,accZ);
        pitch = atan_deg(-accX, accY, accZ);

        /* Let pitch have -90 and 90 degrees to be the continuous (and roll ±180) */
        if ( abs(roll)<= 90 || abs(roll_kalman)<= 90 )
        {
            /* Calculate roll_kalman */
            roll_kalman = kalman_roll.getAngle(roll, roll_gyro_rate_deg_per_sec, double(seconds_passed) );
//printf("\t%d\t%d\t%f", roll, roll_gyro_rate_deg_per_sec, double(seconds_passed));

	}
        else
        {
            kalman_roll.setAngle(roll);
            roll_complementary = roll;
            roll_kalman        = roll;
            roll_gyro          = roll;
        }
        pitch_gyro_rate_deg_per_sec = max_90_deg_correction(pitch_gyro_rate_deg_per_sec, roll_kalman);
        pitch_kalman                = kalman_pitch.getAngle(pitch, pitch_gyro_rate_deg_per_sec, seconds_passed);

    #else
        /* Eq. 28 and 29 from source for equations */
        roll  = atan_deg(accY, accX, accZ);
        pitch = atan2_deg(-accX,accZ);

        /* Let roll have -90 and 90 degrees to be the continuous (and pitch ±180) */
        if ( abs(pitch)<= 90 || abs(pitch_kalman)<= 90 )
        {
            /* Calculate pitch_kalman */
            pitch_kalman = kalman_pitch.getAngle(pitch, pitch_gyro_rate_deg_per_sec, seconds_passed);
        }
        else
        {
            kalman_pitch.setAngle(pitch);
            pitch_complementary = pitch;
            pitch_kalman        = pitch;
            pitch_gyro          = pitch;
        }
        roll_gyro_rate_deg_per_sec = max_90_deg_correction(roll_gyro_rate_deg_per_sec, pitch_kalman);
        roll_kalman                = kalman_roll.getAngle(roll, roll_gyro_rate_deg_per_sec, seconds_passed);
    #endif

        /* Calculate gyro angles without any filter */
        roll_gyro  += roll_gyro_rate_deg_per_sec * seconds_passed;
        pitch_gyro += pitch_gyro_rate_deg_per_sec * seconds_passed;

        /* Calculate gyro angle using the unbiased rate */
        //roll_gyro += kalman_roll.getRate() * seconds_passed;
        //pitch_gyro += kalman_pitch.getRate() * seconds_passed;

        roll_gyro  = max_drift_correction(roll_gyro, roll_kalman);
        pitch_gyro = max_drift_correction(pitch_gyro, pitch_kalman);


	timer0  =  timer;
        
	/* Calculate the angle using a Complimentary filter */
//        roll_complementary  = 0.93 * (roll_complementary + roll_gyro_rate_deg_per_sec * seconds_passed) + 0.07 * roll;
//        pitch_complementary = 0.93 * (pitch_complementary + pitch_gyro_rate_deg_per_sec * seconds_passed) + 0.07 * pitch;

        fprintf(fp_out, "\n%f\t%.2f\t%.2f",(double(timer)-double(time_SetZero))/1000000, roll_kalman, pitch_kalman );

    }
}