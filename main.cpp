// Inside main.cpp
#include <iostream>
#include <fstream>
#include "scheduler/round_robin.cpp" // This pulls in SM and Warp via includes

int main() {
    std::cout << "Starting GPU Scheduler Simulation..." << std::endl;
    
    SM sm;
    sm.id = 0;
    sm.load_warps(4, 10); 
    RoundRobinScheduler scheduler;

    std::ofstream log("viz_data.csv");
    if (!log.is_open()) {
        std::cerr << "Error: Could not create log file!" << std::endl;
        return 1;
    }

    log << "Cycle,WarpID,RemainingInst\n";

    int total_inst = 40;
    while (total_inst > 0 && sm.clock_cycles < 500) {
        Warp* scheduled = scheduler.schedule(sm);
        if (scheduled) {
            for(auto& t : scheduled->threads) {
                if (t.remaining_instructions > 0) t.remaining_instructions--;
            }
            total_inst--;
            log << sm.clock_cycles << "," << scheduled->id << "," << total_inst << "\n";
        }
        sm.clock_cycles++;
    }

    log.close();
    std::cout << "Simulation finished. Output saved to viz_data.csv" << std::endl;
    return 0;
}