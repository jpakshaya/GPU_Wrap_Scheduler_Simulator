#include "../core/sm.cpp"

class RoundRobinScheduler {
    int current_warp_idx = 0;
public:
    Warp* schedule(SM& sm) {
        if (sm.active_warps.empty()) return nullptr;
        
        for (size_t i = 0; i < sm.active_warps.size(); ++i) {
            int idx = (current_warp_idx + i) % sm.active_warps.size();
            if (!sm.active_warps[idx].is_done()) {
                current_warp_idx = (idx + 1) % sm.active_warps.size();
                return &sm.active_warps[idx];
            }
        }
        return nullptr;
    }
};