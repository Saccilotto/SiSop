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
std::mutex cozinheiro_mtx;
std::condition_variable cozinheiro_cv;

// Define the Comida class
struct Comida {
private:
    bool foiComida;
    static int numComida;
public:

    Comida() {
        foiComida = false;
        numComida++;
    }

    void comer() {
        foiComida = true;
    }


    bool getfoiComida() {
        return foiComida;
    }

    void setnumComida(int a) {
        numComida = a;
    }
 
    static int getNumComida() {
        return numComida;
    }   
};

int Comida::numComida = 0;
queue::blocking_queue<Comida> comida_queue = queue::blocking_queue<Comida>(10);

// Define the Cozinheiro class
class Cozinheiro : public wrapper::ThreadTask {
private:
    bool primeira_execucao = true;
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    static int pratosCozinhados;

    // Override the operator() function
    void operator()() { 
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);

            Comida comida;

            while (!primeira_execucao) {
                prato_pobre.wait(lock);
            }

            for (int i = 0; i < comida_queue.getCapacity(); i++) {
                comida = Comida();
                cout << "O Cozinheiro de num" << ThreadTask::num_thread << " está cozinhando a comida de num" << comida.getNumComida() << "\n";
                comida_queue.enqueue(comida);
                pratosCozinhados++;
                {
                    std::lock_guard<std::mutex> lock(cozinheiro_mtx);
                    cozinheiro_cv.notify_all();
                }
            }

            cout << "O Cozinheiro " << ThreadTask::num_thread << " já cozinhou e vai dormir" << "\n";
            prato_cheio.notify_all();

            int aux  = comida_queue.getCapacity() *10;
            if(pratosCozinhados >= aux) {
                cout << "Acabaram os ingredientes e o cozinheiro " << ThreadTask::num_thread << " vai fujir" << "\n";
                ThreadTask::numCozinheirosAtivos--;
                prato_pobre.notify_all();
                break;
            }

            if (primeira_execucao) {
                primeira_execucao = false;
            }
        }
        return;
    };
};

int Cozinheiro::pratosCozinhados = 0;

// Define the Canibal class
class Canibal : public wrapper::ThreadTask {
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    static int pratosComidos;

    // Override the operator() function
    void operator()() { 
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);

            if(comida_queue.size() == 0) {
                // Se não houver comida, verifica se ainda há cozinheiros ativos
                if(ThreadTask::numCozinheirosAtivos == 0) {
                    // Se não houver cozinheiros ativos e não houver comida, os canibais podem sair
                    cout << "Não há comida disponível e não há cozinheiros ativos. Canibal " << ThreadTask::num_thread << " vai sair." << "\n";
                    break;
                } else {
                    // Se houver cozinheiros ativos, aguarde até que haja comida disponível
                    prato_cheio.wait(lock);
                    continue; // Volta para o início do loop para verificar novamente a disponibilidade de comida
                }
            }

            {
                std::unique_lock<std::mutex> lock(cozinheiro_mtx);
                cozinheiro_cv.wait(lock, [] {
                        return ThreadTask::numCozinheirosAtivos == 0; 
                });
            }
            Comida comida = comida_queue.dequeue();
            

            cout  << "O Canibal " << ThreadTask::num_thread << " está comendo a comida de num" << comida.getNumComida() << "\n";
            comida.comer(); 
            pratosComidos++;

            int aux  = comida_queue.getCapacity() *10;
            if(pratosComidos >=  aux) {
                cout << "Acabaram os ingredientes e o canibal " << ThreadTask::num_thread << " vai tentar comer o cozinheiro" << "\n";   
                break;
            }

            cout  << "O canibal de numero  " << ThreadTask::num_thread << " não comeu e vai bater no cozinheiro." << "\n";
            prato_pobre.notify_all();
        }
        prato_pobre.notify_all();
        return;
    };
};

int Canibal::pratosComidos = 0;

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