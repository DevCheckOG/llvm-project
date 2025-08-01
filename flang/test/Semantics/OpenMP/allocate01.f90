! REQUIRES: openmp_runtime

! RUN: %python %S/../test_errors.py %s %flang_fc1 %openmp_flags -fopenmp-version=52
! OpenMP Version 5.0
! 2.11.3 allocate Directive
! The allocate directive must appear in the same scope as the declarations of
! each of its list items and must follow all such declarations.

subroutine allocate()
use omp_lib
  integer, allocatable :: x(:)
  integer :: y
  contains
    subroutine sema()
    integer :: a, b
    real, dimension (:,:), allocatable :: darray

    !ERROR: List items must be declared in the same scoping unit in which the ALLOCATE directive appears
    !$omp allocate(y)
        print *, a

    !WARNING: OpenMP directive ALLOCATE has been deprecated, please use ALLOCATORS instead. [-Wopen-mp-usage]
    !ERROR: List items must be declared in the same scoping unit in which the ALLOCATE directive appears
    !$omp allocate(x) allocator(omp_default_mem_alloc)
      allocate ( x(a), darray(a, b) )
    end subroutine sema

end subroutine allocate
