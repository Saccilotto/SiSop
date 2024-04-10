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
	class ThreadTaskCanibal {
		public:
			int tribo;  // Number of threads
			int canibal;  // Local counter
			int maxTravessas;
			static int travessasAtual;

			// Destructor, copy constructor, move constructor, copy assignment operator, move assignment operator
			~ThreadTaskCanibal() = default;
			ThreadTaskCanibal(const ThreadTaskCanibal&) = default;
			ThreadTaskCanibal(ThreadTaskCanibal&& other) noexcept = default;
			ThreadTaskCanibal& operator=(const ThreadTaskCanibal& other) = default;
			ThreadTaskCanibal& operator=(ThreadTaskCanibal&& other) noexcept = default;

			// Constructor
			ThreadTaskCanibal(int _threads, int _counter, int _maxTravessas) {
				tribo = _threads;
				canibal = _counter;
				maxTravessas = _maxTravessas;
			}
			// Pure virtual function for operator()
			virtual void operator()() = 0;
	};
	int ThreadTaskCanibal::travessasAtual = 0;


	// Define the base class for thread tasks
	class ThreadTaskProdCons {
		public:
			int total_threads;  // Number of threads
			int current_thread;  // Local counter
			static int count;

			// Destructor, copy constructor, move constructor, copy assignment operator, move assignment operator
			~ThreadTaskProdCons() = default;
			ThreadTaskProdCons(const ThreadTaskProdCons&) = default;
			ThreadTaskProdCons(ThreadTaskProdCons&& other) noexcept = default;
			ThreadTaskProdCons& operator=(const ThreadTaskProdCons& other) = default;
			ThreadTaskProdCons& operator=(ThreadTaskProdCons&& other) noexcept = default;

			// Constructor
			ThreadTaskProdCons(int _total_threads, int _current_thread) {
				total_threads = _total_threads;
				current_thread = _current_thread;
			}
			// Pure virtual function for operator()
			virtual void operator()() = 0;
	};
	int ThreadTaskProdCons::count = 0;
}

