; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=fiji -enable-amdgpu-aa=0 -mattr=+flat-for-global -enable-misched=false < %s | FileCheck -enable-var-scope -check-prefixes=GCN,GFX89 %s
; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=hawaii -enable-amdgpu-aa=0 -mattr=+flat-for-global -enable-misched=false < %s | FileCheck -enable-var-scope -check-prefixes=GCN,CI %s

; GCN-LABEL: {{^}}v_insertelement_v2i16_dynamic_vgpr:

; GCN: {{flat|global}}_load_dword [[IDX:v[0-9]+]]
; GCN: {{flat|global}}_load_dword [[VEC:v[0-9]+]]

; GFX89: s_mov_b32 [[MASKK:s[0-9]+]], 0xffff{{$}}

; GFX89: v_lshlrev_b32_e32 [[SCALED_IDX:v[0-9]+]], 4, [[IDX]]
; GFX89: v_lshlrev_b32_e64 [[MASK:v[0-9]+]], [[SCALED_IDX]], [[MASKK]]

; GCN: s_mov_b32 [[K:s[0-9]+]], 0x3e703e7

; CI: v_lshlrev_b32_e32 [[SCALED_IDX:v[0-9]+]], 4, [[IDX]]
; CI: v_lshl_b32_e32 [[MASK:v[0-9]+]], 0xffff, [[SCALED_IDX]]

; GCN: v_bfi_b32 [[RESULT:v[0-9]+]], [[MASK]], [[K]], [[VEC]]
; GCN: {{flat|global}}_store_dword v{{\[[0-9]+:[0-9]+\]}}, [[RESULT]]
define amdgpu_kernel void @v_insertelement_v2i16_dynamic_vgpr(ptr addrspace(1) %out, ptr addrspace(1) %in, ptr addrspace(1) %idx.ptr) #0 {
  %tid = call i32 @llvm.amdgcn.workitem.id.x() #1
  %tid.ext = sext i32 %tid to i64
  %in.gep = getelementptr inbounds <2 x i16>, ptr addrspace(1) %in, i64 %tid.ext
  %idx.gep = getelementptr inbounds i32, ptr addrspace(1) %idx.ptr, i64 %tid.ext
  %out.gep = getelementptr inbounds <2 x i16>, ptr addrspace(1) %out, i64 %tid.ext
  %idx = load i32, ptr addrspace(1) %idx.gep
  %vec = load <2 x i16>, ptr addrspace(1) %in.gep
  %vecins = insertelement <2 x i16> %vec, i16 999, i32 %idx
  store <2 x i16> %vecins, ptr addrspace(1) %out.gep
  ret void
}


declare i32 @llvm.amdgcn.workitem.id.x() #1

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
