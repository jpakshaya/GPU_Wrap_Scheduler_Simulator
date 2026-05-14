class GTOScheduler {
    int current_warp_id = 0; 

public:
    Warp* schedule(SM& sm) {
        for (auto& warp : sm.active_warps) {
            if (warp.id == current_warp_id && !warp.is_done() && !warp.is_stalled())
                return &warp;
        }

        // Current warp stalled or done 
        for (auto& warp : sm.active_warps) {
            if (!warp.is_done() && !warp.is_stalled()) {
                current_warp_id = warp.id; // commit to this one
                return &warp;
            }
        }

        return nullptr; // all warps stalled or done
    }
};