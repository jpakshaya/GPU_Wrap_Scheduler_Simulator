struct Thread {
    int id;
    int remaining_instructions;
    bool is_blocked = false; 
};