cmake_minimum_required(VERSION 3.15)

add_executable(Test_Clock
	../src/clock.c
	vendor/unity.c
	test_clock.c
)

add_executable(Test_Display
	vendor/unity.c
	test_display.c
)

add_executable(Test_Game
	vendor/unity.c
	test_game.c
)

add_executable(Test_Game_Render
	../src/game_render.c
	vendor/unity.c
	test_game_render.c
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
	vendor/unity.c
	test_pixel_buffer.c
)

add_executable(Test_TileMap
	vendor/unity.c
	test_tile_map.c
)

set_property(TARGET Test_Clock PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Display PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game_Render PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Input PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_MemArena PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_PixelBuffer PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_TileMap PROPERTY COMPILE_WARNING_AS_ERROR ON)

if(MSVC)
	target_compile_options(Test_Clock PRIVATE /W4)
	target_compile_options(Test_Display PRIVATE /W4)
   target_compile_options(Test_Game PRIVATE /W4)
	target_compile_options(Test_Game_Render PRIVATE /W4)
   target_compile_options(Test_Input PRIVATE /W4)
   target_compile_options(Test_MemArena PRIVATE /W4)
   target_compile_options(Test_PixelBuffer PRIVATE /W4)
	target_compile_options(Test_TileMap PRIVATE /W4)
endif()

target_include_directories(Test_Clock PRIVATE vendor)
target_include_directories(Test_Clock PRIVATE ../include)
target_include_directories(Test_Clock PRIVATE ../src)

target_include_directories(Test_Display PRIVATE vendor)
target_include_directories(Test_Display PRIVATE mocks)
target_include_directories(Test_Display PRIVATE ../include)
target_include_directories(Test_Display PRIVATE ../src)

target_include_directories(Test_Game PRIVATE vendor)
target_include_directories(Test_Game PRIVATE mocks)
target_include_directories(Test_Game PRIVATE ../include)
target_include_directories(Test_Game PRIVATE ../src)

target_include_directories(Test_Game_Render PRIVATE vendor)
target_include_directories(Test_Game_Render PRIVATE ../include)
target_include_directories(Test_Game_Render PRIVATE ../src)

target_include_directories(Test_Input PRIVATE vendor)
target_include_directories(Test_Input PRIVATE ../include)
target_include_directories(Test_Input PRIVATE ../src)

target_include_directories(Test_MemArena PRIVATE vendor)
target_include_directories(Test_MemArena PRIVATE ../include)
target_include_directories(Test_MemArena PRIVATE ../src)

target_include_directories(Test_PixelBuffer PRIVATE vendor)
target_include_directories(Test_PixelBuffer PRIVATE ../include)
target_include_directories(Test_PixelBuffer PRIVATE ../src)

target_include_directories(Test_TileMap PRIVATE vendor)
target_include_directories(Test_TileMap PRIVATE ../include)
target_include_directories(Test_TileMap PRIVATE ../src)

add_test( NAME Unity_Clock_Tests COMMAND Test_Clock)
add_test( NAME Unity_Display_Tests COMMAND Test_Display)
add_test( NAME Unity_Game_Tests COMMAND Test_Game)
add_test( NAME Unity_Game_Render_Tests COMMAND Test_Game_Render)
add_test( NAME Unity_Input_Tests COMMAND Test_Input)
add_test( NAME Unity_MemArena_Tests COMMAND Test_MemArena)
add_test( NAME Unity_PixelBuffer_Tests COMMAND Test_PixelBuffer)
add_test( NAME Unity_TileMap_Tests COMMAND Test_TileMap)
