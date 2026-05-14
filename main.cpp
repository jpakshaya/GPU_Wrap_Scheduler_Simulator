#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include "scheduler/round_robin.cpp"
#include "scheduler/gto.cpp"
using namespace std;

const float STALL_PROB     = 0.30f;
const int   STALL_MIN      = 5;
const int   STALL_MAX      = 15;
const float DIVERGE_PROB   = 0.15f; // 15% chance a warp diverges each time it starts

void execute_warp(Warp* warp, int cycle, ofstream& log) {
    //triggers divergence when warp is converged and starts executing
    if (!warp->diverged && (float)rand() / RAND_MAX < DIVERGE_PROB)
        warp->trigger_divergence();

    int executed = 0;
    for (auto& t : warp->threads) {
        if (t.active_mask && t.remaining_instructions > 0 && !t.is_blocked) {
            t.remaining_instructions--;
            executed++;
            if ((float)rand() / RAND_MAX < STALL_PROB) {
                t.is_blocked      = true;
                t.stall_countdown = STALL_MIN + rand() % (STALL_MAX - STALL_MIN + 1);
            }
        }
    }

    // If current divergence pass is complete, advance to next pass or converge
    if (warp->diverged && warp->current_pass_done())
        warp->advance_divergence_pass();


    int total_rem = 0, blocked = 0;
    for (auto& t : warp->threads) {
        total_rem += t.remaining_instructions;
        if (t.is_blocked) blocked++;
    }

    float efficiency = (float)executed / 32.0f;

    log << cycle        << ","
        << warp->id     << ","
        << total_rem    << ","
        << blocked      << ","
        << warp->is_stalled()   << ","
        << warp->diverged       << ","
        << warp->active_thread_count() << ","
        << efficiency   << "\n";
}

void run_simulation(const string& scheduler_name, ofstream& log, int seed) {
    srand(seed);
    SM sm; sm.id = 0;
    sm.load_warps(4, 10);

    log << "Cycle,WarpID,RemainingInst,BlockedThreads,"
           "WarpStalled,Diverged,ActiveThreads,Efficiency\n";

    int total_wasted_cycles    = 0;
    int total_divergence_events = 0;

    auto run_cycle = [&](auto& sched) {
        while (sm.clock_cycles < 2000) {
            for (auto& w : sm.active_warps) w.tick_stalls();

            Warp* w = sched.schedule(sm);
            if (w) {
                if (w->diverged) total_wasted_cycles++;  
                execute_warp(w, sm.clock_cycles, log);
            }

            bool all_done = all_of(sm.active_warps.begin(), sm.active_warps.end(), [](Warp& ww){ return ww.is_done(); });
            if (all_done) break;
            sm.clock_cycles++;
        }
    };

    if (scheduler_name == "RR") {
        RoundRobinScheduler rr;
        run_cycle(rr);
    } else {
        GTOScheduler gto;
        run_cycle(gto);
    }

    cout << scheduler_name << " | Cycles: " << sm.clock_cycles
         << " | Diverged cycles: " << total_wasted_cycles << "\n";
}

int main() {
    { ofstream log("viz_rr.csv");  run_simulation("RR",  log, 42); }
    { ofstream log("viz_gto.csv"); run_simulation("GTO", log, 42); }
    return 0;
}