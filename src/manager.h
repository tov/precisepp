#pragma once

#include <vector>

class Allocator_manager
{
public:
    static Allocator_manager& instance();

    typedef void (*action_t)();
    void register_action(action_t);

    void sweep();

private:
    std::vector<action_t> sweep_actions_;

    Allocator_manager() = default;
};