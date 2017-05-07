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

#include "SafeBuffer.h"
#include <signal.h>


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
struct request_data
{
	std::string body;
	int num;
	SafeBuffer * request_buffer;
};
//request threads put requests to Bounded Buffer
void* request_thread_function(void* arg) {
    struct request_data *data;
	data = (struct request_data *) arg;
	std::string body = data->body;
	int num = data->num;
	SafeBuffer* request_buffer = data->request_buffer; 

	for(int i = 0; i < num; i++) {
		request_buffer->push_back(body);
	}
}

struct worker_data
{
	SafeBuffer * request_buffer;
	std::vector<int> *freq1;
	std::vector<int> *freq2;
    std::vector<int> *freq3;
    std::string host;
    unsigned short port;
    NetworkRequestChannel *chan;
};
//Worker threads create channel to communicate with Server for the clients
void* worker_thread_function(void* arg) {
    struct worker_data *data;
	data = (struct worker_data *) arg;
	SafeBuffer* s_buffer = data->request_buffer;
	std::vector<int> *fq1 = data->freq1;
	std::vector<int> *fq2 = data->freq2;
	std::vector<int> *fq3 = data->freq3;
    std::string host = data->host;
    unsigned short port = data->port;
	NetworkRequestChannel *chan = data->chan;

	// std::string s = chan->send_request("newthread");
    NetworkRequestChannel *workerChannel = new NetworkRequestChannel(host, port);

    while(true) {
		std::string request = s_buffer->retrieve_front();
        std::string response = workerChannel->send_request(request);

        if(request == "data John Smith") {
            fq1->at(stoi(response) / 10) += 1;
        }
        else if(request == "data Jane Smith") {
            fq2->at(stoi(response) / 10) += 1;
        }
        else if(request == "data Joe Smith") {
            fq3->at(stoi(response) / 10) += 1;
        }
       	else if(request == "quit") {
            delete workerChannel;
            break;
        }
    }
}

//Stat threads compute the results
void* stat_thread_function(void* arg) {

}

struct signal_data {
    std::vector<int> *freq1;
	std::vector<int> *freq2;
    std::vector<int> *freq3;
    int w;
    int n;
};

void signal_handler(int signum, siginfo_t *si, void * uc) {

    struct signal_data *data;
	data = (struct signal_data *) si->si_value.sival_ptr;
	std::vector<int> *fq1 = data->freq1;
	std::vector<int> *fq2 = data->freq2;
	std::vector<int> *fq3 = data->freq3;
	int n = data->n;
    int w = data->w;
    
    system("clear");
    std::string histogram_table = make_histogram_table("John Smith",
		        "Jane Smith", "Joe Smith", fq1,
		        fq2, fq3);

    std::cout << histogram_table << std::endl;
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

    // NetworkRequestChannel *chan = new NetworkRequestChannel(host_name, port_no);
    // std::cout << chan->send_request("hello") << endl;
    // std::cout << chan->send_request("data") << endl;
    // std::cout << chan->send_request("quit") << endl;

    std::ofstream ofs;
        if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", std::ios::out | std::ios::app);
        else ofs.open("output.txt", std::ios::out | std::ios::app);

        std::cout << "n == " << n << std::endl;
        std::cout << "b == " << b << std::endl;
        std::cout << "w == " << w << std::endl;

        std::cout << "CLIENT STARTED:" << std::endl;
        std::cout << "Establishing control channel... " << std::flush;
        NetworkRequestChannel *chan = new NetworkRequestChannel(host_name, port_no);
        std::cout << "done." << std::endl;

        /*
        -----------------------------------------------------------------------------------------------------------------
        */

        SafeBuffer request_buffer(b);
        std::vector<int> john_frequency_count(10, 0);
        std::vector<int> jane_frequency_count(10, 0);
        std::vector<int> joe_frequency_count(10, 0);

        // Timer init and starting

        timer_t timerid;
        struct itimerspec its;
        long long freq_nanosecs;
        struct sigevent sev;
        sigset_t mask;
        struct sigaction sa;

        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = signal_handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGALRM, &sa, NULL);

        struct signal_data sig_data;
        sig_data.n = n;
        sig_data.w = w;
        sig_data.freq1 = &john_frequency_count;
        sig_data.freq2 = &jane_frequency_count;
        sig_data.freq3 = &joe_frequency_count;

        sev.sigev_value.sival_ptr = &sig_data;
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIGALRM;

        timer_create(CLOCK_REALTIME, &sev, &timerid);

        freq_nanosecs = 2000000000;
        its.it_value.tv_sec = freq_nanosecs / 1000000000;
        its.it_value.tv_nsec = freq_nanosecs % 1000000000;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;

        timer_settime(timerid, 0, &its, NULL);

        // Create consumer/producer

        // std::cout << "Populating request buffer... ";
        // fflush(NULL);

		pthread_t request_threads[3];
		struct request_data request_args[3];

		request_args[0].body = "data John Smith";
		request_args[1].body = "data Jane Smith";
		request_args[2].body = "data Joe Smith";

		for (int i = 0; i < 3; i++)
		{
			request_args[i].num = n;
			request_args[i].request_buffer = &request_buffer;
			pthread_create(&request_threads[i], NULL, 
				request_thread_function, (void *)&request_args[i]);
		}

        // std::cout << "done." << std::endl;

		/*-------------------------------------------*/
		/* START TIMER HERE */
		/*-------------------------------------------*/

		uint64_t time_diff;
		struct timespec start, end;

		clock_gettime(CLOCK_MONOTONIC, &start);

        pthread_t worker_threads[w];
		struct worker_data worker_args[w];

		for(int i = 0; i < w; i++)
		{
			worker_args[i].request_buffer = &request_buffer;
			worker_args[i].freq1 = &john_frequency_count;
			worker_args[i].freq2 = &jane_frequency_count;
			worker_args[i].freq3 = &joe_frequency_count;
			worker_args[i].chan = chan;
            worker_args[i].host = host_name;
            worker_args[i].port = port_no;
			pthread_create(&worker_threads[i], NULL, 
				worker_thread_function, (void *)&worker_args[i]);
		}
        for (int i = 0; i < 3; i++)
		{
			pthread_join(request_threads[i], NULL);
		}

        // std::cout << "Request threads completed" << std::endl;
        for(int i = 0; i < w; ++i) {
            request_buffer.push_back("quit");
        }

		for (int i = 0; i < w; i++)
		{
			pthread_join(worker_threads[i], NULL);
		}

        timer_delete(timerid);
        system("clear");

        std::cout << "Worker threads completed" << std::endl;

		clock_gettime(CLOCK_MONOTONIC, &end);

		time_diff = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

		std::cout << "Elapsed time: " << (unsigned long long) time_diff / 1000000L << " microseconds." << std::endl;

        std::string histogram_table = make_histogram_table("John Smith",
		        "Jane Smith", "Joe Smith", &john_frequency_count,
		        &jane_frequency_count, &joe_frequency_count);

        std::cout << "Results for n == " << n << ", w == " << w << std::endl;

        std::cout << histogram_table << std::endl;


        /*
        -----------------------------------------------------------------------------------------------------------------
        */

        ofs.close();
        std::cout << "Sleeping..." << std::endl;
        usleep(10000);
        std::string finale = chan->send_request("quit");
        std::cout << "Finale: " << finale << std::endl;
}
