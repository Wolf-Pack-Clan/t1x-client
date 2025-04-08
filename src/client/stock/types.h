namespace stock
{
	typedef unsigned char byte;
	typedef enum { qfalse, qtrue } qboolean;
	typedef void (*xcommand_t)(void);
	typedef int cvarHandle_t;
	typedef int qhandle_t;
	typedef float vec_t;
	typedef vec_t vec3_t[3];

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
	
	/*typedef struct playerState_s
	{
		byte gap_0x0[0xB0];
		int weapon; // 0xB0
		byte gap_0xB4[0x40];
		statIndex_t stats[6]; // 0xf4
		// ...
	} playerState_t;//*/
	
	typedef struct playerState_s
	{
		char gap1[12];
		int pm_flags;
		int pm_time;
		vec3_t origin;
		vec3_t velocity;
		char gap2[56];
		int jumpTime;
		float jumpOriginZ;
		//...
	} playerState_t;
	
	
	typedef struct
	{
		playerState_t* ps;
		// ...
	} pmove_t;







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
	
	typedef struct weaponinfo_t
	{
		int number; //0x0000
		char *name; //0x0004
		char *displayName; //0x0008
		char *AIOverlayDescription; //0x000C
		char *gunModel; //0x0010
		char *handModel; //0x0014
		char pad_0018[4]; //0x0018
		char *idleAnim; //0x001C
		char *emptyIdleAnim; //0x0020
		char *fireAnim; //0x0024
		char *holdFireAnim; //0x0028
		char *lastShotAnim; //0x002C
		char pad_0030[4]; //0x0030
		char *meleeAnim; //0x0034
		char *reloadAnim; //0x0038
		char *reloadEmptyAnim; //0x003C
		char pad_0040[8]; //0x0040
		char *raiseAnim; //0x0048
		char *dropAnim; //0x004C
		char *altRaiseAnim; //0x0050
		char *altDropAnim; //0x0054
		char *adsFireAnim; //0x0058
		char *adsLastShotAnim; //0x005C
		char pad_0060[16]; //0x0060
		char *adsUpAnim; //0x0070
		char *adsDownAnim; //0x0074
		char *modeName; //0x0078
		char pad_007C[24]; //0x007C
		char *viewFlashEffect; //0x0094
		char *worldFlashEffect; //0x0098
		char *pickupSound; //0x009C
		char pad_00A0[8]; //0x00A0
		char *pullbackSound; //0x00A8
		char *fireSound; //0x00AC
		char pad_00B0[20]; //0x00B0
		char *reloadSound; //0x00C4
		char *reloadEmptySound; //0x00C8
		char pad_00CC[12]; //0x00CC
		char *altSwitchSound; //0x00D8
		char pad_00DC[36]; //0x00DC
		char *shellEjectEffect; //0x0100
		char pad_0104[4]; //0x0104
		char *reticle; //0x0108
		char *reticleSide; //0x010C
		char pad_0110[180]; //0x0110
		char *radiantName; //0x01C4
		char *worldModel; //0x01C8
		char pad_01CC[8]; //0x01CC
		char *hudIcon; //0x01D4
		char *modeIcon; //0x01D8
		char *ammoIcon; //0x01DC
		int startAmmo; //0x01E0
		char *ammoName;
		int ammoOrClipIndex; // not sure
		char *clipName;
		int clientIndex; //0x01F0 // could this be clipIndex?
		int maxAmmo; //0x01F4
		int clipSize; //0x01F8
		char *sharedAmmoCapName; //0x01FC
		char pad_0200[8]; //0x0200
		int damage; //0x0208
		char gap1[4];
		int minDamagePercent;
		int damageInnerRadius; //0x0214
		int damageOuterRadius; //0x0218
		char gap2[44];
		int reloadAddTime;
		char gap3[28];
		int fuseTime; //0x0268
		float moveSpeedScale; // 0x026c
		float adsSensitivity; // 0x0270
		float adsZoomFov; // 0x0274 <-----|
		float adsZoomInFrac; // 0x0278 <--|--- just guessed these and they seem to work
		float adsZoomOutFrac; // 0x027c <-|
		char *adsOverlayShader; // 0x0280
		char gap4[4]; // not sure if this is adsOverlayReticle
		float adsOverlayWidth;
		float adsOverlayHeight;
		float adsBobFactor;
		float adsViewBobMult;
		float hipSpreadStandMin;
		float hipSpreadDuckedMin;
		float hipSpreadProneMin;
		float hipSpreadMax;
		float hipSpreadDecayRate;
		float hipSpreadFireAdd;
		float hipSpreadTurnAdd;
		float hipSpreadMoveAdd;
		float hipSpreadDuckedDecay;
		float hipSpreadProneDecay;
		float hipReticleSidePos;
		int adsTransInTime;
		int adsTransOutTime;
		float adsIdleAmount;
		float hipIdleAmount;
		float idleCrouchFactor;
		float idleProneFactor;
		float gunMaxPitch;
		float gunMaxYaw;
		float swayMaxAngle;
		float swayLerpSpeed;
		float swayPitchScale;
		float swayYawScale;
		float swayHorizScale;
		float swayVertScale;
		float swayShellShockScale;
		float adsSwayMaxAngle;
		float adsSwayLerpSpeed;
		float adsSwayPitchScale;
		float adsSwayYawScale;
		float adsSwayHorizScale;
		float adsSwayVertScale;
		int twoHanded;
		int rifleBullet;
		int semiAuto;
		int boltAction;
		int aimDownSight;
		int rechamberWhileAds;
		float adsViewErrorMin;
		float adsViewErrorMax;
		char gap5[12];
		int wideListIcon;
		char gap6[8];
		char *killIcon; //0x0350
		char pad_0354[20]; //0x0354
		char *altWeapon; //0x0368
		char pad_036C[12]; //0x036C
		int explosionRadius; //0x0378
		int explosionInnerDamage; //0x037C
		int explosionOuterDamage; //0x0380
		char pad_0384[8]; //0x0384
		char* projectileModel; //0x038C
		char gap7[292];
		float OOPosAnimLength[2]; // 0x4b4
		//...
	} weaponinfo_t;

	struct WeaponProperties // Custom struct for g_legacyStyle
	{
		int reloadAddTime;
		int adsTransInTime;
		float adsZoomInFrac;
		float idleCrouchFactor;
		float idleProneFactor;
		int rechamberWhileAds;
		float adsViewErrorMin;
		float adsViewErrorMax;
	};
}