// RUN: llvm-tblgen -gen-sd-node-info -I %p/../../../include %s | FileCheck %s

include "llvm/Target/Target.td"

def MyTarget : Target;

def my_node_1a : SDNode<"MyTargetISD::NODE_1", SDTypeProfile<1, 0, [SDTCisVT<0, i32>]>>;
def my_node_1b : SDNode<"MyTargetISD::NODE_1", SDTypeProfile<1, 0, [SDTCisVT<0, i32>]>>;
def my_node_2a : SDNode<"MyTargetISD::NODE_2", SDTypeProfile<1, 0, [SDTCisVT<0, i32>]>>;
def my_node_2b : SDNode<"MyTargetISD::NODE_2", SDTypeProfile<1, 0, [SDTCisVT<0, untyped>]>>;

// CHECK:       namespace llvm::MyTargetISD {
// CHECK-EMPTY:
// CHECK-NEXT:  enum GenNodeType : unsigned {
// CHECK-NEXT:    NODE_1 = ISD::BUILTIN_OP_END,
// CHECK-NEXT:    NODE_2,
// CHECK-NEXT:  };
// CHECK-EMPTY:
// CHECK-NEXT:  static constexpr unsigned GENERATED_OPCODE_END = NODE_2 + 1;
// CHECK-EMPTY:
// CHECK-NEXT:  } // namespace llvm::MyTargetISD

// CHECK:       static constexpr char MyTargetSDNodeNamesStorage[] =
// CHECK-NEXT:    "\0"
// CHECK-NEXT:    "MyTargetISD::NODE_1\0"
// CHECK-NEXT:    "MyTargetISD::NODE_2\0"
// CHECK-NEXT:    ;

// CHECK:       static const SDTypeConstraint MyTargetSDTypeConstraints[] = {
// CHECK-NEXT:    /* 0 */ {SDTCisVT, 0, 0, MVT::i32},
// CHECK-NEXT:  };
// CHECK-EMPTY:
// CHECK-NEXT:  static const SDNodeDesc MyTargetSDNodeDescs[] = {
// CHECK-NEXT:      {1, 0, 0, 0, 0, 1, 0, 1}, // NODE_1
// CHECK-NEXT:      {1, 0, 0, 0, 0, 21, 0, 0}, // NODE_2
// CHECK-NEXT:  };
// CHECK-EMPTY:
// CHECK-NEXT:  static const SDNodeInfo MyTargetGenSDNodeInfo(
// CHECK-NEXT:      /*NumOpcodes=*/2, MyTargetSDNodeDescs,
// CHECK-NEXT:      MyTargetSDNodeNames, MyTargetSDTypeConstraints);
