class GTOScheduler {
public:
    Warp* schedule(SM& sm) {
        for (auto& warp : sm.active_warps) {
            if (!warp.is_done()) return &warp; // Simplistic GTO: oldest first
        }
        return nullptr;
    }
};