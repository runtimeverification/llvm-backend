#include <boost/test/unit_test.hpp>

#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(DecisionTest)

BOOST_AUTO_TEST_CASE(simple) {
/* match subject with 
   | Foo(l,r) -> match l with
                 | Bar(b) -> match b with
                             | true -> apply_rule_0()
                             | false -> apply_rule_1()
                 | Baz -> let b = eval_foo(r) in apply_rule_2(r, b)
   | _ -> fail()
*/
  auto Literal = SwitchNode::Create("b");
  auto dv = KOREObjectSymbol::Create("\\dv");

  Literal->addCase({dv, llvm::APInt(1, 1), LeafNode::Create("apply_rule_0")});
  Literal->addCase({dv, llvm::APInt(1, 0), LeafNode::Create("apply_rule_1")});

  auto Inner = SwitchNode::Create("l");
  auto Bar = KOREObjectSymbol::Create("Bar");
  auto Baz = KOREObjectSymbol::Create("Baz");
  auto InnerSort = KOREObjectCompositeSort::Create("Innor", SortCategory::Symbol);
  auto Bool = KOREObjectCompositeSort::Create("Bool", SortCategory::Bool);

  Bar->addSort(InnerSort);
  Bar->addArgument(Bool);
  Baz->addSort(InnerSort);
  Bar->setTag(0);
  Baz->setTag(1);

  DecisionCase BarCase{Bar, Literal};
  BarCase.addBinding("b");
  Inner->addCase(BarCase);
  auto BazLeaf = LeafNode::Create("apply_rule_2");
  BazLeaf->addBinding("r");
  BazLeaf->addBinding("b");
  auto Func = FunctionNode::Create("b", "eval_foo", BazLeaf, SortCategory::Bool);
  Func->addBinding("r");
  Inner->addCase({Baz, Func});

  auto Outer = SwitchNode::Create("subject0");
  auto Foo = KOREObjectSymbol::Create("Foo");
  auto OuterSort = KOREObjectCompositeSort::Create("Outer", SortCategory::Symbol);
  auto Int = KOREObjectCompositeSort::Create("Int", SortCategory::Int);

  Foo->addSort(OuterSort);
  Foo->addArgument(InnerSort);
  Foo->addArgument(Int);
  Foo->setTag(2);

  DecisionCase InnerCase{Foo, Inner};
  InnerCase.addBinding("l");
  InnerCase.addBinding("r");
  Outer->addCase(InnerCase);
  Outer->addCase(DecisionCase{nullptr, FailNode::get()});

  llvm::LLVMContext Ctx;
  auto mod = newModule("test_decision", Ctx);

  auto F = KOREObjectSymbol::Create("func");
  F->addSort(Int);
  F->addArgument(OuterSort);

  makeEvalFunction(F, nullptr, mod.get(), Outer);

  std::string actual;
  llvm::raw_string_ostream out(actual);
  mod->print(out, nullptr);
  BOOST_CHECK_EQUAL(actual, R"(target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }

define %mpz* @eval_func(%block* %subject0) {
entry:
  %0 = ptrtoint %block* %subject0 to i64
  %1 = trunc i64 %0 to i1
  br i1 %1, label %constant, label %block

stuck:                                            ; preds = %l_case_0, %getTag3, %getTag
  call void @abort()
  unreachable

subject0_case_0:                                  ; preds = %getTag
  %2 = bitcast %block* %subject0 to { %blockheader, [0 x i64], %block*, %mpz* }*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %2, i64 0, i32 2
  %l = load %block*, %block** %3
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %mpz* }, { %blockheader, [0 x i64], %block*, %mpz* }* %2, i64 0, i32 3
  %r = load %mpz*, %mpz** %4
  %5 = ptrtoint %block* %l to i64
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

l_case_0:                                         ; preds = %getTag3
  %11 = bitcast %block* %l to { %blockheader, [0 x i64], i1 }*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %11, i64 0, i32 2
  %b = load i1, i1* %12
  switch i1 %b, label %stuck [
    i1 true, label %b_case_0
    i1 false, label %b_case_1
  ]

l_case_1:                                         ; preds = %getTag3
  %13 = bitcast %block* %l to { %blockheader, [0 x i64] }*
  %b5 = call i1 @eval_foo(%mpz* %r)
  %14 = call %mpz* @apply_rule_2(%mpz* %r, i1 %b5)
  ret %mpz* %14

constant1:                                        ; preds = %subject0_case_0
  %15 = lshr i64 %5, 32
  br label %getTag3

block2:                                           ; preds = %subject0_case_0
  %16 = getelementptr inbounds %block, %block* %l, i64 0, i32 0, i32 0
  %17 = load i64, i64* %16
  br label %getTag3

getTag3:                                          ; preds = %block2, %constant1
  %phi4 = phi i64 [ %17, %block2 ], [ %15, %constant1 ]
  %18 = trunc i64 %phi4 to i32
  switch i32 %18, label %stuck [
    i32 0, label %l_case_0
    i32 1, label %l_case_1
  ]

b_case_0:                                         ; preds = %l_case_0
  %19 = call %mpz* @apply_rule_0()
  ret %mpz* %19

b_case_1:                                         ; preds = %l_case_0
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
)");

}

BOOST_AUTO_TEST_SUITE_END()
