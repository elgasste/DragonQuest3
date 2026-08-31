cmake_minimum_required(VERSION 3.15)

add_executable(Test_Clock
   ../src/clock.c
   vendor/unity.c
   test_clock.c
)

add_executable(Test_Display
   ../src/display.c
   ../src/utility.c
   vendor/unity.c
   test_display.c
)

add_executable(Test_Entity
   ../src/entity.c
   vendor/unity.c
   test_entity.c
)

add_executable(Test_Animation
   ../src/animation.c
   vendor/unity.c
   test_animation.c
)

add_executable(Test_Game
   ../src/game.c
   vendor/unity.c
   test_game.c
)

add_executable(Test_Game_Data
   ../src/game_data.c
   vendor/unity.c
   test_game_data.c
)

add_executable(Test_Game_Physics
   ../src/game_physics.c
   vendor/unity.c
   test_game_physics.c
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
   ../src/pixel_buffer.c
   vendor/unity.c
   test_pixel_buffer.c
)

add_executable(Test_Sprite
   ../src/sprite.c
   vendor/unity.c
   test_sprite.c
)

add_executable(Test_Sprite_Texture_Set
   ../src/sprite_texture_set.c
   vendor/unity.c
   test_sprite_texture_set.c
)

add_executable(Test_TileMap
   ../src/tile_map.c
   ../src/utility.c
   vendor/unity.c
   test_tile_map.c
)

add_executable(Test_Tile_Texture_Set
   ../src/tile_texture_set.c
   vendor/unity.c
   test_tile_texture_set.c
)

add_executable(Test_Utility
   ../src/utility.c
   vendor/unity.c
   test_utility.c
)

set_property(TARGET Test_Clock PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Display PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Entity PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Animation PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game_Physics PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game_Data PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Game_Render PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Input PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_MemArena PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_PixelBuffer PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Sprite PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Sprite_Texture_Set PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_TileMap PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Tile_Texture_Set PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET Test_Utility PROPERTY COMPILE_WARNING_AS_ERROR ON)

if(MSVC)
   add_compile_options(/wd4201)
   target_compile_options(Test_Clock PRIVATE /W4)
   target_compile_options(Test_Entity PRIVATE /W4)
   target_compile_options(Test_Animation PRIVATE /W4)
   target_compile_options(Test_Display PRIVATE /W4)
   target_compile_options(Test_Game PRIVATE /W4)
   target_compile_options(Test_Game_Physics PRIVATE /W4 /wd4201)
   target_compile_options(Test_Game_Data PRIVATE /W4)
   target_compile_options(Test_Game_Render PRIVATE /W4)
   target_compile_options(Test_Input PRIVATE /W4)
   target_compile_options(Test_MemArena PRIVATE /W4)
   target_compile_options(Test_PixelBuffer PRIVATE /W4)
   target_compile_options(Test_Sprite PRIVATE /W4)
   target_compile_options(Test_Sprite_Texture_Set PRIVATE /W4)
   target_compile_options(Test_TileMap PRIVATE /W4)
   target_compile_options(Test_Tile_Texture_Set PRIVATE /W4)
   target_compile_options(Test_Utility PRIVATE /W4)
endif()

target_include_directories(Test_Animation PRIVATE vendor)
target_include_directories(Test_Animation PRIVATE ../include)
target_include_directories(Test_Animation PRIVATE ../src)

target_include_directories(Test_Clock PRIVATE vendor)
target_include_directories(Test_Clock PRIVATE ../include)
target_include_directories(Test_Clock PRIVATE ../src)

target_include_directories(Test_Display PRIVATE vendor)
target_include_directories(Test_Display PRIVATE mocks)
target_include_directories(Test_Display PRIVATE ../include)
target_include_directories(Test_Display PRIVATE ../src)

target_include_directories(Test_Entity PRIVATE vendor)
target_include_directories(Test_Entity PRIVATE ../include)
target_include_directories(Test_Entity PRIVATE ../src)

target_include_directories(Test_Game PRIVATE vendor)
target_include_directories(Test_Game PRIVATE mocks)
target_include_directories(Test_Game PRIVATE ../include)
target_include_directories(Test_Game PRIVATE ../src)

target_include_directories(Test_Game_Physics PRIVATE vendor)
target_include_directories(Test_Game_Physics PRIVATE mocks)
target_include_directories(Test_Game_Physics PRIVATE ../include)
target_include_directories(Test_Game_Physics PRIVATE ../src)

target_include_directories(Test_Game_Data PRIVATE vendor)
target_include_directories(Test_Game_Data PRIVATE mocks)
target_include_directories(Test_Game_Data PRIVATE ../include)
target_include_directories(Test_Game_Data PRIVATE ../src)

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

target_include_directories(Test_Sprite PRIVATE vendor)
target_include_directories(Test_Sprite PRIVATE ../include)
target_include_directories(Test_Sprite PRIVATE ../src)

target_include_directories(Test_Sprite_Texture_Set PRIVATE vendor)
target_include_directories(Test_Sprite_Texture_Set PRIVATE ../include)
target_include_directories(Test_Sprite_Texture_Set PRIVATE ../src)

target_include_directories(Test_TileMap PRIVATE vendor)
target_include_directories(Test_TileMap PRIVATE mocks)
target_include_directories(Test_TileMap PRIVATE ../include)
target_include_directories(Test_TileMap PRIVATE ../src)

target_include_directories(Test_Tile_Texture_Set PRIVATE vendor)
target_include_directories(Test_Tile_Texture_Set PRIVATE mocks)
target_include_directories(Test_Tile_Texture_Set PRIVATE ../include)
target_include_directories(Test_Tile_Texture_Set PRIVATE ../src)

target_include_directories(Test_Utility PRIVATE vendor)
target_include_directories(Test_Utility PRIVATE ../include)
target_include_directories(Test_Utility PRIVATE ../src)

add_test( NAME Unity_Animation_Tests COMMAND Test_Animation)
add_test( NAME Unity_Clock_Tests COMMAND Test_Clock)
add_test( NAME Unity_Display_Tests COMMAND Test_Display)
add_test( NAME Unity_Entity_Tests COMMAND Test_Entity)
add_test( NAME Unity_Game_Tests COMMAND Test_Game)
add_test( NAME Unity_Game_Data_Tests COMMAND Test_Game_Data)
add_test( NAME Unity_Game_Physics_Tests COMMAND Test_Game_Physics)
add_test( NAME Unity_Game_Render_Tests COMMAND Test_Game_Render)
add_test( NAME Unity_Input_Tests COMMAND Test_Input)
add_test( NAME Unity_MemArena_Tests COMMAND Test_MemArena)
add_test( NAME Unity_PixelBuffer_Tests COMMAND Test_PixelBuffer)
add_test( NAME Unity_Sprite_Tests COMMAND Test_Sprite)
add_test( NAME Unity_Sprite_Texture_Set_Tests COMMAND Test_Sprite_Texture_Set)
add_test( NAME Unity_TileMap_Tests COMMAND Test_TileMap)
add_test( NAME Unity_Tile_Texture_Set_Tests COMMAND Test_Tile_Texture_Set)
add_test( NAME Unity_Utility_Tests COMMAND Test_Utility)
