#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include "mutexPeterson.hpp"

using namespace concurrent::queue;
using namespace concurrent::wrapper;
using namespace concurrent::mutex; // TODO exchange the mutex usage to Peterson's algorithm in this file
using namespace std;

int data_length = 1000;
mutex mng_lock;

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
        id >= data_length ? isLast = true : isLast = false;
    }

    void nextId() {
        id += 1;
    }

    int getCurrentId() {
        return id;
    }

    bool getIsLast() { 
        return isLast;
    }  

    void reset () {
        id = 0;
    }

    void setMessage(string message) {
        this->message = message;
    }

    void print() {
        string print_message = message + " item: " + to_string(getCurrentId());
        cout << print_message << endl;
    }
};

blocking_queue<Data> data_queue = blocking_queue<Data>();
int Data::id = 0;

class Producer {

public:   
    int threads;

    ~Producer() = default;
    Producer(const Producer&) = default;// copy constructor
    Producer(Producer&& other) noexcept  = default;
    Producer& operator=(const Producer& other) = default; // copy assignment
    Producer& operator=(Producer&& other) noexcept  = default;// move assignment

    Producer(int _threads):
        threads(_threads)
    {}

    void operator()() { 
        while(true) {
            Data new_data = Data();
            new_data.setMessage("Produced");    
            if(new_data.getIsLast()) {
                int i = 0;
                while(i < threads) {
                    new_data.print();
                    data_queue.enqueue(new_data);
                    ++i;
                    Data new_data = Data();
                    new_data.setMessage("Produced");
                }
                break;
            } 
            data_queue.enqueue(new_data);
            new_data.print();
        }
        return;
    };  
    
};

class Consumer {

public:   
    int threads;

    ~Consumer() = default;
    Consumer(const Consumer&) = default;// copy constructor
    Consumer(Consumer&& other) noexcept  = default;
    Consumer& operator=(const Consumer& other) = default; // copy assignment
    Consumer& operator=(Consumer&& other) noexcept  = default;// move assignment

    Consumer(int _threads):
        threads(_threads)
    {}

    void operator()() { 
        int last_itens_counter = 0;
        while (true) {
            Data aux = data_queue.dequeue();
            aux.setMessage("Consumed");
            if(aux.getIsLast()) {
                last_itens_counter++;
                if (last_itens_counter == threads) {
                    aux.print();
                    break;
                } 
            }
            aux.print();
        }
        return;
    };
};

void management(int threads_prod, int threads_cons) {
    vector<threadWrapper> allThreads;

    Producer prod = Producer(threads_prod);
    Consumer cons = Consumer(threads_cons);

    for(int i = 0; i < threads_prod; i++) {
        allThreads.push_back(move(threadWrapper(thread(prod))));
    }

    for(int i = 0; i < threads_cons; i++) {
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

    if(argc == 4) {
        data_length = atoi(argv[3]);
    }

    management(numthreads1, numthreads2);

    return 0;
}
