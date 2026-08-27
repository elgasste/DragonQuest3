#include <stdlib.h>

#include "entity.h"
#include "mem_arena.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_tileIndexChangedCount;
global Entity_t* g_tileIndexChangedEntity;
global u32 g_oldTileIndex;
global u32 g_newTileIndex;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   g_allocCount++;
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   g_freeCount++;
   free( mem );
}

void setUp( void )
{
   g_allocCount = 0;
   g_freeCount = 0;
   g_tileIndexChangedCount = 0;
   g_tileIndexChangedEntity = 0;
   g_oldTileIndex = 0;
   g_newTileIndex = 0;
}

void OnTileIndexChanged( Entity_t* entity, u32 oldTileIndex, u32 newTileIndex )
{
   g_tileIndexChangedCount++;
   g_tileIndexChangedEntity = entity;
   g_oldTileIndex = oldTileIndex;
   g_newTileIndex = newTileIndex;
}

void tearDown( void ) {}

void test_Entity_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, Entity_GetStructSize() );
}

void test_Entity_Create_InitializesSpriteToNull( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   TEST_ASSERT_NULL( Entity_GetSprite( entity ) );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetSprite_UpdatesSprite( void )
{
   ActiveSprite_t* expectedSprite = (ActiveSprite_t*)1;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetSprite( entity, expectedSprite );

   TEST_ASSERT_EQUAL_PTR( expectedSprite, Entity_GetSprite( entity ) );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetSpriteOffset_UpdatesOffset( void )
{
   Vector2i32_t spriteOffset;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetSpriteOffset( entity, -12, 18 );
   spriteOffset = Entity_GetSpriteOffset( entity );

   TEST_ASSERT_EQUAL_INT( -12, spriteOffset.x );
   TEST_ASSERT_EQUAL_INT( 18, spriteOffset.y );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_Setters_ReplaceSpriteAndSpriteOffset( void )
{
   Vector2i32_t spriteOffset;
   ActiveSprite_t* expectedSprite = (ActiveSprite_t*)2;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetSprite( entity, (ActiveSprite_t*)1 );
   Entity_SetSpriteOffset( entity, 3, 4 );
   Entity_SetSprite( entity, expectedSprite );
   Entity_SetSpriteOffset( entity, 7, -9 );
   spriteOffset = Entity_GetSpriteOffset( entity );

   TEST_ASSERT_EQUAL_PTR( expectedSprite, Entity_GetSprite( entity ) );
   TEST_ASSERT_EQUAL_INT( 7, spriteOffset.x );
   TEST_ASSERT_EQUAL_INT( -9, spriteOffset.y );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetPosition_UpdatesOnlyPosition( void )
{
   Vector4i32_t rect;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetSize( entity, 30, 40 );
   Entity_SetPosition( entity, -15, 25 );
   rect = Entity_GetRect( entity );

   TEST_ASSERT_EQUAL_INT( -15, rect.x );
   TEST_ASSERT_EQUAL_INT( 25, rect.y );
   TEST_ASSERT_EQUAL_INT( 30, rect.w );
   TEST_ASSERT_EQUAL_INT( 40, rect.h );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetSize_UpdatesOnlySize( void )
{
   Vector4i32_t rect;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetPosition( entity, 1, 2 );
   Entity_SetSize( entity, 75, 90 );
   rect = Entity_GetRect( entity );

   TEST_ASSERT_EQUAL_INT( 1, rect.x );
   TEST_ASSERT_EQUAL_INT( 2, rect.y );
   TEST_ASSERT_EQUAL_INT( 75, rect.w );
   TEST_ASSERT_EQUAL_INT( 90, rect.h );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetVelocity_UpdatesVelocity( void )
{
   Vector2i32_t velocity;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetVelocity( entity, -7, 11 );
   velocity = Entity_GetVelocity( entity );

   TEST_ASSERT_EQUAL_INT( -7, velocity.x );
   TEST_ASSERT_EQUAL_INT( 11, velocity.y );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_GetVelocity_ReturnsLatestVelocity( void )
{
   Vector2i32_t velocity;
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetVelocity( entity, 3, -4 );
   Entity_SetVelocity( entity, 12, 18 );
   velocity = Entity_GetVelocity( entity );

   TEST_ASSERT_EQUAL_INT( 12, velocity.x );
   TEST_ASSERT_EQUAL_INT( 18, velocity.y );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetTileIndex_UpdatesTileIndex( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetTileIndex( entity, 7 );

   TEST_ASSERT_EQUAL_UINT( 7, Entity_GetTileIndex( entity ) );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_GetTileIndex_ReturnsLatestTileIndex( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetTileIndex( entity, 3 );
   Entity_SetTileIndex( entity, 12 );

   TEST_ASSERT_EQUAL_UINT( 12, Entity_GetTileIndex( entity ) );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetOnTileIndexChanged_NotifiesCallbackWithTileIndices( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetOnTileIndexChanged( entity, OnTileIndexChanged );
   Entity_SetTileIndex( entity, 7 );

   TEST_ASSERT_EQUAL_UINT( 1, g_tileIndexChangedCount );
   TEST_ASSERT_EQUAL_PTR( entity, g_tileIndexChangedEntity );
   TEST_ASSERT_EQUAL_UINT( 0, g_oldTileIndex );
   TEST_ASSERT_EQUAL_UINT( 7, g_newTileIndex );
   TEST_ASSERT_EQUAL_UINT( 7, Entity_GetTileIndex( entity ) );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_SetOnTileIndexChanged_NotifiesOnEachTileChange( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_SetOnTileIndexChanged( entity, OnTileIndexChanged );
   Entity_SetTileIndex( entity, 3 );
   Entity_SetTileIndex( entity, 12 );

   TEST_ASSERT_EQUAL_UINT( 2, g_tileIndexChangedCount );
   TEST_ASSERT_EQUAL_UINT( 3, g_oldTileIndex );
   TEST_ASSERT_EQUAL_UINT( 12, g_newTileIndex );

   Entity_Free( entity, (MemArena_t*)1 );
}

void test_Entity_Free_ReleasesAllocatedEntity( void )
{
   Entity_t* entity = Entity_Create( (MemArena_t*)1 );

   Entity_Free( entity, (MemArena_t*)1 );

   TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_freeCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Entity_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_Entity_Create_InitializesSpriteToNull );
   RUN_TEST( test_Entity_SetSprite_UpdatesSprite );
   RUN_TEST( test_Entity_SetSpriteOffset_UpdatesOffset );
   RUN_TEST( test_Entity_Setters_ReplaceSpriteAndSpriteOffset );

   RUN_TEST( test_Entity_SetPosition_UpdatesOnlyPosition );

   RUN_TEST( test_Entity_SetSize_UpdatesOnlySize );

   RUN_TEST( test_Entity_SetVelocity_UpdatesVelocity );

   RUN_TEST( test_Entity_GetVelocity_ReturnsLatestVelocity );

   RUN_TEST( test_Entity_SetTileIndex_UpdatesTileIndex );
   RUN_TEST( test_Entity_GetTileIndex_ReturnsLatestTileIndex );

   RUN_TEST( test_Entity_SetOnTileIndexChanged_NotifiesCallbackWithTileIndices );
   RUN_TEST( test_Entity_SetOnTileIndexChanged_NotifiesOnEachTileChange );
   
   RUN_TEST( test_Entity_Free_ReleasesAllocatedEntity );

   return UNITY_END();
}