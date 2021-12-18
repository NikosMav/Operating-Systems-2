#include "../headers/aux.h"

using namespace std;

void *createSharedMemory(const char* name){
    //First create the shared memory
    int shm_fd;
    void* shmPtr;
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    if(shm_fd == -1){
        cout << "ERROR cannot create memory" << endl;
        return NULL;
    }

    //configure the size of the shared memory object
    ftruncate(shm_fd, SIZE);
    //memory map the shared memory object
    shmPtr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    return shmPtr;
}

void *openSharedMemory(const char* name){
    //First create the shared memory
    int shm_fd;
    void* shmPtr;
    shm_fd = shm_open(name, O_RDWR, 0666); 

    if(shm_fd == -1){
        cout << "ERROR cannot open memory" << endl;
        return NULL;
    }

    //memory map the shared memory object
    shmPtr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    return shmPtr;
}

char* readMessage(void *shmPtr){
    //first read what the messages' length is
    int stringLength;
    memcpy(&stringLength, (char *)shmPtr, sizeof(stringLength));

    //then read the actual message
    //allocate the same space
    char* buffer = (char *)malloc(sizeof(char) * (stringLength + sizeof(char)));
    memcpy(buffer, (char *)shmPtr + sizeof(stringLength), stringLength + 1);

    return buffer;
}

void writeMessage(string *message, void* shmPtr){
    //first write the length of the string we are about to write
    int stringLength = message->length();
    memcpy((char *)shmPtr, &stringLength, sizeof(stringLength));

    //Write to the shared memory object
    memcpy((char *)shmPtr + sizeof(stringLength), message->c_str(), stringLength);

    //add the zero to the string
    char zero_char = '\0';
    memcpy((char *)shmPtr + sizeof(stringLength) + stringLength, &zero_char, sizeof(char));
}