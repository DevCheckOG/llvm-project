// REQUIRES: shell
// XFAIL: target={{.*}}-zos{{.*}}

// RUN: mkdir -p %t
// RUN: cp %S/Inputs/amdgpu-arch/amdgpu_arch_fail %t/
// RUN: cp %S/Inputs/amdgpu-arch/amdgpu_arch_gfx906 %t/
// RUN: cp %S/Inputs/nvptx-arch/nvptx_arch_fail %t/
// RUN: cp %S/Inputs/nvptx-arch/nvptx_arch_sm_70 %t/
// RUN: cp %S/Inputs/offload-arch/offload_arch_sm_70_gfx906 %t/
// RUN: echo '#!/bin/sh' > %t/amdgpu_arch_empty
// RUN: chmod +x %t/amdgpu_arch_fail
// RUN: chmod +x %t/amdgpu_arch_gfx906
// RUN: chmod +x %t/amdgpu_arch_empty
// RUN: echo '#!/bin/sh' > %t/nvptx_arch_empty
// RUN: chmod +x %t/nvptx_arch_fail
// RUN: chmod +x %t/nvptx_arch_sm_70
// RUN: chmod +x %t/nvptx_arch_empty
// RUN: chmod +x %t/offload_arch_sm_70_gfx906

// case when nvptx-arch and amdgpu-arch return nothing or fails
// RUN:   not %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native \
// RUN:     --nvptx-arch-tool=%t/nvptx_arch_fail --amdgpu-arch-tool=%t/amdgpu_arch_fail %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NO-OUTPUT-ERROR
// RUN:   not %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native \
// RUN:     --nvptx-arch-tool=%t/nvptx_arch_empty --amdgpu-arch-tool=%t/amdgpu_arch_empty %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NO-OUTPUT-ERROR
// NO-OUTPUT-ERROR: error: cannot determine openmp architecture

// case when amdgpu-arch succeeds.
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native \
// RUN:     --nvptx-arch-tool=%t/nvptx_arch_fail --amdgpu-arch-tool=%t/amdgpu_arch_gfx906 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-GFX906
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp -fopenmp-targets=amdgcn-amd-amdhsa \
// RUN:     --nvptx-arch-tool=%t/nvptx_arch_fail --amdgpu-arch-tool=%t/amdgpu_arch_gfx906 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-GFX906
// ARCH-GFX906: "-cc1" "-triple" "amdgcn-amd-amdhsa"{{.*}}"-target-cpu" "gfx906"

// case when nvptx-arch succeeds.
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native \
// RUN:     --amdgpu-arch-tool=%t/amdgpu_arch_fail --nvptx-arch-tool=%t/nvptx_arch_sm_70 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-SM_70
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp -fopenmp-targets=nvptx64-nvidia-cuda \
// RUN:     --amdgpu-arch-tool=%t/amdgpu_arch_fail --nvptx-arch-tool=%t/nvptx_arch_sm_70 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-SM_70
// ARCH-SM_70: "-cc1" "-triple" "nvptx64-nvidia-cuda"{{.*}}"-target-cpu" "sm_70"

// case when both nvptx-arch and amdgpu-arch succeed.
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native \
// RUN:     --offload-arch-tool=%t/offload_arch_sm_70_gfx906 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-SM_70-GFX906
// ARCH-SM_70-GFX906: "-cc1" "-triple" "amdgcn-amd-amdhsa"{{.*}}"-target-cpu" "gfx906"
// ARCH-SM_70-GFX906: "-cc1" "-triple" "nvptx64-nvidia-cuda"{{.*}}"-target-cpu" "sm_70"

// case when both nvptx-arch and amdgpu-arch succeed with other archs.
// RUN:   %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp --offload-arch=native,sm_75,gfx1030 \
// RUN:     --offload-arch-tool=%t/offload_arch_sm_70_gfx906 %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ARCH-MULTIPLE
// ARCH-MULTIPLE: "-cc1" "-triple" "amdgcn-amd-amdhsa"{{.*}}"-target-cpu" "gfx1030"
// ARCH-MULTIPLE: "-cc1" "-triple" "amdgcn-amd-amdhsa"{{.*}}"-target-cpu" "gfx906"
// ARCH-MULTIPLE: "-cc1" "-triple" "nvptx64-nvidia-cuda"{{.*}}"-target-cpu" "sm_70"
// ARCH-MULTIPLE: "-cc1" "-triple" "nvptx64-nvidia-cuda"{{.*}}"-target-cpu" "sm_75"

// case when 'nvptx-arch' returns nothing using `-fopenmp-targets=`.
// RUN:   not %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp \
// RUN:     -fopenmp-targets=nvptx64-nvidia-cuda --nvptx-arch-tool=%t/nvptx_arch_empty %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NVPTX
// NVPTX: error: cannot determine nvptx64 architecture: No NVIDIA GPU detected in the system; consider passing it via '--offload-arch'

// case when 'amdgpu-arch' returns nothing using `-fopenmp-targets=`.
// RUN:   not %clang -### --target=x86_64-unknown-linux-gnu -nogpulib -fopenmp=libomp \
// RUN:     -fopenmp-targets=amdgcn-amd-amdhsa --amdgpu-arch-tool=%t/amdgpu_arch_empty %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=AMDGPU
// AMDGPU: error: cannot determine amdgcn architecture: No AMD GPU detected in the system; consider passing it via '--offload-arch'

// case when CLANG_TOOLCHAIN_PROGRAM_TIMEOUT is malformed for nvptx-arch.
// RUN: env CLANG_TOOLCHAIN_PROGRAM_TIMEOUT=foo \
// RUN: not %clang -### --target=x86_64-unknown-linux-gnu -fopenmp=libomp \
// RUN:     -fopenmp-targets=nvptx64-nvidia-cuda -nogpulib \
// RUN:     --nvptx-arch-tool=%t/nvptx_arch_sm_70 %s 2>&1 | \
// RUN:   FileCheck %s --check-prefix=BAD-TIMEOUT-NVPTX
// BAD-TIMEOUT-NVPTX: clang: error: cannot determine nvptx64 architecture: CLANG_TOOLCHAIN_PROGRAM_TIMEOUT expected an integer, got 'foo'; consider passing it via '--offload-arch'; environment variable CLANG_TOOLCHAIN_PROGRAM_TIMEOUT specifies the tool timeout (integer secs, <=0 is infinite)

// case when CLANG_TOOLCHAIN_PROGRAM_TIMEOUT is malformed for amdgpu-arch.
// RUN: env CLANG_TOOLCHAIN_PROGRAM_TIMEOUT= \
// RUN: not %clang -### --target=x86_64-unknown-linux-gnu -fopenmp=libomp \
// RUN:     -fopenmp-targets=amdgcn-amd-amdhsa -nogpulib \
// RUN:     --amdgpu-arch-tool=%t/amdgpu_arch_gfx906 %s 2>&1 | \
// RUN:   FileCheck %s --check-prefix=BAD-TIMEOUT-AMDGPU
// BAD-TIMEOUT-AMDGPU: clang: error: cannot determine amdgcn architecture: CLANG_TOOLCHAIN_PROGRAM_TIMEOUT expected an integer, got ''; consider passing it via '--offload-arch'; environment variable CLANG_TOOLCHAIN_PROGRAM_TIMEOUT specifies the tool timeout (integer secs, <=0 is infinite)
