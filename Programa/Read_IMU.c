/* MPU6050 REGISTERS */
#define MPU6050_I2C_DEVICE_ADDRESS     0x68
#define REGISTER_FOR_POWER_MANAGEMENT  0x6B  /* PWR_MGMT_1 */
#define REGISTER_FOR_SAMPLE_RATE       0x19  /* SMPLRT_DIV */
#define REGISTER_FOR_ACCEL_XOUT_H      0x3B
#define REGISTER_FOR_ACCEL_YOUT_H      0x3D
#define REGISTER_FOR_ACCEL_ZOUT_H      0x3F
#define REGISTER_FOR_GYRO_XOUT_H       0x43
#define REGISTER_FOR_GYRO_YOUT_H       0x45
#define REGISTER_FOR_GYRO_ZOUT_H       0x47
#define REGISTER_FOR_TEMP_OUT_H        0x41
#define SLEEP_MODE_DISABLED            0x00

#ifndef IMU
#define IMU "IMU"
#endif

int gyro_device_handler;


int read_word_2c(int register_h){
    int val;
    val = wiringPiI2CReadReg8(gyro_device_handler, register_h);
    val = val << 8;
    val += wiringPiI2CReadReg8(gyro_device_handler, register_h+1);
    if (val >= 0x8000)
        val = -(65536 - val);
    return val;
}


void fresh_init(){
    for(int i=0; i++; i<20 ){
	read_word_2c(REGISTER_FOR_ACCEL_XOUT_H);
	read_word_2c(REGISTER_FOR_ACCEL_YOUT_H);
	read_word_2c(REGISTER_FOR_ACCEL_ZOUT_H);
	read_word_2c(REGISTER_FOR_GYRO_XOUT_H);
	read_word_2c(REGISTER_FOR_GYRO_YOUT_H);
	read_word_2c(REGISTER_FOR_GYRO_ZOUT_H);
    }
}


int read_imu(int *estado){
    double AccX;
    double AccY;
    double AccZ;
    double GyroX;
    double GyroY;
    double GyroZ;
    double Temp_raw;

    int Timer;
    char file_in[] = IMU; 
    FILE *fp_in;
    fp_in = fopen( NameIN(file_in), "ab");

    gyro_device_handler = wiringPiI2CSetup(MPU6050_I2C_DEVICE_ADDRESS);
    wiringPiI2CWriteReg8(gyro_device_handler,REGISTER_FOR_POWER_MANAGEMENT,SLEEP_MODE_DISABLED);
    fresh_init();

    while(*estado == 1){
        
	AccX = read_word_2c(REGISTER_FOR_ACCEL_XOUT_H);
	AccY = read_word_2c(REGISTER_FOR_ACCEL_YOUT_H);
	AccZ = read_word_2c(REGISTER_FOR_ACCEL_ZOUT_H);
	GyroX = read_word_2c(REGISTER_FOR_GYRO_XOUT_H);
	GyroY = read_word_2c(REGISTER_FOR_GYRO_YOUT_H);
	GyroZ = read_word_2c(REGISTER_FOR_GYRO_ZOUT_H); 
	Temp_raw = read_word_2c(REGISTER_FOR_TEMP_OUT_H);

        Timer = micros();
        fprintf(fp_in, "%d %lf %lf %lf %lf %lf %lf %lf\n",Timer, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, Temp_raw);
    }

}
