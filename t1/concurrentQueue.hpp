#include <iostream>
#include <vector>
#include <semaphore.h>

#define MAX_CAPACITY 100

namespace queue_block {

    template<typename T>
    class blocking_queue_circular {
    private:
        sem_t sem_full;
        sem_t sem_empty;
        std::vector<T> queue_;
        size_t capacity_;
        size_t front_;
        size_t back_;
        size_t count_;

    public:
        blocking_queue_circular() : capacity_(MAX_CAPACITY), front_(0), back_(0), count_(0) {
            sem_init(&sem_full, 0, 0);
            sem_init(&sem_empty, 0, MAX_CAPACITY);
            queue_.resize(MAX_CAPACITY);
        }

        blocking_queue_circular(size_t capacity) : capacity_(capacity), front_(0), back_(0), count_(0) {
            sem_init(&sem_full, 0, 0);
            sem_init(&sem_empty, 0, capacity);
            queue_.resize(capacity);
        }

        void enqueue(const T& data) {
            sem_wait(&sem_empty);
            queue_[back_] = data;
            back_ = (back_ + 1) % capacity_;
            ++count_;
            sem_post(&sem_full);
        }

        T dequeue() {
            sem_wait(&sem_full);
            T front(queue_[front_]);
            front_ = (front_ + 1) % capacity_;
            --count_;
            sem_post(&sem_empty);
            return front;
        }

        T front() {
            sem_wait(&sem_full);
            T front(queue_[front_]);
            sem_post(&sem_full);
            return front;
        }

        T back() {
            sem_wait(&sem_full);
            T back(queue_[back_]);
            sem_post(&sem_full);
            return back;
        }

        size_t size() {
            return count_;
        }

        bool empty() {
            return count_ == 0;
        }

        void setCapacity(size_t new_capacity) {
            if (new_capacity < count_) {
                front_ = (front_ + (count_ - new_capacity)) % capacity_;
                count_ = new_capacity;
            }

            capacity_ = new_capacity;
            queue_.resize(new_capacity);
            sem_init(&sem_empty, 0, new_capacity - count_);
            sem_post(&sem_full); // para garantir que sem_empty seja ajustado 
        }

        size_t getCapacity() {
            return capacity_;
        }

        void clear() {
            front_ = 0;
            back_ = 0;
            count_ = 0;
            sem_post(&sem_full); // para garantir que sem_empty seja ajustado 
        }

        ~blocking_queue_circular() {
            sem_destroy(&sem_full);
            sem_destroy(&sem_empty);
        }
    };
};
