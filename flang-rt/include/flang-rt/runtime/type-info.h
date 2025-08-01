//===-- include/flang-rt/runtime/type-info.h --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef FLANG_RT_RUNTIME_TYPE_INFO_H_
#define FLANG_RT_RUNTIME_TYPE_INFO_H_

// A C++ perspective of the derived type description schemata in
// flang/module/__fortran_type_info.f90.

#include "descriptor.h"
#include "terminator.h"
#include "flang/Common/Fortran-consts.h"
#include "flang/Common/bit-population-count.h"
#include "flang/Common/optional.h"
#include <cinttypes>
#include <memory>

namespace Fortran::runtime::typeInfo {

class DerivedType;

using ProcedurePointer = void (*)(); // TYPE(C_FUNPTR)

struct Binding {
  ProcedurePointer proc;
  StaticDescriptor<0> name; // CHARACTER(:), POINTER
};

class Value {
public:
  enum class Genre : std::uint8_t {
    Deferred = 1,
    Explicit = 2,
    LenParameter = 3
  };
  RT_API_ATTRS Genre genre() const { return genre_; }
  RT_API_ATTRS Fortran::common::optional<TypeParameterValue> GetValue(
      const Descriptor *) const;

private:
  Genre genre_{Genre::Explicit};
  // The value encodes an index into the table of LEN type parameters in
  // a descriptor's addendum for genre == Genre::LenParameter.
  TypeParameterValue value_{0};
};

class Component {
public:
  enum class Genre : std::uint8_t {
    Data = 1,
    Pointer = 2,
    Allocatable = 3,
    Automatic = 4
  };

  RT_API_ATTRS const Descriptor &name() const { return name_.descriptor(); }
  RT_API_ATTRS Genre genre() const { return genre_; }
  RT_API_ATTRS TypeCategory category() const {
    return static_cast<TypeCategory>(category_);
  }
  RT_API_ATTRS int kind() const { return kind_; }
  RT_API_ATTRS int rank() const { return rank_; }
  RT_API_ATTRS std::uint64_t offset() const { return offset_; }
  RT_API_ATTRS const Value &characterLen() const { return characterLen_; }
  RT_API_ATTRS const DerivedType *derivedType() const {
    return category() == TypeCategory::Derived
        ? derivedType_.descriptor().OffsetElement<const DerivedType>()
        : nullptr;
  }
  RT_API_ATTRS const Value *lenValue() const {
    return lenValue_.descriptor().OffsetElement<const Value>();
  }
  RT_API_ATTRS const Value *bounds() const {
    return bounds_.descriptor().OffsetElement<const Value>();
  }
  RT_API_ATTRS const char *initialization() const { return initialization_; }

  RT_API_ATTRS std::size_t GetElementByteSize(const Descriptor &) const;
  RT_API_ATTRS std::size_t GetElements(const Descriptor &) const;

  // For components that are descriptors, returns size of descriptor;
  // for Genre::Data, returns elemental byte size times element count.
  RT_API_ATTRS std::size_t SizeInBytes(const Descriptor &) const;

  // Establishes a descriptor from this component description.
  RT_API_ATTRS void EstablishDescriptor(
      Descriptor &, const Descriptor &container, Terminator &) const;

  // Creates a pointer descriptor from this component description, possibly
  // with subscripts
  RT_API_ATTRS void CreatePointerDescriptor(Descriptor &,
      const Descriptor &container, Terminator &,
      const SubscriptValue * = nullptr) const;

  FILE *Dump(FILE * = stdout) const;

private:
  StaticDescriptor<0> name_; // CHARACTER(:), POINTER
  Genre genre_{Genre::Data};
  std::uint8_t category_; // common::TypeCategory
  std::uint8_t kind_{0};
  std::uint8_t rank_{0};
  std::uint64_t offset_{0};
  Value characterLen_; // for TypeCategory::Character
  StaticDescriptor<0, true> derivedType_; // TYPE(DERIVEDTYPE), POINTER
  StaticDescriptor<1, true>
      lenValue_; // TYPE(VALUE), POINTER, DIMENSION(:), CONTIGUOUS
  StaticDescriptor<2, true>
      bounds_; // TYPE(VALUE), POINTER, DIMENSION(2,:), CONTIGUOUS
  const char *initialization_{nullptr}; // for Genre::Data and Pointer
  // TODO: cobounds
  // TODO: `PRIVATE` attribute
};

struct ProcPtrComponent {
  StaticDescriptor<0> name; // CHARACTER(:), POINTER
  std::uint64_t offset{0};
  ProcedurePointer procInitialization;
};

class SpecialBinding {
public:
  enum class Which : std::uint8_t {
    None = 0,
    ScalarAssignment = 1,
    ElementalAssignment = 2,
    ReadFormatted = 3,
    ReadUnformatted = 4,
    WriteFormatted = 5,
    WriteUnformatted = 6,
    ElementalFinal = 7,
    AssumedRankFinal = 8,
    ScalarFinal = 9,
    // higher-ranked final procedures follow
  };

  // Special bindings can be created during execution to handle defined
  // I/O procedures that are not type-bound.
  RT_API_ATTRS SpecialBinding(Which which, ProcedurePointer proc,
      std::uint8_t isArgDescSet, std::uint8_t isTypeBound,
      std::uint8_t specialCaseFlag)
      : which_{which}, isArgDescriptorSet_{isArgDescSet},
        isTypeBound_{isTypeBound}, specialCaseFlag_{specialCaseFlag},
        proc_{proc} {}

  static constexpr RT_API_ATTRS Which RankFinal(int rank) {
    return static_cast<Which>(static_cast<int>(Which::ScalarFinal) + rank);
  }

  RT_API_ATTRS Which which() const { return which_; }
  RT_API_ATTRS bool specialCaseFlag() const { return specialCaseFlag_; }
  RT_API_ATTRS bool IsArgDescriptor(int zeroBasedArg) const {
    return (isArgDescriptorSet_ >> zeroBasedArg) & 1;
  }
  RT_API_ATTRS bool IsTypeBound() const { return isTypeBound_ != 0; }
  template <typename PROC>
  RT_API_ATTRS PROC GetProc(const Binding *bindings = nullptr) const {
    if (bindings && isTypeBound_ > 0) {
      return reinterpret_cast<PROC>(bindings[isTypeBound_ - 1].proc);
    } else {
      return reinterpret_cast<PROC>(proc_);
    }
  }

  FILE *Dump(FILE *) const;

private:
  Which which_{Which::None};

  // The following little bit-set identifies which dummy arguments are
  // passed via descriptors for their derived type arguments.
  //   Which::Assignment and Which::ElementalAssignment:
  //     Set to 1, 2, or (usually 3).
  //     The passed-object argument (usually the "to") is always passed via a
  //     a descriptor in the cases where the runtime will call a defined
  //     assignment because these calls are to type-bound generics,
  //     not generic interfaces, and type-bound generic defined assigment
  //     may appear only in an extensible type and requires a passed-object
  //     argument (see C774), and passed-object arguments to TBPs must be
  //     both polymorphic and scalar (C760).  The non-passed-object argument
  //     (usually the "from") is usually, but not always, also a descriptor.
  //   Which::Final and Which::ElementalFinal:
  //     Set to 1 when dummy argument is assumed-shape; otherwise, the
  //     argument can be passed by address.  (Fortran guarantees that
  //     any finalized object must be whole and contiguous by restricting
  //     the use of DEALLOCATE on pointers.  The dummy argument of an
  //     elemental final subroutine must be scalar and monomorphic, but
  //     use a descriptors when the type has LEN parameters.)
  //   Which::AssumedRankFinal: flag must necessarily be set
  //   Defined I/O:
  //     Set to 1 when "dtv" initial dummy argument is polymorphic, which is
  //     the case when and only when the derived type is extensible.
  //     When false, the defined I/O subroutine must have been
  //     called via a generic interface, not a generic TBP.
  std::uint8_t isArgDescriptorSet_{0};
  // When a special binding is type-bound, this is its binding's index (plus 1,
  // so that 0 signifies that it's not type-bound).
  std::uint8_t isTypeBound_{0};
  // For a FINAL subroutine, set when it has a dummy argument that is an array
  // that is CONTIGUOUS or neither assumed-rank nor assumed-shape.
  // For a defined I/O subroutine, set when UNIT= and IOSTAT= are INTEGER(8).
  std::uint8_t specialCaseFlag_{0};
  ProcedurePointer proc_{nullptr};
};

class DerivedType {
public:
  ~DerivedType(); // never defined

  RT_API_ATTRS const Descriptor &binding() const {
    return binding_.descriptor();
  }
  RT_API_ATTRS const Descriptor &name() const { return name_.descriptor(); }
  RT_API_ATTRS std::uint64_t sizeInBytes() const { return sizeInBytes_; }
  RT_API_ATTRS const Descriptor &uninstantiated() const {
    return uninstantiated_.descriptor();
  }
  RT_API_ATTRS const DerivedType *uninstantiatedType() const {
    return reinterpret_cast<const DerivedType *>(
        uninstantiated().raw().base_addr);
  }
  RT_API_ATTRS const Descriptor &kindParameter() const {
    return kindParameter_.descriptor();
  }
  RT_API_ATTRS const Descriptor &lenParameterKind() const {
    return lenParameterKind_.descriptor();
  }
  RT_API_ATTRS const Descriptor &component() const {
    return component_.descriptor();
  }
  RT_API_ATTRS const Descriptor &procPtr() const {
    return procPtr_.descriptor();
  }
  RT_API_ATTRS const Descriptor &special() const {
    return special_.descriptor();
  }
  RT_API_ATTRS bool hasParent() const { return hasParent_; }
  RT_API_ATTRS bool noInitializationNeeded() const {
    return noInitializationNeeded_;
  }
  RT_API_ATTRS bool noDestructionNeeded() const { return noDestructionNeeded_; }
  RT_API_ATTRS bool noFinalizationNeeded() const {
    return noFinalizationNeeded_;
  }
  RT_API_ATTRS bool noDefinedAssignment() const { return noDefinedAssignment_; }

  RT_API_ATTRS std::size_t LenParameters() const {
    return lenParameterKind().Elements();
  }

  RT_API_ATTRS const DerivedType *GetParentType() const;

  // Finds a data component by name in this derived type or its ancestors.
  RT_API_ATTRS const Component *FindDataComponent(
      const char *name, std::size_t nameLen) const;

  // O(1) look-up of special procedure bindings
  RT_API_ATTRS const SpecialBinding *FindSpecialBinding(
      SpecialBinding::Which which) const {
    auto bitIndex{static_cast<std::uint32_t>(which)};
    auto bit{std::uint32_t{1} << bitIndex};
    if (specialBitSet_ & bit) {
      // The index of this special procedure in the sorted array is the
      // number of special bindings that are present with smaller "which"
      // code values.
      int offset{common::BitPopulationCount(specialBitSet_ & (bit - 1))};
      const auto *binding{
          special_.descriptor().ZeroBasedIndexedElement<SpecialBinding>(
              offset)};
      INTERNAL_CHECK(binding && binding->which() == which);
      return binding;
    } else {
      return nullptr;
    }
  }

  FILE *Dump(FILE * = stdout) const;

private:
  // This member comes first because it's used like a vtable by generated code.
  // It includes all of the ancestor types' bindings, if any, first,
  // with any overrides from descendants already applied to them.  Local
  // bindings then follow in alphabetic order of binding name.
  StaticDescriptor<1, true>
      binding_; // TYPE(BINDING), DIMENSION(:), POINTER, CONTIGUOUS

  StaticDescriptor<0> name_; // CHARACTER(:), POINTER

  std::uint64_t sizeInBytes_{0};

  // Instantiations of a parameterized derived type with KIND type
  // parameters will point this data member to the description of
  // the original uninstantiated type, which may be shared from a
  // module via use association.  The original uninstantiated derived
  // type description will point to itself.  Derived types that have
  // no KIND type parameters will have a null pointer here.
  StaticDescriptor<0, true> uninstantiated_; // TYPE(DERIVEDTYPE), POINTER

  // These pointer targets include all of the items from the parent, if any.
  StaticDescriptor<1> kindParameter_; // pointer to rank-1 array of INTEGER(8)
  StaticDescriptor<1>
      lenParameterKind_; // pointer to rank-1 array of INTEGER(1)

  // This array of local data components includes the parent component.
  // Components are in component order, not collation order of their names.
  // It does not include procedure pointer components.
  StaticDescriptor<1, true>
      component_; // TYPE(COMPONENT), POINTER, DIMENSION(:), CONTIGUOUS

  // Procedure pointer components
  StaticDescriptor<1, true>
      procPtr_; // TYPE(PROCPTR), POINTER, DIMENSION(:), CONTIGUOUS

  // Packed in ascending order of "which" code values.
  // Does not include special bindings from ancestral types.
  StaticDescriptor<1, true>
      special_; // TYPE(SPECIALBINDING), POINTER, DIMENSION(:), CONTIGUOUS

  // Little-endian bit-set of special procedure binding "which" code values
  // for O(1) look-up in FindSpecialBinding() above.
  std::uint32_t specialBitSet_{0};

  // Flags
  bool hasParent_{false};
  bool noInitializationNeeded_{false};
  bool noDestructionNeeded_{false};
  bool noFinalizationNeeded_{false};
  bool noDefinedAssignment_{false};
};

} // namespace Fortran::runtime::typeInfo
#endif // FLANG_RT_RUNTIME_TYPE_INFO_H_
