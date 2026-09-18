#include "MovementViewerSet.h"
#include <atomic>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <cstdint>

static void Check(bool value) { if (!value) throw std::runtime_error("viewer index mismatch"); }
int main()
{
    MovementViewerSet<uint64_t> viewers;
    Check(viewers.Snapshot().empty());
    viewers.Add(34); viewers.Add(12); viewers.Add(34);
    Check(viewers.Snapshot() == std::vector<uint64_t>({12,34}));
    auto oldSnapshot = viewers.Snapshot();
    viewers.Remove(12); viewers.Remove(99);
    Check(viewers.Snapshot() == std::vector<uint64_t>({34}));
    Check(oldSnapshot == std::vector<uint64_t>({12,34}));
    viewers.Clear(); // map removal/logout visibility reset
    Check(viewers.Snapshot().empty());
    std::vector<std::thread> writers;
    std::atomic<bool> done{false};
    std::thread reader([&] {
        while (!done.load())
        {
            auto snapshot = viewers.Snapshot();
            Check(std::is_sorted(snapshot.begin(), snapshot.end()));
            Check(std::adjacent_find(snapshot.begin(), snapshot.end()) == snapshot.end());
        }
    });
    for (uint64_t thread = 0; thread < 4; ++thread)
        writers.emplace_back([&, thread] {
            for (uint64_t n = 0; n < 2000; ++n)
            {
                auto guid = n * 4 + thread;
                viewers.Add(guid); viewers.Add(guid);
                if (n % 2 == 0) viewers.Remove(guid);
            }
        });
    for (auto& writer : writers) writer.join();
    done.store(true); reader.join();
    Check(viewers.Snapshot().size() == 4000);
    viewers.Clear();
    viewers.Add(34); // GUID reuse starts with a fresh visibility registration
    Check(viewers.Snapshot() == std::vector<uint64_t>({34}));
    std::cout << "PASS: duplicate/removal/reset, immutable snapshots, concurrent readers/writers\n";
}
