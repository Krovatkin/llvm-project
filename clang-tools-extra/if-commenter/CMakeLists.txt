set(LLVM_LINK_COMPONENTS support)

add_clang_executable(if-commenter
  if.cpp
  )
target_link_libraries(if-commenter
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )

