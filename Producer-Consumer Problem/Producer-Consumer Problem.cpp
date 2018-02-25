/**            作業要求            **
* producer(void *param)及consumer(void *param)傳入的param為ID(ProducerID、ConsumerID)，1, 2, …
* RR=rand()的範圍為1~2ID (最大值為99)，為sleep()的秒數
* item=PPNNNRR，其中PP為ProducerID(1,2,…)，NNN為SequenceNumber(001, 002, …)，RR為亂數
* 測試：120  5  4  (main() sleep 120秒，5個producers，4個consumers)
/**                                **/


//main.cpp  
#include <stdio.h>  
#include <pthread.h>  
#include <assert.h> 
#include <string>
#include <time.h>
#include <Windows.h>
#include <string>
#include <math.h>


#pragma comment(lib,"pthreadVC2.lib") 
#define BUFFER_SIZE 5
#define DATA_LENGTH 8


/* the buffer */
char *buffer[BUFFER_SIZE]={0};
HANDLE MUTEX = CreateSemaphore(NULL, 1, 1, NULL);
//The second and thrid parameters indicate the initial value and maximun value of the semaphore
HANDLE FULL = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);;
HANDLE EMPTY = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);

int in = 0;
int out = 0;
void insert_item(char *data) {
    buffer[in] = data;
    in = (in + 1) % BUFFER_SIZE;
	//return 0;
	//return -1;//超出BUFFER大小
}

char *remove_item() {
	char *BufferData;
	BufferData = buffer[out];
	out = (out + 1) % BUFFER_SIZE;
	return BufferData;
	//return -1;//BUFFER內無東西
}


int SequenceNumber = 0;
void *producer(void *id) {
    int semid = (int) id;
	int RR; 
	int RR_Range_Max = pow(2.0,(double)semid); // RR=rand()的範圍為1~2^ID (最大值為99)，為sleep()的秒數
	char *data;
	char string[7];
	//sprintf(string,"%d %d", semid, RR);
	//printf("%s", string);
	//data = string;
    while(1) {
		RR = (rand()%RR_Range_Max)+1; //範圍為1~2^ID (最大值為99)
		if(RR > 99){
			RR = 99;
		}
		Sleep(RR*1000);

		WaitForSingleObject(EMPTY, INFINITE);
		WaitForSingleObject(MUTEX, INFINITE);

        /** Critical Section **/
		sprintf(string,"%02d %03d %02d", semid , SequenceNumber, RR); //item=PPNNNRR，其中PP為ProducerID(1,2,…)，NNN為SequenceNumber(001, 002, …)，RR為亂數
		data = string;
        insert_item(data);
		SequenceNumber++;
        printf("Producer%d produces item : %s\n",semid ,data);
        /**             **/

        ReleaseSemaphore(MUTEX, 1, NULL);
		ReleaseSemaphore(FULL, 1, NULL);
		//Sleep(RR*1000);
    }
    pthread_exit(NULL);
	return 0;
}

void *consumer(void *id) {
    int semid = (int) id;
	int RR; 
	int RR_Range_Max = pow(2.0,(double)semid); // RR=rand()的範圍為1~2^ID (最大值為99)，為sleep()的秒數
	char *data;
    while(1)
    {
		RR = (rand()%RR_Range_Max)+1; //範圍為1~2^ID (最大值為99)
		if(RR > 99){
			RR = 99;
		}
		Sleep(RR*1000);

		WaitForSingleObject(FULL, INFINITE);
		WaitForSingleObject(MUTEX, INFINITE);

		/** Critical Section     **/    
		data = remove_item();
		printf("Consumer%d consumes item : %s\n", semid, data);
		/**             **/

		ReleaseSemaphore(MUTEX, 1, NULL);
		ReleaseSemaphore(EMPTY, 1, NULL);
		//Sleep(RR*1000);
    }    

    pthread_exit(NULL);
	return 0;
}
 

void CreateProducerThread(int id)
{
	pthread_t prod;
	pthread_create(&prod, NULL, producer, (void *)id);
}

void CreateConsumerThread(int id)
{
	pthread_t prod;
	pthread_create(&prod, NULL, consumer, (void *)id);
}

int    main    (int    argc,    char    *argv[])    
{
	srand(GetTickCount());
	int semid = 1;
	int MainSleep=0;
	int ProducerCount=0;
	int ConsumerCount=0;

	printf("請輸入: main的sleep秒數 producer數量 consumer數量 ");
	scanf("%d%d%d", &MainSleep, &ProducerCount, &ConsumerCount);

	int i;
	for(i=1 ; i<ProducerCount+1 ; i++)
	{
		CreateProducerThread(i);
		Sleep(500);//讓隨機能更隨機
	}

	for(i=1 ; i<ConsumerCount+1 ; i++)
	{
		CreateConsumerThread(i);
		Sleep(600);//讓隨機能更隨機
	}
	Sleep(MainSleep*1000);
	
    //pthread_exit(NULL);
    return    0;
}