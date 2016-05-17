#include "manager.h"

Allocator_manager& Allocator_manager::instance()
{
    static Allocator_manager manager;
    return manager;
}

void Allocator_manager::register_action(action_t action)
{
    sweep_actions_.push_back(action);
}

void Allocator_manager::sweep()
{
    for (auto action : sweep_actions_)
        action();
}
