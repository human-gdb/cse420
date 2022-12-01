#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>    
#include <sys/sem.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXDIRPATH 1024
#define MAXKEYWORD 256
#define MAXLINESIZE 1024
#define MAXOUTSIZE 2048
#define MAXREQSIZE (MAXDIRPATH+1+MAXKEYWORD) 


#define logging 1

//	Global variables
int rqSIZE = 1024*1024*100; //~1MB
int SEMrq=0,SEMfr=0,SEMbuf=0,filelock=0;
char *RQptr; int qSIZE;
char *shm;
struct sembuf signal_struct = { 0, +1 ,SEM_UNDO};
struct sembuf wait_struct = { 0, -1 ,SEM_UNDO};
typedef struct Item Item;
typedef struct String{ char* str; }String;
typedef struct Item{char* dirpath; char*keyword; String *buffer;}Item;
char *rqHeader = "Request Queue ->";
Item *curItem;
 pthread_t tids[20];
 int threadCount = 0;
 int lastlen=0;
 int initvars = 0;
 int BUFFERSIZE = 10;
 int cur_put = 0;
 int cur_get = 0;
sem_t empty;
sem_t full;
sem_t mutex;
int fullbuf=0,emptybuf=0,mutexbuf=0;
int randint(int n);



//	Functions Declarations
char *init_shm();
int sem_ini(int semid,int val);
int sem_val(int semid);
int signals(int semid);
int waits(int semid);
void ChildsPlay();
void printSHM();
Item *dequeueItem(char *QueuePtr);
void openDir(char *dirpath, char* keyword,String *buffer);
void* Worker(void *param);
void* Printer(void *arg);
char* search_test(char * filePath,char *keyword,String *buffer);
void joinThreads(pthread_t *tids,int l);
int charcount(char *str,char c);
void init_file(char *fname);
void fileWriter(char* str);
void put(char *str,String *buf);
char *getitem(String *buf);
void destroyBuffer(String *buffer);
int random_number(int min_num, int max_num);

/*
*
*	Main Routine
*
*/
int main(int argc, char* argv[]) {
	
	if(argc > 2){
		qSIZE = atoi(argv[1]) * MAXREQSIZE;
		BUFFERSIZE = atoi(argv[2]);
	 }
			

	int n=0;

	
	if(logging==1){printf("\n Main Process:\n");fflush(stdout);}
	if(initvars ==0){
		
		char pwd[1024];
		getcwd(pwd, sizeof(pwd));
		
		if(strstr(pwd,"->") != NULL){
			printf("Executing Directory: %s\nError!: '%s' not allowed in directory path.",pwd,"->");
			exit(-1);
		}
		else if(strstr(pwd,">") != NULL){
			printf("Executing Directory: %s\nError!: '%s' not allowed in directory path.",pwd,">");
			exit(-1);
		}
		else if(strstr(pwd,"<") != NULL){
			printf("Executing Directory: %s\nError!: '%s' not allowed in directory path.",pwd,"<");
			exit(-1);
		}
		else if(strstr(pwd," ") != NULL){
			printf("Executing Directory: %s\nError!: '%s' not allowed in directory path.",pwd," ");
			exit(-1);
		}
		else{
			printf("\nWarning! Please make sure that directory names and keywords do not contain space or arrow chars.\n");
			printf("Executing Directory: %s\n",pwd);
		}
		
		
		
		RQptr = init_shm();
		init_file("output.txt");
		SEMbuf = semget(0x7171,1,IPC_CREAT | 0666);
		sem_ini(SEMbuf,1);
		
		
		initvars++;
	}
		
	while(strstr(shm,"exit") == NULL){

 	
		if(logging==1)printf("\rWaiting on client"); //NOP
		
		if(charcount(shm,'>') > 1/*sem_val(SEMrq)==0*/){
			//printf("\nWaiting on SEMrq, key: 0x7070,SEMrq value:%d",sem_val(SEMrq));fflush(stdout); //NOP
				if(strstr(shm,"exit") == NULL)curItem = dequeueItem(RQptr);
				break;
		}
		usleep(10);
	}	
	if(strstr(shm,"exit") == NULL)
		n = fork();
	if(n == 0 && strstr(shm,"exit") == NULL){
		ChildsPlay(); // do childsplay
	}
	else if(n != 0 && strstr(shm,"exit") == NULL){
		free(curItem);
		n =0;
		return main(argc,argv);
	}
	
	
	if(n != 0)
		wait(NULL);
	
	
	shm_unlink(shm);	
	return 0;
}





/*
*
*	Functions
*
*/

char *init_shm(){

	
	int shmid;
	char *s;
	//SEMrq = sem_open("xd", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
	
	if(logging==1){printf("\nMaking SEMrq, key: 0x7070");fflush(stdout);}
	SEMrq = semget(0x7070,1,IPC_CREAT | 0666);
	sem_ini(SEMrq,0);
	 
	filelock = semget(0x8181,1,IPC_CREAT | 0666);
	sem_ini(filelock,0);
	
	shmid = shmget(0x6969, rqSIZE, IPC_CREAT | 0666);
	if(shmid < 0){ perror("shmget failed!"); exit(-1);}
	
	shm = shmat(shmid,NULL,0);
	if(shm == (char*) -1){ perror("shmmat failed!"); exit(-1);}
	
	memcpy(shm,rqHeader,strlen(rqHeader));
	s = shm;
	s+= strlen(rqHeader);
	*s = '<';
	s = shm; 
	s+= rqSIZE -1; 
	*s = '\0';
	signals(SEMrq);
	if(logging==1){printf("\nPosting SEMrq, key: 0x7070,SEMrq value: %d",sem_val(SEMrq));fflush(stdout);}
	
	return shm;		
}

void printSHM(){
	char *s = RQptr;

	for(s = RQptr; *s != '\0'; s++){
		if(*s == '>')if(logging==1)printf("\n");
		if(logging==1)printf("%c",*s);
	}
	
	if(logging==1)printf("\n");
}


int sem_ini(int semid, int val){
	union semun {
    	int val;
    	struct semid_ds *buf;
    	unsigned short  *array;
	};
	union semun u;
	u.val = val;
	return semctl(semid, 0, SETVAL, u);
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

void ChildsPlay(){
	 
	if(logging==1)printf("\n\nForked: I am the child working on\n");
	if(logging==1)printf("\n\tdirpath:%s<-\n\tkeyword:%s<-\n\n",curItem->dirpath,curItem->keyword);
	
	
	String *buffer = calloc(BUFFERSIZE,sizeof(String));
	sem_init(&empty, 0, 10); // max are empty
  	sem_init(&full, 0, 0);    // 0 are full
  	sem_init(&mutex, 0, 1);
  	pthread_t Printer_tid; 	
    	pthread_attr_t attr; 
    	
	
	
	
	sem_ini(fullbuf,0);
	sem_ini(emptybuf,BUFFERSIZE);
	sem_ini(mutexbuf,1);
	
	
	//printf("\nWaiting on Dir access");fflush(stdout);
	waits(SEMfr);
	//printf("\nGot on Dir access");fflush(stdout);
	
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        if(pthread_create(&Printer_tid, &attr, Printer, buffer) != 0 )
	  	if(logging==1)printf("\nWorker thread creation failed!");
	
	
	openDir(curItem->dirpath,curItem->keyword,buffer);
	signals(SEMfr);
    	joinThreads(tids,threadCount);
    	
    	/*  end condition*/
    	sem_wait(&empty);
    	sem_wait(&mutex);
    	put("exitchild",buffer);
    	sem_post(&mutex);
    	sem_post(&full);
    	

    	pthread_join(Printer_tid,NULL);
    	destroyBuffer(buffer);
    	//sem_destroy(&empty);
    	//sem_destroy(&mutex);
    	//sem_destroy(&full);
    	usleep(1000);
    	exit(1);
  	
}

Item *dequeueItem(char *QueuePtr){
	char *s = shm;
	char *str=malloc(1024); int l=0;
	for(s = shm; *s != '>'; s++)	;
	s++;
	while(*s != '>'){
		str[l]=*s;
		s++;l++;	
	}
	
	str = realloc(str,l+1);
	
	
	int r=strlen(rqHeader);
	while(s != shm-1){
	if(r>0){*s = rqHeader[r-1]; r--;}
	else *s = ' ';
	s--;
	}
	
	RQptr = strstr(RQptr,rqHeader);
	
	Item *item = malloc(sizeof(Item));
	item->dirpath = strdup(str);
	char *p = item->dirpath;
	for(p=item->dirpath;*p!=' ';p++) ;
	*p = '\0';
	p++;
	item->keyword = strdup(p);
	printSHM();
	
	SEMfr = semget(ftok(item->dirpath,'E'),1,IPC_CREAT | 0600);
	sem_ini(SEMfr,1);
	
	free(str);
	
	return item;
}	

void openDir(char *path, char* keyword,String *buffer){
    DIR *folder = opendir(path);
    char* abspath;
    struct dirent *entry;
    pthread_t tid; 	
    pthread_attr_t attr;
    
  

    if(folder == NULL){
        perror("Unable to open dir");
        exit(-1);
    }

    while( (entry=readdir(folder)) ){
        if(entry->d_name[0] != '.' && entry->d_type != 4){
                abspath = malloc(MAXDIRPATH);
                sprintf(abspath,"%s/%s",path,entry->d_name);
                  if(logging==1)printf("\n\nfile to work on: %s",entry->d_name);
                  Item *item = malloc(sizeof(Item));
                  item->dirpath = strdup(abspath);
                  item->keyword = strdup(keyword);
                  item->buffer = buffer;

    	          pthread_attr_init(&attr);
                  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
                  if(pthread_create(&tid, &attr, Worker, item) != 0 )
                  if(logging==1)printf("\nWorker thread creation failed!");
                  tids[threadCount] = tid; threadCount++;
                
                free(abspath);
        }
      

    }closedir(folder);free(entry);
}

void* Worker(void *param){
    Item *item = (Item*)param;
    if(logging==1)printf("\nWorker Thread working on:\n");
    if(logging==1)printf("\n\tdirpath:%s<-\n\tkeyword:%s<-\n\n",item->dirpath,item->keyword);
    
    search_test(item->dirpath,item->keyword,item->buffer);
    pthread_exit(0);
}


char* search_test(char * filePath,char *keyword,String *buffer){
	
	char delim[] = " \t\n";
        DIR *dir = opendir(filePath);
        if(dir != NULL) {
         closedir(dir);
         return NULL;
        }

        FILE *file = fopen(filePath,"r");
        char sbuffer[1024];
        char *word;
        char *ptr=NULL;
        int line_num = 1;
        char line[MAXOUTSIZE];
	char *result = calloc(10,MAXOUTSIZE);
	char *temp = calloc(10,MAXOUTSIZE);
	int count = 0;
        int found = 0;
        if(file == NULL) {
              perror("Error opening file");
              return(NULL);
           }
           
           /*get fname from fpath*/
           char *s = filePath + strlen(filePath)-1;
           while(*s != '/') s--;
           s++;
           char *fname = strdup(s);
           			     
	   /*scan line by line*/
           while( fgets (sbuffer, 1024, file)!=NULL ) {
           	
		sprintf(line,"%s:%d:%s",fname,line_num,sbuffer);
              for(word=strtok_r(sbuffer,delim,&ptr);word != NULL;word=strtok_r(NULL,delim,&ptr) ) {
                  word = strdup(word);
	
                  if(strcmp(keyword,word)==0){
                        found=1;    
                  }
		free(word);
              }
              if(found==1){
		      found=0;
		      // send line to buffer
		      sem_wait(&empty);
    		      sem_wait(&mutex);
		      put(line,buffer);
		      sem_post(&mutex);
    		      sem_post(&full);
    		      
		     
		      count++;
              }
           line_num++;
           }
 
           free(temp);
           free(fname);
           fclose(file);
         return result;
}


void* Printer(void *arg){
	String *buffer = arg;
	char *str = NULL;
	do{	
		 sem_wait(&full);
	         sem_wait(&mutex);
		  str = getitem(buffer);
		  if(strcmp(str,"exitchild") != 0)fileWriter(str);
		 sem_post(&mutex);
		 sem_post(&empty);
		 
	}while(strcmp(str,"exitchild") != 0);
	pthread_exit(0);
}



void init_file(char *fname){
    if (remove(fname) == 0) 
        printf("\nold %s was deleted...",fname); 
    fflush(stdout); 
}
void fileWriter(char* str){
    int *file = NULL;
    waits(SEMbuf); //semaphoric file lock no longer needed
    
    file = (int *)fopen("output.txt","a+");
    if(file != NULL)
        	//printf("\nnew output.txt was created...");
    if(file==NULL){
        	printf("Unable export output.txt");
        exit(-1);
    } 
    
    /**Locking output.txt**/
    struct flock lock;
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    //fcntl (*file, F_SETLKW, &lock);
    
    
    fprintf((FILE*)file,"%s",str);
    
    
    /**Unlocking output.txt**/
    //lock.l_type = F_UNLCK;
    fcntl (*file, F_SETLKW, &lock);
    fclose((FILE*)file);
    
    
    signals(SEMbuf); //semaphoric file lock no longer needed
}


void joinThreads(pthread_t *tids,int l){
	for(int i = 0; i < l ; i++) pthread_join(tids[i],NULL);
}

int charcount(char *str,char c){
 int i = 0;char *s = str;
  for(s = shm; *s != '<'; s++)
  	if(*s == '>')i++;
  return i;
}

void put(char *str,String *buf){
	buf[cur_put].str = strdup(str);
	if(logging==1)printf("\n%s -> was placed into the buffer",buf[cur_put].str);
	cur_put++;
	
	if(cur_put == BUFFERSIZE) 
		cur_put = 0;
}

char *getitem(String *buf){
	int temp = cur_get;
	cur_get++;
	if(cur_get == BUFFERSIZE) 
		cur_get = 0;
	return buf[temp].str;
}

void destroyBuffer(String *buffer){
	for(int i = 0; i < BUFFERSIZE; i++){
		free(buffer[i].str);
	}
	free(buffer);
}


int random_number(int min_num, int max_num)
{
	int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num)
	{
	    low_num = min_num;
	    hi_num = max_num + 1; // include max_num in output
	} else {
	    low_num = max_num + 1; // include max_num in output
	    hi_num = min_num;
	}

	srand(time(NULL));
	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
}
