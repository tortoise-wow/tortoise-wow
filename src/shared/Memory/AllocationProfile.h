#ifndef MANTECH_ALLOCATION_PROFILE_H
#define MANTECH_ALLOCATION_PROFILE_H
#include <cstddef>
#include <cstdint>
namespace ManTech {
struct AllocationTotals { std::uint64_t allocated=0,freed=0,live=0,count=0,samples=0,drops=0,siteDrops=0;unsigned probability=0;bool recording=false; };
inline AllocationTotals (*allocationProfileTotals)()=nullptr;
inline AllocationTotals ReadAllocationTotals(){return allocationProfileTotals?allocationProfileTotals():AllocationTotals{};}
// Diagnostic hooks are registered before main only in an opt-in profile build.
inline void (*allocationProfileStart)(unsigned) = nullptr;
inline void (*allocationProfileWrite)() = nullptr;
inline void (*allocationProfileStop)() = nullptr;
inline void (*allocationProfileIgnoreThread)() = nullptr;
inline void IgnoreAllocationProfileThread(){if(allocationProfileIgnoreThread)allocationProfileIgnoreThread();}
inline std::size_t (*allocationProfileCapacity)() = nullptr;
inline std::size_t AllocationProfileCapacity(){return allocationProfileCapacity?allocationProfileCapacity():0;}
inline void StopAllocationProfile(){if(allocationProfileStop)allocationProfileStop();}
inline void StartAllocationProfile(unsigned mask=1023) { if (allocationProfileStart) allocationProfileStart(mask); }
inline void WriteAllocationProfile() { if (allocationProfileWrite) allocationProfileWrite(); }
}
#endif
