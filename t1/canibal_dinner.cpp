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

mutex::peterson_gen peterson_canibal;
mutex::peterson_gen peterson_cozinheiro;
mutex::peterson_gen peterson_fome;

struct Comida {
    bool foiComida;

};

queue::blocking_queue<Comida> comida_queue = queue::blocking_queue<Comida>(10);


// Define the Consumer class
class Cozinheiro : public wrapper::ThreadTask {
    public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    // Override the operator() function
    void operator()() { 


        return;
    };
};

// Define the Consumer class
class Canibal : public wrapper::ThreadTask {
    public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    void comer() {
        peterson_fome.lock(ThreadTask::num_thread);

    
        std::ostringstream ss;
        ss << std::this_thread::get_id();
        cout << "Thread: " << ss.str() << " é o Canibal " << ThreadTask::num_thread << "e está comendo" << endl;

        peterson_fome.unlock(ThreadTask::num_thread);
    }

    // Override the operator() function
    void operator()() { 


        return;
    };
};

// Function to manage the threads
void management(int thr_can, int thr_coz) {
    vector<wrapper::threadWrapper> allThreads;

    for(int i = 0; i < thr_can; i++) {
        Cozinheiro prod = Cozinheiro(thr_can, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(prod))));
    }

    for(int j = 0; j < thr_coz; j++) {
        Canibal cons = Canibal(thr_coz, j);
        allThreads.push_back(move(wrapper::threadWrapper(thread(cons))));
    }
}

// Main function
int main(int argc, char const *argv[]) {
    // Check command line arguments
    int canibais;
    int cozinheiros;
    int comidas;


    if (argc == 4) {
        canibais = atoi(argv[1]);
        cozinheiros = atoi(argv[2]);
        comidas = atoi(argv[3]);    
    } else {
        if (argc  != 1) {
            cout << "Usage1: " << argv[0] << "for predefined inputs <canibais == 10>  <cozinheiro = 1>  <comida = 5>"  << endl;

            cout << "Usage2: " << argv[0] << " <threads_canibais> <threads_cozinheiro> <num_comida>" << endl;

            return 1;
        } else {
            canibais = 10;
            cozinheiros = 1;
            comidas = 5;
        }
    }
    
    comida_queue.setCapacity(comidas);

    peterson_canibal = mutex::peterson_gen(canibais);
    peterson_cozinheiro = mutex::peterson_gen(cozinheiros);
    peterson_fome = mutex::peterson_gen(); 

    // Manage the threads
    management(canibais, cozinheiros);

    return 0;
}