// Include necessary libraries and headers
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <condition_variable>
#include <mutex>

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

// Define the mutexes
mutex::peterson_gen peterson_canibal;
mutex::peterson_gen peterson_cozinheiro;
mutex::peterson_gen peterson_fome;
std::mutex empty_lock;
std::condition_variable prato_pobre;

// Define the Comida class
struct Comida {
private:
    bool foiComida;
    static int instanceCount;

public:

    Comida() {
        foiComida = false;
        instanceCount = instanceCount % 5 + 1;
    }

    void comer() {
        foiComida = true;
    }

    static int getInstanceCount() {
        return instanceCount;
    }

    bool getfoiComida() {
        return foiComida;
    }
};

int Comida::instanceCount = 1;
queue::blocking_queue<Comida> comida_queue = queue::blocking_queue<Comida>(10);

// Define the Canibal class
class Canibal : public wrapper::ThreadTask {
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    bool comer() {
        peterson_fome.lock(ThreadTask::num_thread);
        // std::ostringstream ss;
        // ss << std::this_thread::get_id();
        Comida comida = comida_queue.dequeue(); // Pegar comida
        cout /*<< "Thread " /(<< ss.str() */ << " é o Canibal " << ThreadTask::num_thread << " e está comendo a comida de num" << comida.getInstanceCount() << endl;
        comida.comer(); // Comer a comida
        cout /*<< "Thread " << ss.str() */<< " é o Canibal " << ThreadTask::num_thread << " e já comeu a comida de num" <<  comida.getInstanceCount() << endl;
        peterson_fome.unlock(ThreadTask::num_thread);
        return comida.getfoiComida();
    }

    // Override the operator() function
    void operator()() { 
        while(true) {
            bool comeu = comer();
            peterson_fome.lock(ThreadTask::num_thread);
            // std::ostringstream ss;
            // ss << std::this_thread::get_id();
            if(comeu) {
                ;
            } else {
                cout /* << "Thread Canibal " << ss.str() <<  */  << " de numero  " << ThreadTask::num_thread << " não comeu e vai bater no cozinheiro." << endl;
                prato_pobre.notify_one();
            }
        }
        return;
    };
};

// Define the Consumer class
class Cozinheiro : public wrapper::ThreadTask {
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    bool cozinhar() {
        std::unique_lock<std::mutex> lock(empty_lock);
        while(comida_queue.empty()) {
            prato_pobre.wait(lock);
        }
        std::ostringstream ss;
        ss << std::this_thread::get_id();
        
        for(int i = 0; i < comida_queue.getCapacity(); i++) {
            Comida comida = Comida();
            cout /*<< "Thread: " << ss.str() */ << " é o Cozinheiro " << ThreadTask::num_thread << " e está cozinhando a comida de num" <<  comida.getInstanceCount() <<  endl;
            comida_queue.enqueue(comida);
        }
        cout /* << "Thread: " << ss.str() << */ << " é o Cozinheiro " << ThreadTask::num_thread << " já cozinhou e vai dormir" << endl;
        return true;
    }

    // Override the operator() function
    void operator()() { 
        while(true) {
            bool cozinhou = cozinhar();
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            if(cozinhou) {
                cout /* << "A Thread Cozinheiro: " */ << ss.str() << " de numero " << ThreadTask::num_thread << " cozinhou" << endl;
            } 
        }
        return;
    };
};

// Function to manage the threads
void management(int thr_can, int thr_coz) {
    vector<wrapper::threadWrapper> allThreads;

    for(int j = 0; j < thr_can; j++) {
        Canibal can = Canibal(thr_can, j);
        allThreads.push_back(move(wrapper::threadWrapper(thread(can))));
    }

    for(int i = 0; i < thr_coz; i++) {
        Cozinheiro coz = Cozinheiro(thr_coz, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(coz))));
    }

}

// Main function
int main(int argc, char const *argv[]) {
    // Check command line arguments
    int canibais;
    int cozinheiros;
    int comidas;

    if(argc == 2 || argc == 3 || argc > 4) {
        cout << "Usage2: " << argv[0] << " <threads_canibais> <threads_cozinheiro> <porcoes_comida>" << endl;
        return 1;
    }

    canibais = atoi(argv[1]);
    cozinheiros = atoi(argv[2]);
    comidas = atoi(argv[3]);
    
    comida_queue.setCapacity(static_cast<size_t>(comidas));

    peterson_canibal = mutex::peterson_gen(canibais);
    peterson_cozinheiro = mutex::peterson_gen(cozinheiros);
    peterson_fome = mutex::peterson_gen(); 

    // Manage the threads
    management(canibais, cozinheiros);

    return 0;
}