#include <boost/test/unit_test.hpp>

#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(DecisionTest)

static std::string EXPECTED = R"(target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }

declare %block* @parseConfiguration(i8*)

define %mpz* @eval_func(%block* %subject0) {
entry:
  %0 = ptrtoint %block* %subject0 to i64
  %1 = trunc i64 %0 to i1
  br i1 %1, label %constant, label %block

stuck:                                            ; preds = %_0_case_0, %getTag3, %getTag
  call void @abort()
  unreachable

subject0_case_0:                                  ; preds = %getTag
  %2 = bitcast %block* %subject0 to { %blockheader, [0 x i64], %block*, %mpz* }*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %2, i64 0, i32 2
  %_0 = load %block*, %block** %3
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %2, i64 0, i32 3
  %_1 = load %mpz*, %mpz** %4
  %5 = ptrtoint %block* %_0 to i64
  %6 = trunc i64 %5 to i1
  br i1 %6, label %constant1, label %block2

constant:                                         ; preds = %entry
  %7 = lshr i64 %0, 32
  br label %getTag

block:                                            ; preds = %entry
  %8 = getelementptr inbounds %block, %block* %subject0, i64 0, i32 0, i32 0
  %9 = load i64, i64* %8
  br label %getTag

getTag:                                           ; preds = %block, %constant
  %phi = phi i64 [ %9, %block ], [ %7, %constant ]
  %10 = trunc i64 %phi to i32
  switch i32 %10, label %stuck [
    i32 2, label %subject0_case_0
  ]

_0_case_0:                                        ; preds = %getTag3
  %11 = bitcast %block* %_0 to { %blockheader, [0 x i64], i1 }*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %11, i64 0, i32 2
  %_2 = load i1, i1* %12
  switch i1 %_2, label %stuck [
    i1 true, label %_2_case_0
    i1 false, label %_2_case_1
  ]

_0_case_1:                                        ; preds = %getTag3
  %13 = bitcast %block* %_0 to { %blockheader, [0 x i64] }*
  %_3 = call i1 @eval_foo(%mpz* %_1)
  %14 = call %mpz* @apply_rule_2(%mpz* %_1, i1 %_3)
  ret %mpz* %14

constant1:                                        ; preds = %subject0_case_0
  %15 = lshr i64 %5, 32
  br label %getTag3

block2:                                           ; preds = %subject0_case_0
  %16 = getelementptr inbounds %block, %block* %_0, i64 0, i32 0, i32 0
  %17 = load i64, i64* %16
  br label %getTag3

getTag3:                                          ; preds = %block2, %constant1
  %phi4 = phi i64 [ %17, %block2 ], [ %15, %constant1 ]
  %18 = trunc i64 %phi4 to i32
  switch i32 %18, label %stuck [
    i32 0, label %_0_case_0
    i32 1, label %_0_case_1
  ]

_2_case_0:                                        ; preds = %_0_case_0
  %19 = call %mpz* @apply_rule_0()
  ret %mpz* %19

_2_case_1:                                        ; preds = %_0_case_0
  %20 = call %mpz* @apply_rule_1()
  ret %mpz* %20
}

; Function Attrs: noreturn
declare void @abort() #0

declare %mpz* @apply_rule_0()

declare %mpz* @apply_rule_1()

declare i1 @eval_foo(%mpz*)

declare %mpz* @apply_rule_2(%mpz*, i1)

attributes #0 = { noreturn }
)";

BOOST_AUTO_TEST_CASE(simple) {
/* match subject with 
   | Foo(l,r) -> match l with
                 | Bar(b) -> match b with
                             | true -> apply_rule_0()
                             | false -> apply_rule_1()
                 | Baz -> let b = eval_foo(r) in apply_rule_2(r, b)
   | _ -> fail()
*/
  auto Literal = SwitchNode::Create("_2");
  auto dv = KOREObjectSymbol::Create("\\dv");

  Literal->addCase({dv, llvm::APInt(1, 1), LeafNode::Create("apply_rule_0")});
  Literal->addCase({dv, llvm::APInt(1, 0), LeafNode::Create("apply_rule_1")});

  auto Inner = SwitchNode::Create("_0");
  auto Bar = KOREObjectSymbol::Create("Bar");
  auto Baz = KOREObjectSymbol::Create("Baz");
  auto InnerSort = KOREObjectCompositeSort::Create("Inner", SortCategory::Symbol);
  auto Bool = KOREObjectCompositeSort::Create("Bool", SortCategory::Bool);

  Bar->addSort(InnerSort);
  Bar->addArgument(Bool);
  Baz->addSort(InnerSort);
  Bar->setTag(0);
  Baz->setTag(1);

  DecisionCase BarCase{Bar, {"_2"}, Literal};
  Inner->addCase(BarCase);
  auto BazLeaf = LeafNode::Create("apply_rule_2");
  BazLeaf->addBinding("_1");
  BazLeaf->addBinding("_3");
  auto Func = FunctionNode::Create("_3", "eval_foo", BazLeaf, SortCategory::Bool);
  Func->addBinding("_1");
  Inner->addCase({Baz, std::vector<std::string>{}, Func});

  auto Outer = SwitchNode::Create("subject0");
  auto Foo = KOREObjectSymbol::Create("Foo");
  auto OuterSort = KOREObjectCompositeSort::Create("Outer", SortCategory::Symbol);
  auto Int = KOREObjectCompositeSort::Create("Int", SortCategory::Int);

  Foo->addSort(OuterSort);
  Foo->addArgument(InnerSort);
  Foo->addArgument(Int);
  Foo->setTag(2);

  DecisionCase InnerCase{Foo, {"_0", "_1"}, Inner};
  Outer->addCase(InnerCase);
  Outer->addCase(DecisionCase{nullptr, std::vector<std::string>{}, FailNode::get()});

  llvm::LLVMContext Ctx;
  auto mod = newModule("test_decision", Ctx);

  auto F = KOREObjectSymbol::Create("func");
  F->addSort(Int);
  F->addArgument(OuterSort);

  makeEvalFunction(F, nullptr, mod.get(), Outer);

  std::string actual;
  llvm::raw_string_ostream out(actual);
  mod->print(out, nullptr);
  BOOST_CHECK_EQUAL(actual, EXPECTED);

}

BOOST_AUTO_TEST_CASE(deserialization_complex) {
  std::string dt = R"YAML(
specializations:
- - Foo
  - specializations:
    - - Bar
      - specializations:
        - - 1
          - action:
            - 0
            - []
        - - 0
          - action:
            - 1
            - []
        default: null
        occurrence:
        - 0
        - 0
        - 1
        bitwidth: 1
    - - Baz
      - function: eval_foo
        args:
        - - 1
          - 1
        sort: BOOL.Bool
        next:
          action:
          - 2
          - - - 1
              - 1
            - - 0
              - 0
    default: null
    occurrence:
    - 0
    - 1
default: fail
occurrence:
- 1
)YAML";

  auto Bar = KOREObjectSymbol::Create("Bar");
  auto Baz = KOREObjectSymbol::Create("Baz");
  auto InnerSort = KOREObjectCompositeSort::Create("Inner", SortCategory::Symbol);
  auto Bool = KOREObjectCompositeSort::Create("Bool", SortCategory::Bool);

  Bar->addSort(InnerSort);
  Bar->addArgument(Bool);
  Baz->addSort(InnerSort);
  Bar->setTag(0);
  Baz->setTag(1);
  
  auto Foo = KOREObjectSymbol::Create("Foo");
  auto OuterSort = KOREObjectCompositeSort::Create("Outer", SortCategory::Symbol);
  auto Int = KOREObjectCompositeSort::Create("Int", SortCategory::Int);

  Foo->addSort(OuterSort);
  Foo->addArgument(InnerSort);
  Foo->addArgument(Int);
  Foo->setTag(2);
  
  llvm::StringMap<KOREObjectSymbol *> map;
  map["Foo"] = Foo;
  map["Bar"] = Bar;
  map["Baz"] = Baz;
  auto compiledDt = parseYamlDecisionTreeFromString(dt, 1, map);

  llvm::LLVMContext Ctx;
  auto mod = newModule("test_decision", Ctx);
  
  auto F = KOREObjectSymbol::Create("func");
  F->addSort(Int);
  F->addArgument(OuterSort);

  makeEvalFunction(F, nullptr, mod.get(), compiledDt);

  std::string actual;
  llvm::raw_string_ostream out(actual);
  mod->print(out, nullptr);
  BOOST_CHECK_EQUAL(actual, EXPECTED);
}

BOOST_AUTO_TEST_CASE(deserialization) {
  std::string dt = R"YAML(
specializations:
- - Nil
  - action: 
    - 1
    - - - 2
- - Cons
  - specializations: []
    default:
      swap:
      - 1
      - specializations:
        - - Nil
          - action:
            - 2
            - - - 1
        - - Cons
          - action:
            - 3
            - - - 1
                - 2
              - - 1
                - 1
        default: null
        occurrence:
        - 2
    occurrence:
    - 0
    - 1
default: fail
occurrence:
- 1
)YAML";
  auto Nil = KOREObjectSymbol::Create("Nil");
  auto Cons = KOREObjectSymbol::Create("Cons");
  auto List = KOREObjectCompositeSort::Create("List", SortCategory::Symbol);
  Nil->addSort(List);
  Nil->setTag(0);
  Cons->addArgument(List);
  Cons->addArgument(List);
  Cons->addSort(List);
  Cons->setTag(1);

  llvm::StringMap<KOREObjectSymbol *> map;
  map["Nil"] = Nil;
  map["Cons"] = Cons;
  auto compiledDt = parseYamlDecisionTreeFromString(dt, 2, map);

  llvm::LLVMContext Ctx;
  auto mod = newModule("test_decision", Ctx);
  
  auto Append = KOREObjectSymbol::Create("append");
  Append->addSort(List);
  Append->addArgument(List);
  Append->addArgument(List);

  makeEvalFunction(Append, nullptr, mod.get(), compiledDt);
  std::string actual;
  llvm::raw_string_ostream out(actual);
  mod->print(out, nullptr);

  BOOST_CHECK_EQUAL(actual, R"(target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }

declare %block* @parseConfiguration(i8*)

define %block* @eval_append(%block* %subject0, %block* %subject1) {
entry:
  %0 = ptrtoint %block* %subject0 to i64
  %1 = trunc i64 %0 to i1
  br i1 %1, label %constant, label %block

stuck:                                            ; preds = %getTag7, %getTag
  call void @abort()
  unreachable

subject0_case_0:                                  ; preds = %getTag
  %2 = bitcast %block* %subject0 to { %blockheader, [0 x i64] }*
  %3 = call %block* @apply_rule_1(%block* %subject1)
  ret %block* %3

subject0_case_1:                                  ; preds = %getTag
  %4 = bitcast %block* %subject0 to { %blockheader, [0 x i64], %block*, %block* }*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  %_0 = load %block*, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  %_1 = load %block*, %block** %6
  %7 = ptrtoint %block* %_0 to i64
  %8 = trunc i64 %7 to i1
  br i1 %8, label %constant1, label %block2

constant:                                         ; preds = %entry
  %9 = lshr i64 %0, 32
  br label %getTag

block:                                            ; preds = %entry
  %10 = getelementptr inbounds %block, %block* %subject0, i64 0, i32 0, i32 0
  %11 = load i64, i64* %10
  br label %getTag

getTag:                                           ; preds = %block, %constant
  %phi = phi i64 [ %11, %block ], [ %9, %constant ]
  %12 = trunc i64 %phi to i32
  switch i32 %12, label %stuck [
    i32 0, label %subject0_case_0
    i32 1, label %subject0_case_1
  ]

_0_case_0:                                        ; preds = %getTag3
  %13 = ptrtoint %block* %subject1 to i64
  %14 = trunc i64 %13 to i1
  br i1 %14, label %constant5, label %block6

constant1:                                        ; preds = %subject0_case_1
  %15 = lshr i64 %7, 32
  br label %getTag3

block2:                                           ; preds = %subject0_case_1
  %16 = getelementptr inbounds %block, %block* %_0, i64 0, i32 0, i32 0
  %17 = load i64, i64* %16
  br label %getTag3

getTag3:                                          ; preds = %block2, %constant1
  %phi4 = phi i64 [ %17, %block2 ], [ %15, %constant1 ]
  %18 = trunc i64 %phi4 to i32
  switch i32 %18, label %_0_case_0 [
  ]

subject1_case_0:                                  ; preds = %getTag7
  %19 = bitcast %block* %subject1 to { %blockheader, [0 x i64] }*
  %20 = call %block* @apply_rule_2(%block* %subject0)
  ret %block* %20

subject1_case_1:                                  ; preds = %getTag7
  %21 = bitcast %block* %subject1 to { %blockheader, [0 x i64], %block*, %block* }*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %21, i64 0, i32 2
  %_2 = load %block*, %block** %22
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %21, i64 0, i32 3
  %_3 = load %block*, %block** %23
  %24 = call %block* @apply_rule_3(%block* %_3, %block* %_1)
  ret %block* %24

constant5:                                        ; preds = %_0_case_0
  %25 = lshr i64 %13, 32
  br label %getTag7

block6:                                           ; preds = %_0_case_0
  %26 = getelementptr inbounds %block, %block* %subject1, i64 0, i32 0, i32 0
  %27 = load i64, i64* %26
  br label %getTag7

getTag7:                                          ; preds = %block6, %constant5
  %phi8 = phi i64 [ %27, %block6 ], [ %25, %constant5 ]
  %28 = trunc i64 %phi8 to i32
  switch i32 %28, label %stuck [
    i32 0, label %subject1_case_0
    i32 1, label %subject1_case_1
  ]
}

; Function Attrs: noreturn
declare void @abort() #0

declare %block* @apply_rule_1(%block*)

declare %block* @apply_rule_2(%block*)

declare %block* @apply_rule_3(%block*, %block*)

attributes #0 = { noreturn }
)");
}

BOOST_AUTO_TEST_SUITE_END()
