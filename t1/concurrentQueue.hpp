#pragma once
#include <iostream>
#include <assert.h>    
#include <queue>
#include <mutex>
#include <condition_variable>

#define MAX_CAPACITY 100

namespace queue_block {

    template<typename T>
    class blocking_queue_circular {
        
        private:
            mutable std::mutex mtx;
            std::condition_variable full;
            std::condition_variable empty_;
            std::vector<T> queue_;
            size_t capacity_;
            size_t front_;
            size_t back_;
            size_t count_;

        public:

        blocking_queue_circular() :
            mtx(), 
            full(), 
            empty_(), 
            queue_(MAX_CAPACITY),
            capacity_(MAX_CAPACITY),
            front_(0),
            back_(0),
            count_(0)
        {}

        blocking_queue_circular(size_t capacity) :
            mtx(), 
            full(), 
            empty_(), 
            queue_(capacity),
            capacity_(capacity),
            front_(0),
            back_(0),
            count_(0)
        {}

        void enqueue(const T& data) {
            std::unique_lock<std::mutex> lock(mtx);
            while(count_ == capacity_){
                full.wait(lock);
            }

            assert(count_ < capacity_);
            queue_[back_] = data;
            back_ = (back_ + 1) % capacity_;
            ++count_;
            empty_.notify_one(); 
        }

        T dequeue() {
            std::unique_lock<std::mutex> lock(mtx);
            while(count_ == 0) {
                empty_.wait(lock);
            }

            assert(count_ > 0);
            T front(queue_[front_]);
            front_ = (front_ + 1) % capacity_;
            --count_;
            full.notify_one();
            return front;
        }

        T front() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock);
            }

            assert(!queue_.empty());
            T front(queue_.front());
            return front;
        }

        T back() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock);
            }

            assert(!queue_.empty());
            T back(queue_.back());
            return back;
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mtx);
            return queue_.size();
        }

        bool empty() {
            std::unique_lock<std::mutex> lock(mtx);
            return queue_.empty();
        }

        void setCapacity(size_t new_capacity) {
            std::lock_guard<std::mutex> lock(mtx);
            if (new_capacity < count_) {
                front_ = (front_ + (count_ - new_capacity)) % capacity_;
                count_ = new_capacity;
            }
        
            capacity_ = new_capacity;
            std::vector<T> new_queue(new_capacity);
            
            for (size_t i = 0; i < count_; ++i) {
                new_queue[i] = std::move(queue_[(front_ + i) % capacity_]);
            }
            queue_ = std::move(new_queue);
            front_ = 0;
            back_ = count_;
        }

        size_t getCapacity() {
            std::lock_guard<std::mutex> lock(mtx);
            return capacity_;
        }

        void clear() {
            std::lock_guard<std::mutex> lock(mtx);
            front_ = 0;
            back_ = 0;
            count_ = 0;
        }
    };

    template<typename T>
    class blocking_queue {
    private:
        //DISABLE_COPY_AND_ASSIGN(blocking_queue);
        blocking_queue(const blocking_queue& rhs);
        blocking_queue& operator= (const blocking_queue& rhs);

    private:
        mutable std::mutex mtx;
        std::condition_variable full_;
        std::condition_variable empty_;
        std::queue<T> queue_;
        size_t capacity_; 

    public:
        blocking_queue() :
            mtx(), 
            full_(), 
            empty_(), 
            capacity_(MAX_CAPACITY) 
        {}
        
        void enqueue(const T& data) {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.size() == capacity_){
                full_.wait(lock);
            }

            assert(queue_.size() < capacity_);
            queue_.push(data);
            empty_.notify_all(); 
        }
        // pops from the front of the queue
        T dequeue() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock);
            }

            assert(!queue_.empty());
            T front(queue_.front()); // store the value
            queue_.pop(); // pop it from start
            full_.notify_all();
            return front;
        }

        T front() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock );
            }

            assert(!queue_.empty());
            T front(queue_.front());
            return front;
        }

        T back() {
            std::unique_lock<std::mutex> lock(mtx);
            while(queue_.empty()) {
                empty_.wait(lock );
            }

            assert(!queue_.empty());
            T back(queue_.back());
            return back;
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mtx);
            return queue_.size();
        }

        bool empty() {
            std::unique_lock<std::mutex> lock(mtx);
            return queue_.empty();
        }

        size_t getCapacity() {
            std::lock_guard<std::mutex> lock(mtx);
            return capacity_;
        }   

        void setCapacity(const size_t capacity) {
            capacity_ = (capacity > 0 ? capacity : MAX_CAPACITY);
        }
    };
};
