#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include "mutexPeterson.hpp"

using std::string;
using std::thread;
using std::vector;
using std::cout;
using std::endl;
using std::to_string;
using std::move;

int data_length = 1000;
mutex::peterson2 peterson_mutex = mutex::peterson2();

struct Data {

private:
    static int id;
public:
    bool isLast;
    string message;

    Data() {
        message = to_string(id);
        setIsLast();
        nextId();
    };

    Data(string message) {
       this->message = message; 
       setIsLast();
       nextId();
    };

    void setIsLast() {
        peterson_mutex.lock(0);
        id >= data_length - 1 ? isLast = true : isLast = false;
        peterson_mutex.unlock(0);   
    }

    void nextId() {
        peterson_mutex.lock(0);
        id += 1;
        peterson_mutex.unlock(0);
    }

    int getCurrentId() {
        return id;
    }

    bool getIsLast() { 
        return isLast;
    }  

    void setMessage(string message) {
        peterson_mutex.lock(0);
        this->message = message;
        peterson_mutex.lock(0);
    }   

    void print() {
        peterson_mutex.lock(0);
        string print_message = message + "\nitem: " + to_string(getCurrentId());
        cout << print_message << endl;
        peterson_mutex.unlock(0);
    }

};

queue::blocking_queue<Data> data_queue = queue::blocking_queue<Data>();
int Data::id = 0;

class Producer {

public:   
    int threads;
    int local_counter;

    ~Producer() = default;
    Producer(const Producer&) = default;// copy constructor
    Producer(Producer&& other) noexcept  = default;
    Producer& operator=(const Producer& other) = default; // copy assignment
    Producer& operator=(Producer&& other) noexcept  = default;// move assignment

    Producer(int _threads, int _counter):
        threads(_threads),
        local_counter(_counter)
    {}

    void operator()() { 
        std::ostringstream ss;
        ss << std::this_thread::get_id();
        while(true) {
            Data new_data = Data();
            new_data.setMessage("\nProduced by thread id " + ss.str() + " that is the " + to_string(local_counter) + "th Producer thread");    
            if(new_data.getIsLast()) {
                int i = 0;
                data_queue.enqueue(new_data);
                peterson_mutex.lock(0);
                new_data.print();
                peterson_mutex.lock(0);
                break;
            } 
            data_queue.enqueue(new_data);
            peterson_mutex.lock(0);
            new_data.print();
            peterson_mutex.lock(0);
        }
        return;
    };  
    
};

class Consumer {

public:   
    int threads;
    int local_counter;

    ~Consumer() = default;
    Consumer(const Consumer&) = default;// copy constructor
    Consumer(Consumer&& other) noexcept  = default;
    Consumer& operator=(const Consumer& other) = default; // copy assignment
    Consumer& operator=(Consumer&& other) noexcept  = default;// move assignment

    Consumer(int _threads, int _counter):
        threads(_threads),
        local_counter(_counter)
    {}

    void operator()() { 
        std::ostringstream ss;
        ss << std::this_thread::get_id();
        while(true) {
            Data aux = data_queue.dequeue();
            aux.setMessage("\nProduced by thread id " + ss.str() + " that is the " + to_string(local_counter) + "th Consumer thread");    
            if(aux.getIsLast()) {
                int i = 0;
                peterson_mutex.lock(0);
                aux.print();
                peterson_mutex.unlock(0);
                break;
            }
            peterson_mutex.lock(0);
            aux.print();
            peterson_mutex.unlock(0);
        }
        return;
    };

};

void management(int threads_prod, int threads_cons) {
    vector<wrapper::threadWrapper> allThreads;

    for(int i = 0; i < threads_prod; i++) {
        Producer prod = Producer(threads_prod, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(prod))));
    }

    for(int i = 0; i < threads_cons; i++) {
        Consumer cons = Consumer(threads_cons, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(cons))));
    }

}

int main(int argc, char const *argv[]) {

    if(argc != 4 && argc != 3) {
        cout << "Usage1: " << argv[0] << " <threads_producer> <threads_consumer>" << endl;
        cout << "Usage2: " << argv[0] << " <threads_producer> <threads_consumer> <number_of_data_items>" << endl;
        return 1;
    }

    int numthreads1 = atoi(argv[1]);
    int numthreads2 = atoi(argv[2]);

    if(argc == 4) {
        data_length = atoi(argv[3]);
    }

    management(numthreads1, numthreads2);

    return 0;
}
