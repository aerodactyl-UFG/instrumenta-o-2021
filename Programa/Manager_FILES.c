#include <stdio.h>
#include <stdlib.h>

#define	Manager		"./files/.manager.dat"
#define LocalSave	"./files/"
#define Extension	".dat"

int ReadNum (void);
void PrintNum (int);
char * concatenar (char *);
FILE *FpNum;
int cache[] = {0,0};

int* ReadManager (void){
	if(FpNum = fopen (Manager,"rb") ){
		fscanf (FpNum,"%d %d",&cache[0], &cache[1]);
		fclose(FpNum);
		return (cache);		//prox voo
	}
	else{
		cache[0] = cache[1] = 0;
		return (cache);		//primeiro voo
	}
}

void WriteManager (int NumSave[]){
	FpNum = fopen (Manager,"wb");
	fprintf(FpNum,"%d %d",NumSave[0],NumSave[1]);
	fclose(FpNum);
}

int UpdateManager (int N){
	int *atual = ReadManager();

	if(N==1){	//nova leitura de dados sem processamento
		cache[0] = atual[0] + 1; //modifica apenas "voo atual"
		cache[1] = atual[1];
		WriteManager(cache);
		return (1);
	}
	else if(N==2 && (atual[1] < atual[0]) ){	
		cache[0] = atual[0];
		cache[1] = atual[1] + 1; //modifica apenas "voos processados"
		if(cache[0]==cache[1]) printf("Process OK\n");
		WriteManager(cache);
		return (1);
	}
	else{
//		printf("\nOpção invalida para manager");
		delay(500);
		return(0);
	}

}

char* NameRead (char *Name){
	int *NumLido = ReadManager();
	char *name = (char *) malloc (sizeof(LocalSave) + sizeof(Name) + sizeof("_") + sizeof(NumLido[0]) + sizeof(Extension) );
	sprintf (name, "%s%s_%d%s", LocalSave, Name, NumLido[0], Extension);
	return name;
}

char* NameProcess (char *Name){
	int *NumLido = ReadManager();
	char *name = (char *) malloc (sizeof(LocalSave) + sizeof(Name) + sizeof("_") + sizeof(NumLido[1]) + sizeof(Extension) );
	sprintf (name, "%s%s_%d%s", LocalSave, Name, NumLido[1], Extension);
	return name;
}

