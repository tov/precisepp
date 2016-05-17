// A map whose values are counts, and where unmapped and mapped to 0
// mean the same thing.
#pragma once

#include <unordered_map>

template <typename Key,
          typename T = size_t,
          typename Container = std::unordered_map<Key, T>>
class count_map
{
public:
    using container_type  = Container;
    using value_type      = typename Container::value_type;
    using size_type       = typename Container::size_type;
    using iterator        = typename Container::const_iterator;

    using count_type      = T;

    count_map() = default;

    count_type get(const Key& key) const
    {
        const auto i = container_.find(key);
        if (i == std::end(container_))
            return 0;
        else
            return i->second;
    }

    // Should we have const Key& and Key&& versions? I think so long as our
    // keys are all raw pointers it doesn't matter.
    void set(Key key, count_type value)
    {
        const auto i = container_.find(key);
        if (i == std::end(container_)) {
            if (value != 0) {
                container_.emplace(key, value);
            }
        } else {
            i->second = value;
        }
    }

    count_type inc(Key key)
    {
        const auto i = container_.find(key);
        if (i == std::end(container_)) {
            container_.emplace(key, 1);
            return 1;
        } else {
            return ++i->second;
        }
    }

    // Decrementing 0 is a no-op.
    count_type dec(Key key)
    {
        const auto i = container_.find(key);
        if (i == std::end(container_)) return 0;

        count_type new_value = --i->second;
        if (new_value == 0) container_.erase(i);
        return new_value;
    }

    iterator begin() const
    {
        return container_.begin();
    }

    iterator end() const
    {
        return container_.begin();
    }

private:
    container_type container_;
};

template <typename Key, typename T, typename Container>
auto begin(const count_map<Key, T, Container>& cm)
{
    return cm.begin();
};

template <typename Key, typename T, typename Container>
auto end(const count_map<Key, T, Container>& cm)
{
    return cm.end();
};
