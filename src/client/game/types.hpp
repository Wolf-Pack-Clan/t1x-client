#pragma once

namespace game
{
	typedef unsigned char byte;
	typedef enum { qfalse, qtrue } qboolean;
	typedef void (*xcommand_t)(void);
	typedef struct vm_s vm_t;
	
	typedef struct cvar_s
	{
		char* name;
		char* string;
		char* resetString;
		char* latchedString;
		int flags;
		qboolean modified;
		int modificationCount;
		float value;
		int integer;
		// ...
	} cvar_t;
	
	typedef struct playerState_s
	{
		byte gap_0x0[0xB0];
		int weapon; // 0xB0
		// ...
	} playerState_t;
	
	typedef struct
	{
		playerState_t* ps;
		// ...
	} pmove_t;
	
	typedef struct weaponInfo_s
	{
		byte gap_0x0[0x224];
		char* adsOverlayShader;	// 0x224
		// ...
	} weaponInfo_t;

	typedef enum
	{
		ERR_FATAL,
		ERR_DROP,
		// ...
	} errorParm_t;















	/*typedef struct
	{
		float x;
		float y;
		float w;
		float h;
	} rectDef_t;
	typedef rectDef_t Rectangle;
	typedef struct
	{
		Rectangle rect;
		Rectangle rectClient;
		const char* name;
		// ...
	} windowDef_t;

	typedef windowDef_t Window;
	typedef struct itemDef_s
	{
		Window window;
		// ...
	} itemDef_t;*/


	/*typedef enum
	{
		CA_ACTIVE = 6
	} connstate_t;*/


}