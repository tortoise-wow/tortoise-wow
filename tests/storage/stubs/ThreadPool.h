#pragma once
#include <future>
class ThreadPool {
public:
    template<class T=void> struct MySQL {};
    struct SingleQueue {};
    ThreadPool(int, const char*) {}
    template<class T> void start() {}
    template<class T> ThreadPool& operator<<(T&&) { return *this; }
    std::future<void> processWorkload() { return {}; }
};
template<class T> std::unique_ptr<ThreadPool>& operator<<(std::unique_ptr<ThreadPool>& pool, T&&) { return pool; }
