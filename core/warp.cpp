#include <vector>
#include <algorithm>
#include "thread.cpp"
using namespace std;

struct Warp {
    int  id;
    vector<Thread> threads;
    int  pc            = 0;
    bool diverged      = false;
    int  diverge_pass  = 0;  

    bool is_done() {
        return all_of(threads.begin(), threads.end(),
            [](const Thread& t){ return t.remaining_instructions <= 0; });
    }

    bool is_stalled() {
        for (auto& t : threads)
            if (!t.is_blocked && t.remaining_instructions > 0 && t.active_mask)
                return false;
        return !is_done();
    }

    void tick_stalls() {
        for (auto& t : threads) t.tick_stall();
    }

    // Count how many threads are actually doing work this cycle
    int active_thread_count() {
        int n = 0;
        for (auto& t : threads)
            if (t.active_mask && !t.is_blocked && t.remaining_instructions > 0)
                n++;
        return n;
    }

    // Returns true if divergence actually happened
    bool trigger_divergence() {
        int path0 = 0, path1 = 0;
        for (auto& t : threads) {
            if (t.remaining_instructions > 0) {
                t.branch_path = rand() % 2;
                t.branch_path == 0 ? path0++ : path1++;
            }
        }
        if (path0 > 0 && path1 > 0) {
            diverged     = true;
            diverge_pass = 0;
            // Only path-0 threads are active in first pass
            for (auto& t : threads)
                t.active_mask = (t.branch_path == 0);
            return true;
        }
        // All threads took same path: no divergence
        for (auto& t : threads) t.active_mask = true;
        return false;
    }

    // Called when all active pass threads finish their branch
    // Switches to the other path, or resolves divergence
    void advance_divergence_pass() {
        if (!diverged) return;
        if (diverge_pass == 0) {
            // Switch to path-B threads
            diverge_pass = 1;
            for (auto& t : threads)
                t.active_mask = (t.branch_path == 1);
        } else {
            // Both passes done: converge
            diverged = false;
            for (auto& t : threads) {
                t.active_mask  = true;
                t.branch_path  = 0;
            }
        }
    }

    bool current_pass_done() {
        for (auto& t : threads)
            if (t.active_mask && t.remaining_instructions > 0 && !t.is_blocked)
                return false;
        return true;
    }
};