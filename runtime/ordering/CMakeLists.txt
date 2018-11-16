set(LLVM_REQUIRES_EH ON)

add_library(ordering STATIC
	ordering.cpp
)

install(
  TARGETS ordering
  ARCHIVE DESTINATION lib/kllvm
)
