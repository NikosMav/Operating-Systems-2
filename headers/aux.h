#ifndef AUX
#define AUX

#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <chrono>

#define SIZE 100

#define PAR_CRF_CHILD "/par_can_read_from_child"
#define CHILD_CRF_PAR "/child_can_read_from_parent"
#define CHILD_USING_PAR "/child_using_parent"

#define PAR_CHILD "PARCHILD"

using namespace std;

void *createSharedMemory(const char* name);
void *openSharedMemory(const char* name);
void writeMessage(string *message, void *shmPtr);
char* readMessage(void *shmPtr);



#endif