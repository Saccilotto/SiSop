#pragma once
#include <iostream>
#include <assert.h>    

#include <queue>
#include <mutex>
#include <condition_variable>

#define MAX_CAPACITY 10

namespace queue {
    template<typename T>
    class blocking_queue {
        
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

        blocking_queue() :
            mtx(), 
            full(), 
            empty_(), 
            queue_(MAX_CAPACITY),
            capacity_(MAX_CAPACITY),
            front_(0),
            back_(0),
            count_(0)
        {}

        blocking_queue(size_t capacity) :
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
                empty_.wait(lock );
            }

            assert(!queue_.empty());
            T back(queue_.back());
            return back;
        }

        size_t Size() {
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
                // If the new capacity is smaller than the current size of the queue,
                // remove items from the front of the queue until it fits in the new capacity.
                front_ = (front_ + (count_ - new_capacity)) % capacity_;
                count_ = new_capacity;
            }
            capacity_ = new_capacity;
            // Create a new vector with the new capacity and move the items to it.
            std::vector<T> new_queue(new_capacity);
            for (size_t i = 0; i < count_; ++i) {
                new_queue[i] = std::move(queue_[(front_ + i) % capacity_]);
            }
            queue_ = std::move(new_queue);
            front_ = 0;
            back_ = count_;
        }
    };
};
