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
#include <optional>

// Use namespace std for convenience
using std::string;
using std::thread;
using std::vector;
using std::cout;
using std::endl;
using std::to_string;
using std::move;

// Define the mutexes and condition variables
mutex::peterson_gen peterson_fome;
std::mutex empty_lock;
std::condition_variable prato_pobre;

// Define the Comida class
struct Comida {
private:
    bool foiComida;
    static int instanceCount;
public:
    int ingredientesOver;
    static int numComida;

    Comida() {
        foiComida = false;
        instanceCount++;

        if(instanceCount % numComida == 0) {
            instanceCount = 1;
            ingredientesOver++;
        }
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

    int getIngredientesOver() {
        return ingredientesOver;
    }
};

int Comida::numComida = 1;
int Comida::instanceCount = 1;
queue::blocking_queue<Comida> comida_queue = queue::blocking_queue<Comida>(10);

// Define the Cozinheiro class
class Cozinheiro : public wrapper::ThreadTask {
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    bool cozinhar(Comida ingredientes) {
        std::unique_lock<std::mutex> lock(empty_lock);
        while(comida_queue.empty()) {                  
            if(!comida_queue.empty()) {
                return false;
            }
            prato_pobre.wait(lock);
        }


        for(int i = 0; i < comida_queue.getCapacity(); i++) {
            cout << "O Cozinheiro de num " << ThreadTask::num_thread << " está cozinhando a comida de num" <<  ingredientes.getInstanceCount() <<  "\n";
            comida_queue.enqueue(ingredientes);
        }

        cout << "O Cozinheiro " << ThreadTask::num_thread << " já cozinhou e vai dormir" << "\n";
        return true;
    }

    // Override the operator() function
    void operator()() { 
        while(true) {
            Comida comida = Comida();
            bool cozinhou = cozinhar(comida);

            if(cozinhou) {
                cout << "Cozinheiro de numero " << ThreadTask::num_thread << " retornou da cozinha" << "\n";
            } 

            if(comida.getIngredientesOver() == comida_queue.getCapacity()) {
                cout << "Acabaram os ingredientes e o cozinheiro " << ThreadTask::num_thread << " vai fujir" << "\n";
                break;
            }
        }
        return;
    };
};


// Define the Canibal class
class Canibal : public wrapper::ThreadTask {
public:   
    // Inherit the constructor
    using ThreadTask::ThreadTask;

    std::pair<bool, Comida> comer() {
        std::optional<Comida> comida = comida_queue.dequeue();
        if(!comida.has_value()) {
            cout << "O Canibal " << ThreadTask::num_thread << " ainda não tem comida para comer" << "\n";
            return std::make_pair(false, comida.value());
        }

        cout  << "O Canibal " << ThreadTask::num_thread << " e está comendo a comida de num" << comida.value().getInstanceCount() << "\n";
        comida.value().comer(); // Comer a comida
        cout << "O Canibal " << ThreadTask::num_thread << " e já comeu a comida de num" <<  comida.value().getInstanceCount() << "\n";
        
        bool foiComida = comida.value().getfoiComida();
        return std::make_pair(true, comida.value());
    }

    // Override the operator() function
    void operator()() { 
        while(true) {
            bool comeu; Comida aux;
            std::tie(comeu, aux) = comer();
            if(!comeu) {
                if(aux.getIngredientesOver() == comida_queue.getCapacity()) {
                    cout << "Acabaram os ingredientes e o canibal " << ThreadTask::num_thread << " vai tentar comer o cozinheiro" << "\n";
                    prato_pobre.notify_all();
                    break;
                }
                cout  << "O canibal de numero  " << ThreadTask::num_thread << " não comeu e vai bater no cozinheiro." << "\n";
                prato_pobre.notify_all();
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
    Comida::numComida = comida_queue.getCapacity() + 1;

    peterson_fome = mutex::peterson_gen(); 

    // Manage the threads
    management(canibais, cozinheiros);

    return 0;
}