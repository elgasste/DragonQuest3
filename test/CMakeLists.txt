cmake_minimum_required(VERSION 3.15)

add_executable(Test_Clock
	../src/clock.c
	vendor/unity.c
	test_clock.c
)

add_executable(Test_Game
	../src/game.c
	../src/mem_arena.c
	vendor/unity.c
	test_game.c
)

add_executable(Test_Input
	../src/input.c
	vendor/unity.c
	test_input.c
)

add_executable(Test_MemArena
	../src/mem_arena.c
	vendor/unity.c
	test_mem_arena.c
)

add_executable(Test_PixelBuffer
	../src/mem_arena.c
	../src/pixel_buffer.c
	vendor/unity.c
	test_pixel_buffer.c
)

add_executable(Test_Screen
	../src/screen.c
	vendor/unity.c
	test_screen.c
)

add_executable(MemArena_Perf
	../src/mem_arena.c
	mem_arena_perf.c
)

set_property(TARGET Test_Clock PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Input PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_MemArena PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_PixelBuffer PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Screen PROPERTY COMPILE_WARNING_AS_ERROR ON)

if(MSVC)
	target_compile_options(Test_Clock PRIVATE /W4)
   target_compile_options(Test_Game PRIVATE /W4)
   target_compile_options(Test_Input PRIVATE /W4)
   target_compile_options(Test_MemArena PRIVATE /W4)
   target_compile_options(Test_PixelBuffer PRIVATE /W4)
	target_compile_options(Test_Screen PRIVATE /W4)
   target_compile_options(MemArena_Perf PRIVATE /W4)
endif()

target_include_directories(Test_Clock PRIVATE vendor)
target_include_directories(Test_Clock PRIVATE ../include)
target_include_directories(Test_Clock PRIVATE ../src)

target_include_directories(Test_Game PRIVATE vendor)
target_include_directories(Test_Game PRIVATE ../include)
target_include_directories(Test_Game PRIVATE ../src)

target_include_directories(Test_Input PRIVATE vendor)
target_include_directories(Test_Input PRIVATE ../include)
target_include_directories(Test_Input PRIVATE ../src)

target_include_directories(Test_MemArena PRIVATE vendor)
target_include_directories(Test_MemArena PRIVATE ../include)
target_include_directories(Test_MemArena PRIVATE ../src)

target_include_directories(Test_PixelBuffer PRIVATE vendor)
target_include_directories(Test_PixelBuffer PRIVATE ../include)
target_include_directories(Test_PixelBuffer PRIVATE ../src)

target_include_directories(Test_Screen PRIVATE vendor)
target_include_directories(Test_Screen PRIVATE ../include)
target_include_directories(Test_Screen PRIVATE ../src)

target_include_directories(MemArena_Perf PRIVATE ../include)
target_include_directories(MemArena_Perf PRIVATE ../src)

add_test( NAME Unity_Clock_Tests COMMAND Test_Clock)
add_test( NAME Unity_Game_Tests COMMAND Test_Game)
add_test( NAME Unity_Input_Tests COMMAND Test_Input)
add_test( NAME Unity_MemArena_Tests COMMAND Test_MemArena)
add_test( NAME Unity_PixelBuffer_Tests COMMAND Test_PixelBuffer)
add_test( NAME Unity_Screen_Tests COMMAND Test_Screen)
