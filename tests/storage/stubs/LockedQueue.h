#pragma once
#include <queue>
#include <mutex>
template<class T, class Mutex> class LockedQueue {
public:
    void add(T value) { values.push(value); }
    bool next(T& value) {
        if (values.empty()) return false;
        value = values.front(); values.pop(); return true;
    }
private:
    std::queue<T> values;
};
