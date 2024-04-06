// Include necessary libraries and headers
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>

#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"
#include "peterson.hpp"

// namespaces for convenience
using std::string;
using std::thread;
using std::vector;   
using std::cout;
using std::endl;
using std::move; 

mutex_peterson::peterson_gen mtx2_coz = mutex_peterson::peterson_gen();     // default initialized with 2 
mutex_peterson::peterson_gen mtx2_can = mutex_peterson::peterson_gen();
mutex_peterson::peterson_gen mtxCan;
// Define the Comida class
struct Comida {
private:
    bool foiComida;
    int numComida;
    static int canibal;
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

queue_block::blocking_queue_circular<Comida> travessa = queue_block::blocking_queue_circular<Comida>();

// Define the Cozinheiro class
class Cozinheiro : public wrapper::ThreadTaskCanibal {
public:   
    using ThreadTaskCanibal::ThreadTaskCanibal;

    // Override the operator() function
    void operator()() override { 
        while(true) {
            mtx2_coz.lock(0);
            std::ostringstream ss;
            ss << std::this_thread::get_id();

            for (int i = 1; i <= travessa.getCapacity(); i++) {
                Comida comida = Comida();
                comida.setNumComida(i);
                cout << "O Cozinheiro de num" << ss.str() << " está cozinhando a comida de num" << comida.getNumComida() << "\n";
                travessa.enqueue(comida);
                ThreadTaskCanibal::travessasAtual++;
            }

            if(travessa.size() == travessa.getCapacity()) {
                cout << "O Cozinheiro " << ss.str() << " encheu o prato e vai dormir" << "\n";
                mtx2_can.unlock(0);
            }
        }
        return;
    };
};

// Define the Canibal class
class Canibal : public wrapper::ThreadTaskCanibal {
public:   
    // Inherit the constructor

    using ThreadTaskCanibal::ThreadTaskCanibal;

    // Override the operator() function
    void operator()() override { 
        while(true) {
            mtx2_can.lock(0);
            mtxCan.lock(ThreadTaskCanibal::canibal);
            std::ostringstream ss;
            ss << std::this_thread::get_id();
            auto yum = travessa.dequeue();
            if(travessa.empty()) {
                mtx2_coz.unlock(0);
            }
            cout  << ss.str() << " O Canibal " << ThreadTaskCanibal::canibal << " está comendo a comida de num" << yum.getNumComida() << "\n";
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
            yum.comer(); 
            cout << ss.str() << " O Canibal " << ThreadTaskCanibal::canibal << " comeu a comida de num" << yum.getNumComida() << "\n";
            mtxCan.unlock(ThreadTaskCanibal::canibal);
        }
        return;
    };
};

// Function to manage the threads
void management(int thr_can, int pratos) {
    vector<wrapper::threadWrapper> allThreads;

    Cozinheiro coz = Cozinheiro(1, 0, pratos);
    allThreads.push_back(move(wrapper::threadWrapper(thread(coz))));

    for(int j = 1; j <= thr_can; j++) {
        Canibal can = Canibal(thr_can, j, pratos);
        allThreads.push_back(move(wrapper::threadWrapper(thread(can))));
    }

}

// Main function
int main(int argc, char const *argv[]) {
    // Check command line arguments
    int canibais;
    int comidas;

    if(argc != 3) {
        cout << "Usage2: " << argv[0] << " <threads_canibais> <porcoes_comida>" << "\n";
        return 1;
    }

    canibais = atoi(argv[1]);
    comidas = atoi(argv[2]);

    if(comidas < 1 || canibais < 1) {
        cout << "Usage: " << argv[0] << " <greater_than0> <greater_than0>" << "\n";
        return 1;
    }

    travessa.setCapacity(comidas);
    mtxCan = mutex_peterson::peterson_gen(canibais);
    // Manage the threads
    management(canibais, comidas);

    return 0;
}
