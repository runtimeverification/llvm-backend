kllvm_add_tool(kore-embed
  main.cpp
)

target_link_libraries(kore-embed PUBLIC fmt::fmt)

install(
  TARGETS kore-embed
  RUNTIME DESTINATION bin
)
