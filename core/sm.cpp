#include <vector>
#include "warp.cpp"

class SM {
public:
    int id;
    std::vector<Warp> active_warps;
    int clock_cycles = 0;

    void load_warps(int count, int inst_per_thread) {
        for (int i = 0; i < count; ++i) {
            Warp w; w.id = i;
            for (int j = 0; j < 32; ++j) // Standard SIMT width
                w.threads.push_back({j, inst_per_thread});
            active_warps.push_back(w);
        }
    }
};