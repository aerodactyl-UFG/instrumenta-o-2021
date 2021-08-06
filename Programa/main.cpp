#include <stdio.h>
#include <thread>
#include <iostream>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "Manager_FILES.c"
#include "Read_IMU.c"
#include "Process_IMU.c"
#include "Read_Airspeed.c"
#include "Process_Airspeed.c"
#include "ChaveLed.c"

int main(){

	int estado = 0;
	std::thread t0 (ReadChave, &estado);
	std::cout << "thread t0 id= " << t0.get_id() << std::endl;	
	
	while (1){
//		printf("\nEstado: %d", estado);
		if (estado == 1){
		    UpdateManager(1);	
//		    std::thread t1 (read_imu, &estado);
		    std::thread t2 (read_pitot, &estado);

//		    std::cout << "thread t1 id= " << t1.get_id() << std::endl;	
		    std::cout << "thread t2 id= " << t2.get_id() << std::endl;

//  		    t1.join();
		    t2.join();
		    }

		else	if (UpdateManager(2) ){	
//		    	std::thread t3 (process_imu);
		    	std::thread t4 (process_pitot);

//		    	std::cout << "thread t3 id= " << t3.get_id() << std::endl;	
		    	std::cout << "thread t4 id= " << t4.get_id() << std::endl;

//		    	t3.join();
		    	t4.join();
		    }


	}

t0.join();
return (0);
}
