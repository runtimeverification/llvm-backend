#include <boost/test/unit_test.hpp>

#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(DecisionTest)

const llvm::StringMap<KOREObjectCompositeSort *> emptyMap;

static std::string EXPECTED = R"(target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define fastcc %mpz* @"eval_func{Outer{}}"(%block* %subject0) {
entry:
  %tag = call i32 @getTag(%block* %subject0)
  switch i32 %tag, label %stuck [
    i32 2, label %subject0_case_0
  ]

stuck:                                            ; preds = %_0_case_0, %subject0_case_0, %entry
  call void @abort()
  unreachable

subject0_case_0:                                  ; preds = %entry
  %0 = bitcast %block* %subject0 to { %blockheader, [0 x i64], %block*, %mpz* }*
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %0, i64 0, i32 2
  %_0 = load %block*, %block** %1
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %0, i64 0, i32 3
  %_1 = load %mpz*, %mpz** %2
  %tag1 = call i32 @getTag(%block* %_0)
  switch i32 %tag1, label %stuck [
    i32 0, label %_0_case_0
    i32 1, label %_0_case_1
  ]

_0_case_0:                                        ; preds = %subject0_case_0
  %3 = bitcast %block* %_0 to { %blockheader, [0 x i64], i1 }*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  %_2 = load i1, i1* %4
  switch i1 %_2, label %stuck [
    i1 true, label %_2_case_0
    i1 false, label %_2_case_1
  ]

_0_case_1:                                        ; preds = %subject0_case_0
  %5 = bitcast %block* %_0 to { %blockheader, [0 x i64] }*
  %_3 = call i1 @eval_foo(%mpz* %_1)
  %6 = call fastcc %mpz* @apply_rule_2(%mpz* %_1, i1 %_3)
  ret %mpz* %6

_2_case_0:                                        ; preds = %_0_case_0
  %7 = call fastcc %mpz* @apply_rule_0()
  ret %mpz* %7

_2_case_1:                                        ; preds = %_0_case_0
  %8 = call fastcc %mpz* @apply_rule_1()
  ret %mpz* %8
}

; Function Attrs: noreturn
declare void @abort() #0

declare i32 @getTag(%block*)

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
  auto Literal = SwitchNode::Create("_2", false);
  auto dv = KOREObjectSymbol::Create("\\dv");

  Literal->addCase({dv, llvm::APInt(1, 1), LeafNode::Create("apply_rule_0")});
  Literal->addCase({dv, llvm::APInt(1, 0), LeafNode::Create("apply_rule_1")});

  auto Inner = SwitchNode::Create("_0", false);
  auto Bar = KOREObjectSymbol::Create("Bar");
  auto Baz = KOREObjectSymbol::Create("Baz");
  auto InnerSort = KOREObjectCompositeSort::Create("Inner", {SortCategory::Symbol, 0});
  auto Bool = KOREObjectCompositeSort::Create("Bool", {SortCategory::Bool, 0});

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
  auto Func = FunctionNode::Create("_3", "eval_foo", BazLeaf, {SortCategory::Bool, 0});
  Func->addBinding("_1");
  Inner->addCase({Baz, std::vector<std::string>{}, Func});

  auto Outer = SwitchNode::Create("subject0", false);
  auto Foo = KOREObjectSymbol::Create("Foo");
  auto OuterSort = KOREObjectCompositeSort::Create("Outer", {SortCategory::Symbol, 0});
  auto Int = KOREObjectCompositeSort::Create("Int", {SortCategory::Int, 0});

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
        occurrence:
        - 0
        - 0
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
  auto InnerSort = KOREObjectCompositeSort::Create("Inner", {SortCategory::Symbol, 0});
  auto Bool = KOREObjectCompositeSort::Create("Bool", {SortCategory::Bool, 0});

  Bar->addSort(InnerSort);
  Bar->addArgument(Bool);
  Baz->addSort(InnerSort);
  Bar->setTag(0);
  Baz->setTag(1);
  
  auto Foo = KOREObjectSymbol::Create("Foo");
  auto OuterSort = KOREObjectCompositeSort::Create("Outer", {SortCategory::Symbol, 0});
  auto Int = KOREObjectCompositeSort::Create("Int", {SortCategory::Int, 0});

  Foo->addSort(OuterSort);
  Foo->addArgument(InnerSort);
  Foo->addArgument(Int);
  Foo->setTag(2);
  
  llvm::StringMap<KOREObjectSymbol *> map;
  map["Foo"] = Foo;
  map["Bar"] = Bar;
  map["Baz"] = Baz;
  auto compiledDt = parseYamlDecisionTreeFromString(dt, map, emptyMap);

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

BOOST_AUTO_TEST_CASE(stepper) {
  std::string dt = "fail";

  llvm::StringMap<KOREObjectSymbol *> map;
  auto compiledDt = parseYamlDecisionTreeFromString(dt, map, emptyMap);

  llvm::LLVMContext Ctx;
  auto mod = newModule("test_decision", Ctx);
  
  makeStepFunction(nullptr, mod.get(), compiledDt);
  std::string actual;
  llvm::raw_string_ostream out(actual);
  mod->print(out, nullptr);

  BOOST_CHECK_EQUAL(actual, R"(target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define %block* @step(%block* %subject0) {
entry:
  br label %stuck

stuck:                                            ; preds = %entry
  call void @finish_rewriting(%block* %subject0)
  unreachable
}

; Function Attrs: noreturn
declare void @finish_rewriting(%block*) #0

attributes #0 = { noreturn }
)");
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
  auto List = KOREObjectCompositeSort::Create("List", {SortCategory::Symbol, 0});
  Nil->addSort(List);
  Nil->setTag(0);
  Cons->addArgument(List);
  Cons->addArgument(List);
  Cons->addSort(List);
  Cons->setTag(1);

  llvm::StringMap<KOREObjectSymbol *> map;
  map["Nil"] = Nil;
  map["Cons"] = Cons;
  auto compiledDt = parseYamlDecisionTreeFromString(dt, map, emptyMap);

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

declare void @printConfiguration(i32, %block*)

define fastcc %block* @"eval_append{List{}, List{}}"(%block* %subject0, %block* %subject1) {
entry:
  %tag = call i32 @getTag(%block* %subject0)
  switch i32 %tag, label %stuck [
    i32 0, label %subject0_case_0
    i32 1, label %subject0_case_1
  ]

stuck:                                            ; preds = %_0_case_0, %entry
  call void @abort()
  unreachable

subject0_case_0:                                  ; preds = %entry
  %0 = bitcast %block* %subject0 to { %blockheader, [0 x i64] }*
  %1 = call fastcc %block* @apply_rule_1(%block* %subject1)
  ret %block* %1

subject0_case_1:                                  ; preds = %entry
  %2 = bitcast %block* %subject0 to { %blockheader, [0 x i64], %block*, %block* }*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  %_0 = load %block*, %block** %3
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  %_1 = load %block*, %block** %4
  br label %_0_case_0

_0_case_0:                                        ; preds = %subject0_case_1
  %tag1 = call i32 @getTag(%block* %subject1)
  switch i32 %tag1, label %stuck [
    i32 0, label %subject1_case_0
    i32 1, label %subject1_case_1
  ]

subject1_case_0:                                  ; preds = %_0_case_0
  %5 = bitcast %block* %subject1 to { %blockheader, [0 x i64] }*
  %6 = call fastcc %block* @apply_rule_2(%block* %subject0)
  ret %block* %6

subject1_case_1:                                  ; preds = %_0_case_0
  %7 = bitcast %block* %subject1 to { %blockheader, [0 x i64], %block*, %block* }*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  %_2 = load %block*, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  %_3 = load %block*, %block** %9
  %10 = call fastcc %block* @apply_rule_3(%block* %_3, %block* %_1)
  ret %block* %10
}

; Function Attrs: noreturn
declare void @abort() #0

declare i32 @getTag(%block*)

declare %block* @apply_rule_1(%block*)

declare %block* @apply_rule_2(%block*)

declare %block* @apply_rule_3(%block*, %block*)

attributes #0 = { noreturn }
)");
}

BOOST_AUTO_TEST_SUITE_END()
