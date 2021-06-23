#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <pthread.h>
#include <math.h>

#ifndef PITOT_IN
#define PITOT_IN "AirSpeed"
#endif

void read_pitot(int *estado) {
    
    static int fd;
    typedef unsigned char   u8;
    
    fd = open("/dev/i2c-1", O_RDWR);
    char file_pitot[] = PITOT_IN;
    FILE *fp_pitot;
    fp_pitot = fopen(NameIN(file_pitot), "ab");
    int timer;

    while(*estado == 1){
	unsigned char bytes[4] = {0, 0, 0, 0};
	ioctl(fd, I2C_SLAVE, 0x28);
	read(fd, bytes, 4);
	timer = micros();
	char status = (bytes[0] & 0xC0) >> 6;
	int dp_raw = 0, dT_raw = 0;
	dp_raw = (bytes[0] << 8) + bytes[1];
	/* mask the used bits */
	dp_raw = 0x3FFF & dp_raw;
	dT_raw = (bytes[2] << 8) + bytes[3];
	dT_raw = (0xFFE0 & dT_raw) >> 5;
	
	if(status == 0){
	    fprintf(fp_pitot, "%d\t%d\t%d\n",timer, dp_raw, dT_raw );
        }
    }
}
