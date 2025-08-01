//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <clc/opencl/atomic/atom_and.h>
#include <clc/opencl/atomic/atomic_and.h>

#ifdef cl_khr_global_int32_extended_atomics
#define __CLC_ATOMIC_OP and
#define __CLC_ATOMIC_ADDRESS_SPACE global
#include "atom_int32_binary.inc"
#endif // cl_khr_global_int32_extended_atomics

#ifdef cl_khr_local_int32_extended_atomics
#define __CLC_ATOMIC_OP and
#define __CLC_ATOMIC_ADDRESS_SPACE local
#include "atom_int32_binary.inc"
#endif // cl_khr_local_int32_extended_atomics

#ifdef cl_khr_int64_extended_atomics

#define IMPL(AS, TYPE)                                                         \
  _CLC_OVERLOAD _CLC_DEF TYPE atom_and(volatile AS TYPE *p, TYPE val) {        \
    return __sync_fetch_and_and_8(p, val);                                     \
  }

IMPL(global, long)
IMPL(global, unsigned long)
IMPL(local, long)
IMPL(local, unsigned long)
#undef IMPL

#endif // cl_khr_int64_extended_atomics
