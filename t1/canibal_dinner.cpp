// Include necessary libraries and headers
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include <optional>

// Use namespace std for convenience
using std::string;
using std::thread;
using std::vector;   
using std::atomic;
using std::cout;
using std::endl;
using std::to_string;
using std::move; 
using std::mutex;
using std::unique_lock;
using std::condition_variable;

// Define the mutexes and condition variables
std::mutex mtx;
std::condition_variable prato_pobre;
std::condition_variable prato_cheio;

// Define the Comida class
struct Comida {
private:
    bool foiComida;
    int numComida;
public:

    Comida() {
        foiComida = false;
        numComida = 0;
    }

    void comer() {
        foiComida = true;
    }


    bool getfoiComida() {
        return foiComida;
    }
 
    int getNumComida() {
        return numComida;
    }   

    void setNumComida(int ingredient) {
        numComida = ingredient;
    }
};

queue::blocking_queue<Comida> comida_queue = queue::blocking_queue<Comida>(10);

// Define the Cozinheiro class
class Cozinheiro : public wrapper::ThreadTask {
private:
    bool primeira_execucao = true;
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    // Override the operator() function
    void operator()() { 
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);

            if(!primeira_execucao) {
                prato_pobre.wait(lock);
            }

            for (int i = 1; i <= comida_queue.getCapacity(); i++) {
                Comida comida = Comida();
                comida.setNumComida(i);
                cout << "O Cozinheiro de num" << ThreadTask::num_thread << " está cozinhando a comida de num" << comida.getNumComida() << "\n";
                comida_queue.enqueue(comida);
            }

            cout << "O Cozinheiro " << ThreadTask::num_thread << " já cozinhou e vai dormir" << "\n";
            prato_cheio.notify_one();

            primeira_execucao == true ? primeira_execucao = false : primeira_execucao = true;
        }
        return;
    };
};


// Define the Canibal class
class Canibal : public wrapper::ThreadTask {
public:   
    // Inherit the constructor

    using ThreadTask::ThreadTask;

    // Override the operator() function
    void operator()() { 
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);

            if(comida_queue.size() == 0) {
                cout  << "O Canibal de numero  " << ThreadTask::num_thread << " não comeu e vai bater no cozinheiro." << "\n";
                prato_pobre.notify_one();
                // Se não houver comida, verifica se ainda há cozinheiros ativos
                if(ThreadTask::numCozinheirosAtivos != 0) {
                    cout << "Não há comida disponível e não há cozinheiros presentes. Canibal " << ThreadTask::num_thread << " vai sair." << "\n";
                    break;
                }

                prato_cheio.wait(lock);
            }

            auto yum = comida_queue.dequeue();
            cout  << "O Canibal " << ThreadTask::num_thread << " está comendo a comida de num" << yum.getNumComida() << "\n";
            yum.comer(); 
            cout << "O Canibal " << ThreadTask::num_thread << " comeu a comida de num" << yum.getNumComida() << "\n";
        }
        return;
    };
};

// Function to manage the threads
void management(int thr_can, int thr_coz) {
    vector<wrapper::threadWrapper> allThreads;

    wrapper::ThreadTask::numCozinheirosAtivos = thr_coz;

    for(int i = 0; i < thr_coz; i++) {
        Cozinheiro coz = Cozinheiro(thr_coz, i);
        allThreads.push_back(move(wrapper::threadWrapper(thread(coz))));
    }


    for(int j = 0; j < thr_can; j++) {
        Canibal can = Canibal(thr_can, j);
        allThreads.push_back(move(wrapper::threadWrapper(thread(can))));
    }

}

// Main function
int main(int argc, char const *argv[]) {
    // Check command line arguments
    int canibais;
    int cozinheiros;
    int comidas;

    if(argc == 2 || argc == 3 || argc > 4) {
        cout << "Usage2: " << argv[0] << " <threads_canibais> <threads_cozinheiro> <porcoes_comida>" << "\n";
        return 1;
    }

    canibais = atoi(argv[1]);
    cozinheiros = atoi(argv[2]);
    comidas = atoi(argv[3]);

    if(comidas < 1 || canibais < 1 || cozinheiros < 1) {
        cout << "Usage: " << argv[0] << " <greater_than0> <greater_than0> <greater_than0>" << "\n";
        return 1;
    }
    
    comida_queue.setCapacity(static_cast<size_t>(comidas));

    // Manage the threads
    management(canibais, cozinheiros);

    return 0;
}