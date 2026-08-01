cmake_minimum_required(VERSION 3.15)
project(DragonQuest3 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

enable_testing()
include_directories(src)
add_subdirectory(test)

add_executable(DragonQuest3
	src/win_main.c
	src/pixel_buffer.c
)

set_property(TARGET DragonQuest3 PROPERTY COMPILE_WARNING_AS_ERROR ON)

if(WIN32)
   set_target_properties(DragonQuest3 PROPERTIES WIN32_EXECUTABLE TRUE)
   target_compile_options(DragonQuest3 PRIVATE /W4)
   target_link_libraries(DragonQuest3 PRIVATE winmm)
endif()

target_include_directories(DragonQuest3 PRIVATE include)
target_include_directories(DragonQuest3 PRIVATE src)
