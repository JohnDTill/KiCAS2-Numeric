#ifndef KI_CAS_BIG_NUM_WRAPPER_H
#define KI_CAS_BIG_NUM_WRAPPER_H

#ifdef _MSC_VER
#include <malloc.h>  // MSC dependencies for GMP
#endif

#include <gmp.h>
#include <flint/fmpq.h>
#include <flint/fmpz.h>

namespace KiCAS2 {

typedef mpz_t BigInteger;
typedef fmpq_t BigRational;

bool isNegative(const BigInteger val) noexcept;
void flipSign(BigInteger val) noexcept;

#ifndef NDEBUG
bool isAllGmpMemoryFreed() noexcept;
#define DEBUG_REQUIRE(x) REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x) REQUIRE_FALSE(x)
#else
#define DEBUG_REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x)
#endif

}

#endif // KI_CAS_BIG_NUM_WRAPPER_H
