#pragma once

namespace mutex {   
    // implements Peterson's algorithm for mutual exclusion (between 2 threads only)
    class peterson2 {
    private:
        volatile bool flag[2];
        volatile int turn;

    public:
        peterson2() {
            flag[0] = false;
            flag[1] = false;
            turn = 0;
        }

        void lock(int t) {
            flag[t] = true;
            turn = t;           
            while (flag[1 - t] && turn == t) {} 
            // wait until the other thread passes the turn
        }

        void unlock(int t) {
            flag[t] = false;
        }
    };
}
