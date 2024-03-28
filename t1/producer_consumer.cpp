// Include necessary libraries and headers
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include "peterson.hpp"

// Use namespace std for convenience
using std::string;
using std::thread;
using std::vector;
using std::cout;
using std::endl;
using std::to_string;
using std::move;

// Define the length of the data
int data_length = 1000;

// Define Peterson's mutex for general use and another for specific use
mutex::peterson_gen peterson_operator;
mutex::peterson_gen peterson_data;

// Define the Data structure
struct Data {
    // Private members
    private:
        static int id;  // Static ID counter

    // Public members
    public:
        bool isLast;  // Flag to indicate if this is the last data
        string message;  // Message of the data

        // Default constructor
        Data() {
            message = to_string(id);
            setIsLast();
            nextId();
        };

        // Constructor with message
        Data(string message) {
            this->message = message; 
            setIsLast();
            nextId();
        };

        // Set the isLast flag
        void setIsLast() {
            peterson_data.lock(0);
            id >= data_length - 1 ? isLast = true : isLast = false;
            peterson_data.unlock(0);   
        }

        // Increment the ID
        void nextId() {
            peterson_data.lock(0);
            id += 1;
            peterson_data.unlock(0);
        }

        // Get the current ID
        int getCurrentId() {
            return id;
        }

        // Get the isLast flag
        bool getIsLast() { 
            return isLast;
        }  

        // Set the message
        void setMessage(string message) {
            peterson_data.lock(0);
            this->message = message;
            peterson_data.unlock(0);
        }   

        // Print the data
        void print() {
            peterson_data.lock(0);
            string print_message = "item: " + to_string(getCurrentId()) + " | " + message + " | isLast: " + to_string(isLast);
            cout << print_message << endl;
            peterson_data.unlock(0);
        }
};

// Initialize the static ID counter
int Data::id = 0;
// Define the blocking queue for data
queue::blocking_queue<Data> data_queue = queue::blocking_queue<Data>();

// Define the base class for thread tasks
class ThreadTask {
    public:
        int num_thread;  // Number of threads
        int local_counter;  // Local counter

        // Destructor, copy constructor, move constructor, copy assignment operator, move assignment operator
        ~ThreadTask() = default;
        ThreadTask(const ThreadTask&) = default;
        ThreadTask(ThreadTask&& other) noexcept = default;
        ThreadTask& operator=(const ThreadTask& other) = default;
        ThreadTask& operator=(ThreadTask&& other) noexcept = default;

        // Constructor
        ThreadTask(int _threads, int _counter) :
            num_thread(_threads),
            local_counter(_counter)
        {}

        // Pure virtual function for operator()
        virtual void operator()() = 0;
};

// Define the Producer class
class Producer : public ThreadTask {
    public:   
        // Inherit the constructor
        using ThreadTask::ThreadTask;

        // Override the operator() function
        void operator()() override { 
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            while(true) {
                Data new_data = Data();
                new_data.setMessage("Produced by thread id " + ss.str() + " that is the " + to_string(ThreadTask::num_thread) + "th Producer thread");    
                if(new_data.getIsLast()) {
                    data_queue.enqueue(new_data);
                    peterson_operator.lock(ThreadTask::num_thread);
                    new_data.print();
                    peterson_operator.lock(ThreadTask::num_thread);
                    break;
                } 
                data_queue.enqueue(new_data);
                peterson_operator.lock(ThreadTask::num_thread);
                new_data.print();
                peterson_operator.lock(ThreadTask::num_thread);
            }
            return;
        };  
};

// Define the Consumer class
class Consumer : public ThreadTask {
    public:   
        // Inherit the constructor
        using ThreadTask::ThreadTask;

        // Override the operator() function
        void operator()() { 
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            while(true) {
                Data aux = data_queue.dequeue();
                aux.setMessage("Produced by thread id " + ss.str() + " that is the " + to_string(ThreadTask::num_thread) + "th Consumer thread");    
                if(aux.getIsLast()) {
                    int i = 0;
                    peterson_operator.lock(0);
                    aux.print();
                    peterson_operator.unlock(0);
                    break;
                }
                peterson_operator.lock(0);
                aux.print();
                peterson_operator.unlock(0);
            }
            return;
        };
};

// Function to manage the threads
void management(int threads_prod, int threads_cons) {
    vector<wrapper::threadWrapper> allThreads;

    for(int i = 0; i < threads_prod; i++) {
        Producer prod = Producer(threads_prod, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(prod))));
    }

    for(int j = 0; j < threads_cons; j++) {
        Consumer cons = Consumer(threads_cons, j);
        allThreads.push_back(move(wrapper::threadWrapper(thread(cons))));
    }
}

// Main function
int main(int argc, char const *argv[]) {
    // Check command line arguments
    if(argc != 4 && argc != 3) {
        cout << "Usage1: " << argv[0] << " <threads_producer> <threads_consumer>" << endl;
        cout << "Usage2: " << argv[0] << " <threads_producer> <threads_consumer> <number_of_data_items>" << endl;
        return 1;
    }

    // Set the data length if provided
    if(argc == 4) {
        data_length = atoi(argv[3]);
    }

    // Get the number of producer and consumer threads
    int num_producer_threads = atoi(argv[1]);
    int num_consumer_threads = atoi(argv[2]);
    int total_threads = num_producer_threads + num_consumer_threads;

    // Initialize the Peterson's mutexes
    peterson_operator = mutex::peterson_gen(total_threads);
    peterson_data = mutex::peterson_gen();


    // Manage the threads
    management(num_producer_threads, num_consumer_threads);

    return 0;
}