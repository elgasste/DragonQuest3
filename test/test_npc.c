#include <stdlib.h>
#include <string.h>

#include "mocks/mock_game_data.h"
#include "mocks/mock_entity.h"
#include "mocks/mock_npc.h"
#include "mocks/mock_sprite.h"

#include "entity.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "npc.h"
#include "sprite.h"
#include "unity.h"

global u8 g_fileData[ sizeof( NpcInfo_t ) ];
global File_t g_file;
global size_t g_filePosition;
global u32 g_fatalErrorCount;
global u32 g_allocCount;
global u32 g_freeCount;
#if defined( _WIN32 )
WinDebugFlags_t g_winDebugFlags;
#endif

r32 Clock_GetFrameSec( Clock_t* clock )
{
	UNUSED_PARAM( clock );
	return 1.0f / 60.0f;
}

global u32 g_randU32Sequence[64];
global size_t g_randU32Index;
global size_t g_randU32Count;
global b32 g_useRandSequence;

u32 Platform_Rand_u32Ranged( u32 min, u32 max )
{
	UNUSED_PARAM( max );
	if ( g_useRandSequence && g_randU32Count > 0 )
	{
		u32 val = g_randU32Sequence[g_randU32Index % g_randU32Count];
		g_randU32Index++;
		return val;
	}
	return min;
}

i32 Platform_Rand_i32Ranged( i32 min, i32 max )
{
	UNUSED_PARAM( max );
	return min;
}

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

File_t* GameData_GetFile( GameData_t* gameData )
{
	return gameData->file;
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
	UNUSED_PARAM( file );
	UNUSED_PARAM( origin );
	g_filePosition = (size_t)offset;
}

void Platform_ReadFileBytes( File_t* file, u8* buffer, size_t size )
{
	UNUSED_PARAM( file );
	memcpy( buffer, g_fileData + g_filePosition, size );
	g_filePosition += size;
}

void Platform_FatalError( const char* msg )
{
	UNUSED_PARAM( msg );
	g_fatalErrorCount++;
}

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
	UNUSED_PARAM( textureSet );
	return 1;
}

ActiveSprite_t* ActiveSprite_Create( MemArena_t* arena, ActiveSpriteTextureSet_t* textureSet )
{
	ActiveSprite_t* sprite = (ActiveSprite_t*)MemArena_AllocMem( arena, sizeof( ActiveSprite_t ) );
	sprite->textureSet = textureSet;
	sprite->dir = Direction_Down;
	sprite->textureIndex = 0;
	return sprite;
}

void ActiveSprite_Free( ActiveSprite_t* sprite, MemArena_t* arena )
{
	MemArena_FreeMem( arena, sprite );
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* sprite ) { return sprite->dir; }
u32 ActiveSprite_GetTextureIndex( ActiveSprite_t* sprite ) { return sprite->textureIndex; }
ActiveSpriteTextureSet_t* ActiveSprite_GetTextureSet( ActiveSprite_t* sprite ) { return sprite->textureSet; }
void ActiveSprite_SetTextureIndex( ActiveSprite_t* sprite, u32 textureIndex ) { sprite->textureIndex = textureIndex; }
void ActiveSprite_SetDirection( ActiveSprite_t* sprite, Direction_t dir ) { sprite->dir = dir; }
void ActiveSprite_Tic( ActiveSprite_t* sprite, r32 deltaSec )
{
	UNUSED_PARAM( sprite );
	UNUSED_PARAM( deltaSec );
}

Entity_t* Entity_Create( MemArena_t* arena, ActiveSprite_t* sprite )
{
	Entity_t* entity = (Entity_t*)MemArena_AllocMem( arena, sizeof( Entity_t ) );
	entity->sprite = sprite;
	entity->tileIndex = 0;
	entity->rect = (Vector4i32_t){ 0, 0, 0, 0 };
	entity->velocity = (Vector2i32_t){ 0, 0 };
	entity->spriteOffset = (Vector2i32_t){ 0, 0 };
	return entity;
}

void Entity_Free( Entity_t* entity, MemArena_t* arena )
{
	ActiveSprite_Free( entity->sprite, arena );
	MemArena_FreeMem( arena, entity );
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity ) { return entity->sprite; }
Vector4i32_t Entity_GetRect( Entity_t* entity ) { return entity->rect; }
u32 Entity_GetTileIndex( Entity_t* entity ) { return entity->tileIndex; }
Vector2i32_t Entity_GetSpriteOffset( Entity_t* entity ) { return entity->spriteOffset; }
Vector2i32_t Entity_GetVelocity( Entity_t* entity ) { return entity->velocity; }
void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy ) { entity->velocity = (Vector2i32_t){ vx, vy }; }
void Entity_SetVelocityX( Entity_t* entity, i32 vx ) { entity->velocity.x = vx; }
void Entity_SetVelocityY( Entity_t* entity, i32 vy ) { entity->velocity.y = vy; }
void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex ) { entity->tileIndex = tileIndex; }
void Entity_SetPosition( Entity_t* entity, i32 x, i32 y ) { entity->rect.x = x; entity->rect.y = y; }
void Entity_SetSize( Entity_t* entity, i32 w, i32 h ) { entity->rect.w = w; entity->rect.h = h; }
void Entity_SetSpriteOffset( Entity_t* entity, i32 offsetX, i32 offsetY ) { entity->spriteOffset = (Vector2i32_t){ offsetX, offsetY }; }

internal GameData_t* CreateGameData( void )
{
	static GameData_t gameData;

	if ( g_file.size == 0 )
	{
		g_file.size = sizeof( g_fileData );
	}
	gameData.file = &g_file;
	return &gameData;
}

internal void WriteNpcInfo( void )
{
	NpcInfo_t info = { 23, 18, 20, -3, 4, Direction_Up, 7, True };

	memcpy( g_fileData, &info, sizeof( info ) );
	g_file.size = sizeof( g_fileData );
}

void setUp( void )
{
	memset( g_fileData, 0, sizeof( g_fileData ) );
	g_file.stream = g_fileData;
	g_file.size = sizeof( g_fileData );
	g_filePosition = 0;
	g_fatalErrorCount = 0;
	g_allocCount = 0;
	g_freeCount = 0;
	g_useRandSequence = False;
	g_randU32Index = 0;
	g_randU32Count = 0;
}

void tearDown( void ) {}

void test_Npc_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, Npc_GetStructSize() );
}

void test_Npc_LoadFromGameData_LoadsNpcAndEntityState( void )
{
	ActiveSpriteTextureSet_t* textureSet = (ActiveSpriteTextureSet_t*)1;
	Npc_t npc;
	Entity_t* entity;
	Vector4i32_t rect;
	Vector2i32_t spriteOffset;
	ActiveSprite_t* sprite;

	WriteNpcInfo();
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, textureSet );

	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 2, g_allocCount );
	TEST_ASSERT_TRUE( Npc_GetWanders( &npc ) );

	entity = Npc_GetEntity( &npc );
	TEST_ASSERT_NOT_NULL( entity );
	TEST_ASSERT_EQUAL_UINT( 23, Entity_GetTileIndex( entity ) );
	rect = Entity_GetRect( entity );
	TEST_ASSERT_EQUAL_INT( 18, rect.w );
	TEST_ASSERT_EQUAL_INT( 20, rect.h );
	spriteOffset = Entity_GetSpriteOffset( entity );
	TEST_ASSERT_EQUAL_INT( -3, spriteOffset.x );
	TEST_ASSERT_EQUAL_INT( 4, spriteOffset.y );

	sprite = Entity_GetSprite( entity );
	TEST_ASSERT_EQUAL_PTR( textureSet, ActiveSprite_GetTextureSet( sprite ) );
	TEST_ASSERT_EQUAL_UINT( 7, ActiveSprite_GetTextureIndex( sprite ) );
	TEST_ASSERT_EQUAL_INT( Direction_Up, ActiveSprite_GetDirection( sprite ) );

	Npc_Free( &npc, (MemArena_t*)1 );
	TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

void test_Npc_LoadFromGameData_RejectsTruncatedInfo( void )
{
	Npc_t npc;

	g_file.size = sizeof( NpcInfo_t ) - 1;
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );

	TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_freeCount );
}

void test_Npc_SetWanders_UpdatesWandersState( void )
{
	Npc_t npc;

	WriteNpcInfo();
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	Npc_SetWanders( &npc, False );

	TEST_ASSERT_FALSE( Npc_GetWanders( &npc ) );

	Npc_Free( &npc, (MemArena_t*)1 );
}

void test_Npc_Tic_PausesBrieflyAfterWandering( void )
{
	Npc_t npc;
	Entity_t* entity;
	Vector2i32_t velocity;
	Clock_t* clock = (Clock_t*)1;
	int i;

	WriteNpcInfo();
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	entity = Npc_GetEntity( &npc );

	for ( i = 0; i < 61; i++ )
	{
		Npc_Tic( &npc, clock );
	}
	velocity = Entity_GetVelocity( entity );
	TEST_ASSERT_NOT_EQUAL( 0, velocity.x );

	for ( i = 0; i < 61; i++ )
	{
		Npc_Tic( &npc, clock );
	}
	velocity = Entity_GetVelocity( entity );
	TEST_ASSERT_EQUAL_INT( 0, velocity.x );
	TEST_ASSERT_EQUAL_INT( 0, velocity.y );

	for ( i = 0; i < 16; i++ )
	{
		Npc_Tic( &npc, clock );
	}
	velocity = Entity_GetVelocity( entity );
	TEST_ASSERT_NOT_EQUAL( 0, velocity.x );

	Npc_Free( &npc, (MemArena_t*)1 );
}

void test_Npc_Tic_UpdatesSpriteDirectionMatchingVelocity( void )
{
	Npc_t npc;
	Entity_t* entity;
	ActiveSprite_t* sprite;
	Clock_t* clock = (Clock_t*)1;
	int i;

	// Test moving Left:
	// In Npc_LoadFromGameData: rand(0,1) for isWandering -> 0 (False), rand for actionSeconds -> 1000ms
	// Advance 61 frames (>1000ms) -> triggers transition from not wandering to Npc_StartWandering
	// In Npc_StartWandering:
	//   rand(0,1) for choose X -> 0
	//   rand(0,1) for sign X -> 0 (-vx, moving left)
	//   rand(0,1) for choose Y -> 1 (no Y)
	//   rand for actionSeconds -> 1000ms
	g_useRandSequence = True;
	g_randU32Sequence[0] = 0;       // Npc_LoadFromGameData: isWandering = False
	g_randU32Sequence[1] = 1000;    // Npc_LoadFromGameData: actionSeconds
	g_randU32Sequence[2] = 0;       // Npc_StartWandering: choose X
	g_randU32Sequence[3] = 0;       // Npc_StartWandering: -vx (left)
	g_randU32Sequence[4] = 1;       // Npc_StartWandering: skip Y
	g_randU32Sequence[5] = 1000;    // Npc_StartWandering: actionSeconds
	g_randU32Count = 6;
	g_randU32Index = 0;

	WriteNpcInfo(); // Initial direction is Up
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	entity = Npc_GetEntity( &npc );
	sprite = Entity_GetSprite( entity );

	TEST_ASSERT_EQUAL_INT( Direction_Up, ActiveSprite_GetDirection( sprite ) );

	for ( i = 0; i < 61; i++ )
	{
		Npc_Tic( &npc, clock );
	}

	TEST_ASSERT_LESS_THAN_INT32( 0, Entity_GetVelocity( entity ).x );
	TEST_ASSERT_EQUAL_INT( Direction_Left, ActiveSprite_GetDirection( sprite ) );

	Npc_Free( &npc, (MemArena_t*)1 );

	// Test moving Right:
	g_randU32Sequence[0] = 0;       // Npc_LoadFromGameData: isWandering = False
	g_randU32Sequence[1] = 1000;    // Npc_LoadFromGameData: actionSeconds
	g_randU32Sequence[2] = 0;       // Npc_StartWandering: choose X
	g_randU32Sequence[3] = 1;       // Npc_StartWandering: +vx (right)
	g_randU32Sequence[4] = 1;       // Npc_StartWandering: skip Y
	g_randU32Sequence[5] = 1000;    // Npc_StartWandering: actionSeconds
	g_randU32Count = 6;
	g_randU32Index = 0;

	WriteNpcInfo();
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	entity = Npc_GetEntity( &npc );
	sprite = Entity_GetSprite( entity );

	for ( i = 0; i < 61; i++ )
	{
		Npc_Tic( &npc, clock );
	}

	TEST_ASSERT_GREATER_THAN_INT32( 0, Entity_GetVelocity( entity ).x );
	TEST_ASSERT_EQUAL_INT( Direction_Right, ActiveSprite_GetDirection( sprite ) );

	Npc_Free( &npc, (MemArena_t*)1 );

	// Test moving Down:
	g_randU32Sequence[0] = 0;       // Npc_LoadFromGameData: isWandering = False
	g_randU32Sequence[1] = 1000;    // Npc_LoadFromGameData: actionSeconds
	g_randU32Sequence[2] = 1;       // Npc_StartWandering: skip X
	g_randU32Sequence[3] = 0;       // Npc_StartWandering: choose Y
	g_randU32Sequence[4] = 1;       // Npc_StartWandering: +vy (down)
	g_randU32Sequence[5] = 1000;    // Npc_StartWandering: actionSeconds
	g_randU32Count = 6;
	g_randU32Index = 0;

	WriteNpcInfo();
	Npc_LoadFromGameData( &npc, (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	entity = Npc_GetEntity( &npc );
	sprite = Entity_GetSprite( entity );

	for ( i = 0; i < 61; i++ )
	{
		Npc_Tic( &npc, clock );
	}

	TEST_ASSERT_GREATER_THAN_INT32( 0, Entity_GetVelocity( entity ).y );
	TEST_ASSERT_EQUAL_INT( Direction_Down, ActiveSprite_GetDirection( sprite ) );

	Npc_Free( &npc, (MemArena_t*)1 );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_Npc_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_Npc_LoadFromGameData_LoadsNpcAndEntityState );
	RUN_TEST( test_Npc_LoadFromGameData_RejectsTruncatedInfo );
   
	RUN_TEST( test_Npc_SetWanders_UpdatesWandersState );
	RUN_TEST( test_Npc_Tic_PausesBrieflyAfterWandering );
	RUN_TEST( test_Npc_Tic_UpdatesSpriteDirectionMatchingVelocity );

	return UNITY_END();
}
