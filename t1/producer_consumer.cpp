#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"

using namespace std;
using namespace concurrent::queue;
using namespace concurrent::wrapper;

static int data_length = 1000;

struct Data {

private:
    static int id;
public:
    bool isLast;
    int CURRENT_ID;
    string message;

    Data() {
        CURRENT_ID = id;
        message = to_string(CURRENT_ID);
        setIsLast(data_length);
        nextId();
    };

    Data(string message) {
       CURRENT_ID = id;
       this->message = message; 
       nextId();
    };

    void setIsLast(int length) {
        CURRENT_ID >= length - 1 ? isLast = true : isLast = false;
    }

    void reset() {
        id = 0;
    }

    void nextId() {
        id += 1;
    }

    int getCurrentId() {
        return CURRENT_ID;
    }

    bool getIsLast() { 
        return isLast;
    }  

    void setMessage(string message) {
        this->message = message;
    }

    void print() {
        cout << " Message: " << message << endl;
    }
};


int Data::id = 0;
mutex cout_lock;
blocking_queue<Data> data_queue;

class Producer {

public:   

    Producer(){}

    void operator()() { 
        while(true)
        {
            Data new_data;
            if (new_data.getIsLast()) {
                data_queue.enqueue(new_data);
                cout_lock.lock();
                cout << "Produced item: "  << new_data.getCurrentId() << endl;
                cout_lock.unlock();
                break;
            }
            data_queue.enqueue(new_data);
            cout_lock.lock();
            cout << "\nProduced item: "  << new_data.getCurrentId() << endl;
            cout_lock.unlock();
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
            cout << "\nConsumed item: " << aux.getCurrentId() << endl;
            cout_lock.unlock();
            if(aux.getIsLast()) {
               break;
            }
        }
        return;
    };
};

void management(int threads_prod, int threads_cons) {
    vector<threadWrapper> allThreads;

    for(int i = 0; i < threads_prod; i++) {
        Producer prod = Producer();
        allThreads.push_back(move(threadWrapper(thread(prod))));
    }

    for(int i = 0; i < threads_cons; i++) {
        Consumer cons = Consumer();
        allThreads.push_back(move(threadWrapper(thread(cons))));
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

    management(numthreads1, numthreads2);

    return 0;
}
