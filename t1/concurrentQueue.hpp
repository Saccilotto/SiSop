#pragma once
#include <iostream>
#include <assert.h>    

#include <queue>
#include <mutex>
#include <condition_variable>

#define MAX_CAPACITY 20

using namespace std;

namespace concurrent {
    namespace queue {

    template<typename T>
        class blocking_queue {
        private:
            //DISABLE_COPY_AND_ASSIGN(blocking_queue);
            blocking_queue(const blocking_queue& rhs);
            blocking_queue& operator=(const blocking_queue& rhs);

        private:
            mutable mutex mtx;
            condition_variable full_;
            condition_variable empty_;
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
                unique_lock<mutex> lock(mtx);
                while(queue_.size() == capacity_){
                    full_.wait(lock);
                }

                assert(queue_.size() < capacity_);
                queue_.push(data);
                empty_.notify_all(); 
            }

            // pops from the front of the queue
            T dequeue() {
                unique_lock<mutex> lock(mtx);
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
                unique_lock<mutex> lock(mtx);
                while(queue_.empty()) {
                    empty_.wait(lock);
                }

                assert(!queue_.empty());
                T front(queue_.front());
                return front;
            }

            T back() {
                unique_lock<mutex> lock(mtx);
                while(queue_.empty()) {
                    empty_.wait(lock);
                }

                assert(!queue_.empty());
                T back(queue_.back());
                return back;
            }

            size_t Size() {
                lock_guard<mutex> lock(mtx);
                return queue_.size();
            }

            bool empty() {
                unique_lock<mutex> lock(mtx);
                return queue_.empty();
            }

            void setCapacity(const size_t capacity) {
                capacity_ = (capacity > 0 ? capacity : MAX_CAPACITY);
            }
        };
    }
}