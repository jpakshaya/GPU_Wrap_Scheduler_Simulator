#include <vector>
#include <algorithm>
#include "thread.cpp"

struct Warp {
    int id;
    std::vector<Thread> threads;
    int pc = 0; // Program Counter

    bool is_done() {
        return std::all_of(threads.begin(), threads.end(), 
               [](const Thread& t) { return t.remaining_instructions <= 0; });
    }
};