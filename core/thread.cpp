#include <cstdlib>

struct Thread {
    int id;
    int remaining_instructions;
    bool is_blocked     = false;
    int  stall_countdown = 0;
    int  branch_path    = 0;   
    bool active_mask    = true; 

    void tick_stall() {
        if (is_blocked && stall_countdown > 0) {
            if (--stall_countdown == 0) is_blocked = false;
        }
    }
};

