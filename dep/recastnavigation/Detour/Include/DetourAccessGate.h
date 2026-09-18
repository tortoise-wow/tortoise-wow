// ManTech addition: scoped, reentrant readers and exclusive mutation.
// No heap allocation on the read path. Recursion is tracked per thread, not
// per object, so nested Detour queries never recursively lock an SRW lock.
#pragma once
#include <cassert>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>

class dtAccessGate
{
    struct Frame
    {
        dtAccessGate const* gate;
        bool writer;
        Frame* previous;
    };
    inline static thread_local Frame* current = nullptr;
    mutable std::shared_mutex mutex;
public:
    class Read
    {
        Frame frame;
        std::shared_lock<std::shared_mutex> lock;
    public:
        explicit Read(dtAccessGate const* gate) : frame{gate, false, current}
        {
            if (!gate) return;
            bool nested = false;
            for (Frame* p = current; p; p = p->previous)
                if (p->gate == gate) { nested = true; break; }
            if (!nested) lock = std::shared_lock<std::shared_mutex>(gate->mutex);
            current = &frame;
        }
        ~Read() { if (frame.gate) { assert(current == &frame); current = frame.previous; } }
        Read(Read const&) = delete;
        Read& operator=(Read const&) = delete;
    };
    class Write
    {
        Frame frame;
        std::unique_lock<std::shared_mutex> lock;
    public:
        explicit Write(dtAccessGate const* gate) : frame{gate, true, current}
        {
            if (!gate) return;
            bool nested = false;
            for (Frame* p = current; p; p = p->previous)
                if (p->gate == gate && p->writer) { nested = true; break; }
            if (!nested)
            {
                for (Frame* p = current; p; p = p->previous)
                    if (p->gate == gate)
                        throw std::logic_error("navigation mutation attempted inside a read scope");
                lock = std::unique_lock<std::shared_mutex>(gate->mutex);
            }
            current = &frame;
        }
        ~Write() { if (frame.gate) { assert(current == &frame); current = frame.previous; } }
        Write(Write const&) = delete;
        Write& operator=(Write const&) = delete;
    };
};
