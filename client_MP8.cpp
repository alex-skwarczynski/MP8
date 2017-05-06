/*
    File: client_MP8.cpp

    Author: J. Higginbotham
    Department of Computer Science
    Texas A&M University
    Date  : 2016/05/21

    Based on original code by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */
/* -- This might be a good place to put the size of
    of the patient response buffers -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
    As in MP8 no additional includes are required
    to complete the assignment, but you're welcome to use
    any that you think would help.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include "reqchannel.h"

/*
    This next included file will need to be written from scratch, along with
    semaphore.h and (if you choose) their corresponding .cpp files.
 */

//#include "bounded_buffer.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
    All *_params structs are optional,
    but they might help since you still
    can't use global variables.
 */

/*
    This class can be used to write to standard output
    in a multithreaded environment. It's primary purpose
    is printing debug messages while multiple threads
    are in execution. You probably saw the explanation of
    this in MP8, it hasn't changed since then so it
    won't be repeated.
 */
class atomic_standard_output {
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    ~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
    void print(std::string s) {
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string make_histogram_table(std::string name1, std::string name2,
                                 std::string name3, std::vector<int> *data1, std::vector<int> *data2,
                                 std::vector<int> *data3) {
    std::stringstream tablebuilder;
    tablebuilder << std::setw(25) << std::right << name1;
    tablebuilder << std::setw(15) << std::right << name2;
    tablebuilder << std::setw(15) << std::right << name3 << std::endl;
    for (int i = 0; i < data1->size(); ++i) {
        tablebuilder << std::setw(10) << std::left
                     << std::string(
                         std::to_string(i * 10) + "-"
                         + std::to_string((i * 10) + 9));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data1->at(i));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data2->at(i));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data3->at(i)) << std::endl;
    }
    tablebuilder << std::setw(10) << std::left << "Total";
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data1->begin(), data1->end(), 0);
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data2->begin(), data2->end(), 0);
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data3->begin(), data3->end(), 0) << std::endl;

    return tablebuilder.str();
}

/*
    You'll need to fill these in.
*/
//request threads put requests to Bounded Buffer
void* request_thread_function(void* arg) {

}

//Worker threads create channel to communicate with Server for the clients
void* worker_thread_function(void* arg) {

}

//Stat threads compute the results
void* stat_thread_function(void* arg) {

}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 10; //default number of requests per "patient"
    int b = 50; //default size of request_buffer
    int w = 10; //default number of worker threads
    string host_name="";
    unsigned short port_no = 0;
    bool USE_ALTERNATE_FILE_OUTPUT = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:b:w:mh:p:")) != -1) {
        switch (opt) {
        case 'n':
            n = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 'w':
            w = atoi(optarg);
            break;
        case 'm':
            USE_ALTERNATE_FILE_OUTPUT = true;
            break;
        case 'h':
        	host_name=optarg;
        	break;
        case 'p':
            port_no = (unsigned short) atoi(optarg);
            break;
        default:
            std::cout << "This program can be invoked with the following flags:" << std::endl;
            std::cout << "-n [int]: number of requests per patient" << std::endl;
            std::cout << "-b [int]: maximum number of requests that will be allowed in the request buffer" << std::endl;
            std::cout << "-w [int]: number of worker threads" << std::endl;
            std::cout << "-m: use output2.txt instead of output.txt for all file output" << std::endl; //purely for convenience, you may find it helpful since you have to make two graphs instead of one, and they require different data
            std::cout << "-h: print this message and quit" << std::endl;
            std::cout << "Example: ./client_solution -n 10000 -b 50 -w 120 -m" << std::endl;
            std::cout << "If a given flag is not used, a default value will be given" << std::endl;
            std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
            std::cout << "behavior is the same as using the -h flag." << std::endl;
            exit(0);
        }
    }

    struct timeval start_time;
    struct timeval finish_time;
    int64_t start_usecs;
    int64_t finish_usecs;

    /*
    Do what you did in MP7, but remember we use NetworkReqChannel Here.
    */

    NetworkRequestChannel *chan = new NetworkRequestChannel(host_name, port_no);
}
