#pragma once
#include <functional>
#include <iostream>
#include <thread>

namespace wrapper {
	/* class threadWrapper */
	class threadWrapper {
		std::thread th;

	public:
		//Delete the copy constructor
		threadWrapper(const threadWrapper&) = delete;
		//Delete the Assignment operator
		threadWrapper& operator=(const threadWrapper&) = delete;
		
		//Parameterized Constructor
		threadWrapper(std::thread _t): 
			th(move(_t)) 
		{}

		// Move Constructor
		threadWrapper(threadWrapper && obj): 
			th(move(obj.th)) 
		{}

		//Move Assignment Operator
		threadWrapper & operator=(threadWrapper && obj) {
			if (th.joinable()) {
				th.join();
			}
			th = move(obj.th);
			return *this;
		}

		// Destructor
		~threadWrapper() {
			if (th.joinable()) {
				th.join();
			}
		}	
	};

	// Define the base class for thread tasks
	class ThreadTask {
		public:
			int num_thread;  // Number of threads
			int local_counter;  // Local counter
			static int numCozinheirosAtivos;


			// Destructor, copy constructor, move constructor, copy assignment operator, move assignment operator
			~ThreadTask() = default;
			ThreadTask(const ThreadTask&) = default;
			ThreadTask(ThreadTask&& other) noexcept = default;
			ThreadTask& operator=(const ThreadTask& other) = default;
			ThreadTask& operator=(ThreadTask&& other) noexcept = default;

			// Constructor
			ThreadTask(int _threads, int _counter) :
				num_thread(_threads),
				local_counter(_counter)
			{}

			// Pure virtual function for operator()
			virtual void operator()() = 0;
	};
	int ThreadTask::numCozinheirosAtivos = 0;
}

