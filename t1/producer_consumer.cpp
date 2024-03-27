#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include "mutexPeterson.hpp"

using namespace std;
using namespace concurrent::queue;
using namespace concurrent::wrapper;
using namespace concurrent::mutex; // TODO exchange the mutex usage to Peterson's algorithm in this file

int data_length = 1000;
mutex cout_lock;

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
        cout_lock.lock();
        id >= data_length - 1 ? isLast = true : isLast = false;
        cout_lock.unlock();
    }


    void nextId() {
        cout_lock.lock();
        id += 1;
        cout_lock.unlock();
    }

    int getCurrentId() {
        return id;
    }

    bool getIsLast() { 
        return isLast;
    }  

    void setMessage(string message) {
        this->message = message;
    }

    void print(string s) {
        cout << s << " item: " << getCurrentId() << endl;
    }
};


int Data::id = 0;
blocking_queue<Data> data_queue;

class Producer {

public:   

    Producer(){}

    void operator()() { 
        while(true)
        {
            Data new_data = Data();
            if(new_data.getIsLast()) {
                data_queue.enqueue(new_data);
                cout_lock.lock();
                new_data.print("\nProduced");
                cout_lock.unlock();
                break;
            } else {
                data_queue.enqueue(new_data);
                cout_lock.lock();
                new_data.print("\nProduced");
                cout_lock.unlock();
            }
        }
        return;
    };  
    
};

class Consumer {

public:   

    Consumer(){}

    void operator()() { 
        while (true) {
            Data aux = data_queue.dequeue();
            cout_lock.lock();
            aux.print("\nConsumed");
            cout_lock.unlock();
            if(aux.getIsLast()) {
               break;
            }
        }
        return;
    };
};

void management(int threads_prod, int threads_cons) {
    vector<threadWrapper> prod_threads;
    vector<threadWrapper> cons_threads;

    for(int i = 0; i < threads_prod; i++) {
        Producer prod = Producer();
        prod_threads.push_back(move(threadWrapper(thread(prod))));
    }

    for(int i = 0; i < threads_cons; i++) {
        Consumer cons = Consumer();
        cons_threads.push_back(move(threadWrapper(thread(cons))));
    }
}

int main(int argc, char const *argv[]) {
    if(argc  != 4 && argc != 3) {
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
