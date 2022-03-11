add_library(BinaryKore
  serializer.cpp
  deserializer.cpp
)

install(
  TARGETS BinaryKore
  ARCHIVE DESTINATION lib/kllvm
)
