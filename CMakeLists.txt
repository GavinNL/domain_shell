project(domain_shell)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

FILE(GLOB files    "./*.h"
                                 "./*.cpp"
                                 "./*.md"
                                 ".gitignore" "makefile" "LICENSE")

add_custom_target(headers    SOURCES ${files})

include_directories ("${PROJECT_SOURCE_DIR}")


  # set stuff for other systems
  ADD_DEFINITIONS(
      -std=c++11
      -Wfloat-equal
  #    -Wundef
      -Werror=shadow
      -Wpointer-arith
      -Wcast-align
  #    -Wstrict-prototypes
      -Wstrict-overflow=5
      -Wwrite-strings
  #    -Waggregate-return
      -Wcast-qual
      -Werror=switch-default
  #    -Wswitch-enum
  #    -Wconversion
      -Wunreachable-code
     # -Werror
      #-Werror=unused-function
      -Werror=return-type
      -Werror=cast-qual

      -pedantic-errors
      -Wfatal-errors
  )





add_executable(domain_shell main.cpp)
target_link_libraries(domain_shell pthread)
