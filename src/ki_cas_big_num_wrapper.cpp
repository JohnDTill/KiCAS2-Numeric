#include "ki_cas_big_num_wrapper.h"

#ifndef NDEBUG
#include <cassert>
#include <memory.h>
#include <shared_mutex>
#include <unordered_set>
#endif

namespace KiCAS2 {

bool isNegative(const BigInteger val) noexcept {
    return val->_mp_size < 0;
}

void flipSign(BigInteger val) noexcept {
    val->_mp_size *= -1;
}

#ifndef NDEBUG
static std::allocator<size_t> allocator;
static std::unordered_set<const void*> allocated_memory;
static std::shared_mutex allocation_mutex;

static void* leakTrackingAlloc(size_t n) {
    allocation_mutex.lock_shared();
    size_t* allocated = allocator.allocate(n);
    const auto result = allocated_memory.insert(allocated);
    allocation_mutex.unlock_shared();
    assert(result.second);

    return allocated;
}

static void leakTrackingFree(void* p, size_t old) noexcept {
    allocation_mutex.lock_shared();
    assert(allocated_memory.find(p) != allocated_memory.end());
    allocated_memory.erase(p);
    allocator.deallocate(reinterpret_cast<size_t*>(p), old);
    allocation_mutex.unlock_shared();
}

static void* leakTrackingRealloc(void* p, size_t old, size_t n) {
    void* reallocated = leakTrackingAlloc(n);
    memcpy(reallocated, p, sizeof(size_t)*std::min(old, n));
    leakTrackingFree(p, old);
    return reallocated;
}

struct Init {
    Init(){ mp_set_memory_functions(leakTrackingAlloc, leakTrackingRealloc, leakTrackingFree); }
};

static Init memoryTrackingInit;

bool isAllGmpMemoryFreed() noexcept {
    return allocated_memory.empty();
}
#endif

}
