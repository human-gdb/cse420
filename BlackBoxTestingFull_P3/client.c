#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

#define MAXDIRPATH 1024
#define MAXKEYWORD 256

//	Global variables
int qSIZE = 1024*1024; //~1MB
int SEMrq=0;
char *RQptr;
char *shm;
char *Request;
char *Inputfile;
int bufSIZE;
struct sembuf signal_struct = { 0, 1 ,SEM_UNDO};
struct sembuf wait_struct = { 0, -1 ,SEM_UNDO};

//	Functions Declarations
char *get_shm();
void printSHM();
void AddReq(char *str);
void parseReq(char *rawReq);
int sem_val(int semid);
int signals(int semid);
int waits(int semid);
int readfile(char *filepath);
char *rws(char *str);




/*
*
*	Main Routine
*
*/
int main(int argc, char* argv[]) {
	
	RQptr = get_shm();
		//printf("\nWaiting on Request Queue access, SEMrq = %d",sem_val(SEMrq));fflush(stdout);
	waits(SEMrq);
	
	if(argc == 2 && strcmp(argv[1],"exit") == 0){
		AddReq("exit");
		//printSHM();
	}
	else if(argc < 2){
		readfile("./inputFile2.dat");
		//printSHM();
	}
	else{
		bufSIZE = atoi(argv[1]);
		Inputfile = strdup(argv[2]);
		
		readfile(Inputfile);
		//printSHM();
	}
	
	signals(SEMrq);
	return 0;
}





/*
*
*	Functions
*
*/
char* get_shm(){
	int shmid;
	
	
	
	//SEMrq = semget(0x7070,1, 0600);
	SEMrq = semget(0x7070,1,IPC_CREAT|0666);
	

	
	shmid = shmget(0x6969, qSIZE, IPC_CREAT | 0666);

	if(shmid < 0){ printf("shmget failed!");fflush(stdout); exit(-1);}

	shm = shmat(shmid,NULL,0);
	if(shm == (char*) -1){ printf("shmmat failed!");fflush(stdout); exit(-1);}

	//printSHM(shm);
	return shm;
}

void printSHM(){
	char *s = shm;

	for(s = shm; *s != '\0'; s++){
		if(*s == '>')printf("\n");
		printf("%c",*s);
	}
	
	printf("\n");
}

void AddReq(char *str){
	char *s = shm;
	



	for(s = shm; *s != '<'; s++)	;
	while(*s != '>')s--;
	s++;

	memcpy(s,str,strlen(str)+1);

	s+= strlen(str);
	*s = '>';s++;*s = '<';
	
}

int sem_val(int semid){
	int val = semctl(semid, 0, GETVAL);
	//printf("SEM ID: %d, Value: %d\n",semid,val);
	return val;
}

int signals(int semid){
	return semop(semid,&signal_struct,1);
}

int waits(int semid){
	return semop(semid,&wait_struct,1);
}

int readfile(char *filepath){
	DIR *dir = opendir(filepath);
        if(dir != NULL) {
         closedir(dir);
         return 0;
        }

        FILE *file = fopen(filepath,"r");
        char buffer[1024];
        int line_num = 1;

        if(file == NULL) {
              perror("Error opening file");
              return(-1);
           }

           while( fgets (buffer, 1024, file)!=NULL ) {
		
		if(strstr(buffer,"exit") == NULL){
             		parseReq(buffer);
             		AddReq(Request);
             	}
             	else
             		AddReq("exit");
           line_num++;
           }
           //printf("File:%s \nhas %d occurance of %s\n",filePath,count,keyword);
           fclose(file);
     	   return 0;
}

void parseReq(char *rawReq){
	char path [MAXDIRPATH] = "";
	//getcwd(path, sizeof(path));
	char *rest = strstr(rawReq,"/");
	rawReq = rws(strstr(rawReq," ")+1);
	rawReq[strchr(rawReq,'\n')-rawReq] = '\0';
	Request = malloc(strlen(path)+strlen(rest));
	sprintf(Request,"%s%s",path,rest);
}

char *rws(char *str)
{
	int i = 0, j = 0;
	while (str[i])
	{
		if (str[i] != ' ')
          str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
	return str;
}

