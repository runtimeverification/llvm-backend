macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})

  llvm_config(${name}
    USE_SHARED true
    core irreader
  )

  if(APPLE)
    if(NOT USE_NIX)
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath ${BREW_PREFIX}/lib" "-ldl")
    else()
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-ldl")
    endif() # NOT USE_NIX
  else()
    if(NOT USE_NIX)
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath /usr/local/lib" "-ldl")
    else()
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-ldl")
    endif() # NOT USE_NIX
  endif() # APPLE
endmacro(kllvm_add_tool)

add_subdirectory(llvm-backend-version)
add_subdirectory(llvm-kompile-codegen)
add_subdirectory(llvm-kompile-gc-stats)
add_subdirectory(kprint)
add_subdirectory(kore-arity)
add_subdirectory(kore-convert)
add_subdirectory(kore-expand-macros)
add_subdirectory(kore-header)
add_subdirectory(kore-proof-trace)
add_subdirectory(kore-strip)
add_subdirectory(kore-split)
add_subdirectory(k-rule-apply)
add_subdirectory(k-rule-find)
