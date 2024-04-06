#include <iostream>
#include <vector>
#include <semaphore.h> // Inclui a biblioteca para semáforos

#define MAX_CAPACITY 100 // Define a capacidade máxima padrão da fila

namespace queue_block {

    template<typename T>
    class blocking_queue_circular {
    private:
        sem_t sem_full; // Semáforo para indicar que a fila está cheia
        sem_t sem_empty; // Semáforo para indicar que a fila está vazia
        std::vector<T> queue_; // Vetor para armazenar os elementos da fila
        size_t capacity_; // Capacidade máxima da fila
        size_t front_; // Índice da frente da fila
        size_t back_; // Índice da parte de trás da fila
        size_t count_; // Contador de elementos na fila

    public:
        // Construtor padrão, inicializa a fila com capacidade máxima padrão
        blocking_queue_circular() : capacity_(MAX_CAPACITY), front_(0), back_(0), count_(0) {
            sem_init(&sem_full, 0, 0); // Inicializa semáforo full com 0 (indicando que a fila está vazia)
            sem_init(&sem_empty, 0, MAX_CAPACITY); // Inicializa semáforo empty com a capacidade máxima
            queue_.resize(MAX_CAPACITY); // Redimensiona o vetor para a capacidade máxima
        }

        // Construtor que permite especificar a capacidade máxima da fila
        blocking_queue_circular(size_t capacity) : capacity_(capacity), front_(0), back_(0), count_(0) {
            sem_init(&sem_full, 0, 0); // Inicializa semáforo full com 0
            sem_init(&sem_empty, 0, capacity); // Inicializa semáforo empty com a capacidade especificada
            queue_.resize(capacity); // Redimensiona o vetor para a capacidade especificada
        }

        // Método para inserir um elemento na fila
        void enqueue(const T& data) {
            sem_wait(&sem_empty); // Aguarda até que haja espaço disponível na fila
            queue_[back_] = data; // Insere o elemento na posição de trás da fila
            back_ = (back_ + 1) % capacity_; // Atualiza o índice da parte de trás
            ++count_; // Incrementa o contador de elementos na fila
            sem_post(&sem_full); // Sinaliza que a fila não está mais vazia
        }

        // Método para remover e retornar o elemento da frente da fila
        T dequeue() {
            sem_wait(&sem_full); // Aguarda até que haja elementos disponíveis na fila
            T front(queue_[front_]); // Obtém o elemento da frente da fila
            front_ = (front_ + 1) % capacity_; // Atualiza o índice da frente
            --count_; // Decrementa o contador de elementos na fila
            sem_post(&sem_empty); // Sinaliza que há espaço disponível na fila
            return front; // Retorna o elemento removido
        }

        // Método para acessar o elemento da frente da fila sem removê-lo
        T front() {
            sem_wait(&sem_full); // Aguarda até que haja elementos disponíveis na fila
            T front(queue_[front_]); // Obtém o elemento da frente da fila
            sem_post(&sem_full); // Sinaliza que a fila não está mais vazia
            return front; // Retorna o elemento da frente
        }

        // Método para acessar o elemento da parte de trás da fila sem removê-lo
        T back() {
            sem_wait(&sem_full); // Aguarda até que haja elementos disponíveis na fila
            T back(queue_[back_]); // Obtém o elemento da parte de trás da fila
            sem_post(&sem_full); // Sinaliza que a fila não está mais vazia
            return back; // Retorna o elemento da parte de trás
        }

        // Método para obter o tamanho atual da fila
        size_t size() {
            return count_; // Retorna o contador de elementos
        }

        // Método para verificar se a fila está vazia
        bool empty() {
            return count_ == 0; // Retorna verdadeiro se o contador de elementos for zero
        }

        // Método para definir a capacidade máxima da fila
        void setCapacity(size_t new_capacity) {
            if (new_capacity < count_) {
                front_ = (front_ + (count_ - new_capacity)) % capacity_; // Ajusta o índice da frente se a nova capacidade for menor que o número atual de elementos
                count_ = new_capacity; // Atualiza o contador de elementos
            }

            capacity_ = new_capacity; // Atualiza a capacidade máxima
            queue_.resize(new_capacity); // Redimensiona o vetor para a nova capacidade
            sem_init(&sem_empty, 0, new_capacity - count_); // Recria o semáforo empty com o novo valor
            sem_post(&sem_full); // Sinaliza que a fila não está mais vazia
        }

        // Método para obter a capacidade máxima da fila
        size_t getCapacity() {
            return capacity_; // Retorna a capacidade máxima
        }

        // Método para limpar a fila
        void clear() {
            front_ = 0; // Reseta o índice da frente
            back_ = 0; // Reseta o índice da parte de trás
            count_ = 0; // Reseta o contador de elementos
            sem_post(&sem_full); // Sinaliza que a fila não está mais vazia
        }

        // Destrutor, libera os recursos alocados para os semáforos
        ~blocking_queue_circular() {
            sem_destroy(&sem_full); // Destroi o semáforo full
            sem_destroy(&sem_empty); // Destroi o semáforo empty
        }
    };
};