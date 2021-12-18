// child.cpp: the child program

#include "../headers/aux.h"

using namespace std;

int main(int argc, char *argv[]) {

    //Open already created semaphores
    sem_t *child_r_par = sem_open(CHILD_CRF_PAR, 0);
    sem_t *par_r_child = sem_open(PAR_CRF_CHILD, 0);
    sem_t *child_using_par = sem_open(CHILD_USING_PAR, 0);
    //Open shared memory between PARENT - CHILDREN
    void *par_child_Ptr = openSharedMemory(PAR_CHILD);

    //Get arguments
    int lines_of_file = atoi(argv[1]);
    int samples = atoi(argv[2]);

    //Ask a random line from parent
    string *wanted_line = new string();
    string *taken_line;
    int wanted_line_num, i;
    char *buffer;

    /* initialize random seed for more randomness between proccesses: */
    srand(time(NULL) ^ (getpid()<<16));

    // Child run time
    int run_time = 0;

    for(i = 0; i < samples; i++) {
        //Entering Critical point
        //Down the semaphore so that no other child uses shared memory until process is finished
        sem_wait(child_using_par);

        /* generate secret number between 1 and X: */
        wanted_line_num = rand() % lines_of_file + 1;
        //cout << "CHILD: Wanted line number is: " << wanted_line_num << endl;
        //Convert it to string
        *wanted_line = to_string(wanted_line_num);

        //First child must write number to shared memory to parent
        writeMessage(wanted_line, par_child_Ptr);

        //After child has writen the message, start the timer
        auto start = chrono::steady_clock::now();

        //Now that write process is complete, parent can read
        sem_post(par_r_child);

        //Pause child from reading until parent writes
        sem_wait(child_r_par);
        //after parent writes, child must read the taken line that parent returned to child
        buffer = readMessage(par_child_Ptr);

        // After child has read the given message, stop the timer
        auto  end = chrono::steady_clock::now();

        // Add to  run time
        run_time = run_time + chrono::duration_cast<chrono::microseconds>(end - start).count();

        // Print message
        taken_line = new string(buffer);
        free(buffer);
        cout << "CHILD: Received line is: " << *taken_line << endl;

        //After critical point up the semaphore
        sem_post(child_using_par);
    }

    // Print average time
    cout << "Average time in microseconds for child: " << getpid() << " is: " << run_time / samples << "μs" << endl; 

    return 0;
}