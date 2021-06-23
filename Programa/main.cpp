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

int main(int argc, char *argv[]){
	int mode = 0;
	if(argc == 1){	 	//opção defaut, inicio sem argumentos
	    mode = 1;
	}
	else{
	    mode = *argv[1] - '0';
	}
        printf("Mode: %d\n", mode);


	int estado = 0;
	std::thread t0 (ReadChave, &estado);
	std::cout << "thread t0 id= " << t0.get_id() << std::endl;	
	
	while (1){

//	    switch(mode){
		if (estado == 1)	
//	        case 1:	//apenas armazena dados, sem nenhum processamento
		    {
		    UpdateManager(1);	
		    std::thread t1 (read_imu, &estado);
//		    std::thread t2 (read_pitot, &estado);

		    std::cout << "thread t1 id= " << t1.get_id() << std::endl;	
//		    std::cout << "thread t2 id= " << t2.get_id() << std::endl;

  		    t1.join();
//		    t2.join();
		    }
//		    break;

		else
//	        case 2:	//processar dados restantes
		    {
		    UpdateManager(2);	
		    std::thread t1 (process_imu);
//		    std::thread t2 (process_pitot);

//		    std::cout << "thread t1 id= " << t1.get_id() << std::endl;	
//		    std::cout << "thread t2 id= " << t2.get_id() << std::endl;

		    t1.join();
//		    t2.join();
		    }	
//		    break;


//	        default:
//		    {printf ("\nErro!!\nModo de execução não definido\n");
//		    break;
//		    }
//	    }
	}

t0.join();
return (0);
}
