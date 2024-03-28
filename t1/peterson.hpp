#pragma once

namespace mutex {   
   
    class peterson2 {  // Implementação de mutex de Peterson para duas threads
        volatile bool flag[2];  // Flags indicando se cada thread quer entrar na seção crítica
        volatile int turn;  // Indica qual thread tem a vez de entrar na seção crítica

    public:
        peterson2() {
            flag[0] = false;
            flag[1] = false;
            turn = 0;
        }

        // Método para solicitar acesso à seção crítica
        void lock(int t) {
            flag[t] = true;  // Indica que a thread t quer entrar na seção crítica
            turn = t;  // Dá a vez para a thread t
            // Espera enquanto a outra thread quer entrar na seção crítica e tem a vez
            while (flag[1 - t] && turn == t) {} 
        }
            
        // Método para liberar a seção crítica
        void unlock(int t) {
            flag[t] = false;  // Indica que a thread t não quer mais entrar na seção crítica
        }
    };

    class peterson_gen { // Implementação de mutex de Peterson para n threads
    private:
        volatile int* level = 0;  // Níveis das threads no torneio
        volatile int* waiting = 0;  // Threads que estão esperando em cada nível
        int size;  // armazena o tamnho do vetor level e waiting

    public:
        // Construtor padrão para duas threads
        peterson_gen() : size(2){ 
            level = new int[2];
            waiting = new int[2];
            level[0] = 0;
            level[1] = 0;
            waiting[0] = 0;
            waiting[1] = 0;
        }

        // Construtor para n threads
        peterson_gen(int n) : size(n) {
            level = new int[n];
            waiting = new int[n];
            // Inicializa todos os níveis e esperas para 0
            for (int i = 0; i < n; i++) {
                level[i] = 0;
                waiting[i] = 0;
            }
        }

        // Copy constructor
        peterson_gen(const peterson_gen& other) : size(other.size) {
            level = new int[size];
            waiting = new int[size];
            // Copia os níveis e esperas do objeto other
            for (int i = 0; i < size; i++) {
                level[i] = other.level[i];
                waiting[i] = other.waiting[i];
            }
        }

        // Copy assignment operator
        peterson_gen& operator=(const peterson_gen& other) {
            if (this != &other) {  // Proteção contra auto-atribuição
                delete[] level;  // Deleta os dados antigos
                delete[] waiting;

                int n = other.size;
                level = new int[n];  // Aloca novos dados
                waiting = new int[n];
                // Copia os níveis e esperas do objeto other
                for (int i = 0; i < n; i++) {  
                    level[i] = other.level[i];
                    waiting[i] = other.waiting[i];
                }
            }
            return *this;
        }

        // Método para solicitar acesso à seção crítica
        void lock(int thr) {
            // Para cada nível do torneio
            for (int lev = 1; lev < size; lev++) {   
                level[thr] = lev;  // A thread thr está no nível lev
                waiting[lev] = thr;  // A thread thr está esperando no nível lev
                bool found = false;             
                // Verifica se todas as outras threads estão no nível lev ou acima
                for (int koutras = 0; koutras < size; koutras++) {   
                    if (koutras != thr) {  // Exceto a thread thr        
                        found = level[koutras] >= lev && waiting[lev] == thr;   
                        if (found) break;  // Se uma thread não está no nível lev ou acima, espera                         
                    }
                }
                if (!found) return;
            }
        }

        // Método para liberar a seção crítica
        void unlock(int thr) {
            level[thr] = 0;  // A thread thr não está mais no torneio
        }

        // Destrutor
        ~peterson_gen() {
            delete[] level;  // Deleta os níveis
            delete[] waiting;  // Deleta as esperas
        }
    };
};