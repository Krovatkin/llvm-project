set(LLVM_LINK_COMPONENTS support)

add_clang_executable(if-commenter2
  if2.cpp
  )
target_link_libraries(if-commenter2
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )

