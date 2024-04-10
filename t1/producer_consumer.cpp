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
using queue_block::blocking_queue_circular;

// Define the length of the data
int data_length = 1000;

// Define Peterson's mutex for general use and another for 2 threads use
mutex_peterson::peterson_gen peterson_operator_prod;
mutex_peterson::peterson_gen peterson_operator_cons;
mutex_peterson::peterson_gen peterson_data;

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
        peterson_data.lock(0);
        return id;
        peterson_data.unlock(0);
    }

    // Get the isLast flag
    bool getIsLast() { 
        peterson_data.lock(0);
        return isLast;
        peterson_data.unlock(0);
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
        cout << print_message << "\n";
        peterson_data.unlock(0);
    }
};

// Initialize the static ID counter
int Data::id = 0;
// Define the blocking queue for data
queue_block::blocking_queue_circular<Data> data_queue = queue_block::blocking_queue_circular<Data>();

// Define the Producer class
class Producer : public wrapper::ThreadTaskProdCons {
    public:   
    // Inherit the constructor
    using ThreadTaskProdCons::ThreadTaskProdCons;

    // Override the operator() function
    void operator()() override { 
        while(true) {
            peterson_operator_prod.lock(ThreadTaskProdCons::current_thread);
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            Data new_data = Data();
            if(new_data.getIsLast()) {
                data_queue.enqueue(new_data);
                new_data.setMessage("Produced by thread id " + ss.str() + " that is the " + to_string(ThreadTaskProdCons::current_thread) + "th Producer thread");
                new_data.print();
                break;
            } 
            new_data.setMessage("Produced by thread id " + ss.str() + " that is the " + to_string(ThreadTaskProdCons::current_thread) + "th Producer thread");    
            new_data.print();
            data_queue.enqueue(new_data);
            peterson_operator_prod.unlock(ThreadTaskProdCons::current_thread);
        }
        cout << "Estou morrendo PRODUCER :(" << to_string(ThreadTaskProdCons::current_thread) << "\n";
        return;
    };  
};

// Define the Consumer class
class Consumer : public wrapper::ThreadTaskProdCons {
    public:   
    // Inherit the constructor
    using ThreadTaskProdCons::ThreadTaskProdCons;

    // Override the operator() function
    void operator()() override { 
        while(true) {
            peterson_operator_cons.lock(ThreadTaskProdCons::current_thread);
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            Data aux = data_queue.dequeue();
            if(aux.getIsLast()) {
                aux.setMessage("Consumed by thread id " + ss.str() + " that is the " + to_string(ThreadTaskProdCons::current_thread) + "th Consumer thread");    
                aux.print();
                break;
            }
            
            aux.setMessage("Consumed by thread id " + ss.str() + " that is the " + to_string(ThreadTaskProdCons::current_thread) + "th Consumer thread");    
            aux.print();
            peterson_operator_cons.unlock(ThreadTaskProdCons::current_thread);
        }
        cout << "Estou morrendo CONSUMER :(" << to_string(ThreadTaskProdCons::current_thread) << "\n";
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

    // Initialize the Peterson's mutexes
    peterson_operator_prod = mutex_peterson::peterson_gen(num_producer_threads); // for the N producer threads
    peterson_operator_cons = mutex_peterson::peterson_gen(num_consumer_threads); // for the M consumer thread
    peterson_data = mutex_peterson::peterson_gen();  // for syncronization of the data acess (1 thread from prod and cons at a time)

    // Manage the threads
    management(num_producer_threads, num_consumer_threads);

    return 0;
}