cmake_minimum_required(VERSION 3.15)
project(DragonQuest3 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

enable_testing()
include_directories(src)

add_executable(DragonQuest3
   src/clock.c
   src/game.c
   src/game_load.c
   src/game_render.c
   src/input.c
   src/mem_arena.c
	src/pixel_buffer.c
   src/screen.c
)

set_property(TARGET DragonQuest3 PROPERTY COMPILE_WARNING_AS_ERROR ON)

if(WIN32)
   target_sources(DragonQuest3 PRIVATE
      src/win_main.c
      src/win_platform.c)
   set_target_properties(DragonQuest3 PROPERTIES WIN32_EXECUTABLE TRUE)
   target_compile_options(DragonQuest3 PRIVATE /W4)
   target_link_libraries(DragonQuest3 PRIVATE winmm msimg32 shlwapi)
endif()

target_include_directories(DragonQuest3 PRIVATE include)
target_include_directories(DragonQuest3 PRIVATE src)

add_subdirectory(test)
