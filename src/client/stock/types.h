namespace stock
{
	typedef unsigned char byte;
	typedef enum { qfalse, qtrue } qboolean;
	typedef void (*xcommand_t)(void);
	typedef struct vm_s vm_t;
	typedef int cvarHandle_t;
	typedef int qhandle_t;

	typedef enum
	{
		ERR_FATAL,
		ERR_DROP,
		// ...
	} errorParm_t;

	typedef enum
	{
		CA_DISCONNECTED = 0,
		CA_CONNECTING = 1,
		CA_ACTIVE = 6,
		// ...
	} connstate_t;

	typedef enum
	{
		EXEC_NOW,
		EXEC_INSERT,
		EXEC_APPEND
	} cbufExec_t;

	typedef enum
	{
		STAT_HEALTH,
		// ...
	} statIndex_t;
	
	typedef enum
	{
		// ...
		CG_MILLISECONDS = 0x6,
		CG_R_SETCOLOR = 0x48,
		CG_R_REGISTERSHADERNOMIP = 0x58,
		// ...
	} cgameImport_t;
	
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

	typedef struct
	{
		cvarHandle_t handle;
		int modificationCount;
		float value;
		int integer;
		// ...
	} vmCvar_t;

	typedef struct
	{
		vmCvar_t* vmCvar;
		char* cvarName;
		char* defaultString;
		int cvarFlags;
	} cvarTable_t;
	
	typedef struct playerState_s
	{
		byte gap_0x0[0xB0];
		int weapon; // 0xB0
		byte gap_0xB4[0x40];
		statIndex_t stats[6]; // 0xf4
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
		// ...
	} netadrtype_t;
	
	typedef struct
	{
		netadrtype_t type;
		byte ip[4];
		byte ipx[10];
		unsigned short port;
	} netadr_t;









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
}