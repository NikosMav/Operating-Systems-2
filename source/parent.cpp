// parent.cpp: the parent program

#include "../headers/aux.h"

using namespace std;

int main(int argc, char *argv[]){
    //First check if semaphores already exist, if thats the case then destroy them.
    //if not then ignore
    sem_unlink(PAR_CRF_CHILD);
    sem_unlink(CHILD_CRF_PAR); 
    sem_unlink(CHILD_USING_PAR);

    //First we must initialize our semaphores between PAR - CHILDREN
    sem_t *par_r_child = sem_open(PAR_CRF_CHILD, O_CREAT, 0644, 0);
    sem_t *child_r_par = sem_open(CHILD_CRF_PAR, O_CREAT, 0644, 0);
    //Create shared memory between PARENT - CHILDREN
    void *par_child_Ptr = createSharedMemory(PAR_CHILD);
    //Create a new semaphore for the children to keep in order
    sem_t *child_using_par = sem_open(CHILD_USING_PAR, O_CREAT, 0644, 1);

    // Get the arguments
    string *fileName = new string(argv[1]);
    int children = atoi(argv[2]);   //K
    int samples = atoi(argv[3]);    //N
    cout << "K Is: " << children << endl;
    cout << "N Is: " << samples << endl;

    // Create a text string, which is used to output the text file
    string myText;
    // Read from the text file
    ifstream MyReadFile(*fileName);
    int lines_of_file = 0;
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, myText)) {
        //count lines
        lines_of_file++;
    }
    cout << "Lines of file: " << lines_of_file << endl;

    //Creating child processes
    string *name = new string("./child");
    pid_t pids[children];
    int i = 0, j;
    for (i = 0; i < children; i++) {
        pids[i] = fork();

        if(pids[i] < 0){  // pid == -1 means error occured

            printf("can't fork, error occured\n"); 
            exit(EXIT_FAILURE);

        }else if(pids[i] == 0){  // pid == 0 we're the child proccess

            // Make argument list and call execv
            char *myargs[] = {(char *)name->c_str(), (char *)to_string(lines_of_file).c_str(), (char *)to_string(samples).c_str(), NULL};
            // Execute the child process
            execvp(myargs[0], myargs);
        }
    }

    // Parent behaviour
    string *taken_line_num;
    char *buffer;
    string *wanted_line = new string();

    //K * N times
    for(j = 0; j < children * samples; j++) {

        //Pause parent from reading until child writes..
        sem_wait(par_r_child);
        //after child writes, parent must read from child the given line
        buffer = readMessage(par_child_Ptr);

        taken_line_num = new string(buffer);
        free(buffer);
        //cout << "PARENT: Taken line number is: " << *taken_line_num << endl;

        // Find the line
        MyReadFile.clear();
        MyReadFile.seekg(0, MyReadFile.beg);
        for(i=0; i < lines_of_file; i++) {
            getline (MyReadFile, *wanted_line);

            //break condition
            if(i == stoi(*taken_line_num) - 1) {
                break;
            }
        }

        //Now that we got the wanted line we have to write it back to the child
        writeMessage(wanted_line, par_child_Ptr);
        //Now that write process is complete, child can read
        sem_post(child_r_par);
    }


    /* Wait for children to exit. */
    int status;
    pid_t pid;
    for(i = 0; i < children; i++){
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
    }

    //remove the shared memory object and semaphores
    sem_close(par_r_child);
    sem_close(child_r_par);
    sem_close(child_using_par);

    shm_unlink(PAR_CHILD);
    sem_unlink(PAR_CRF_CHILD);
    sem_unlink(CHILD_CRF_PAR); 

    // Close the file
    MyReadFile.close();

    return  0;
}