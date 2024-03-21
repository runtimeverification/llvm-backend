add_library( timer STATIC
  timer.cpp
)

install(
  TARGETS timer
  ARCHIVE DESTINATION lib/kllvm
)
