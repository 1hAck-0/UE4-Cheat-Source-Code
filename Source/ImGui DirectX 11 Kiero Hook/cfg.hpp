#include <Windows.h>



struct cfg_Exploits_t
{
	bool bInfiniteHealth = false;
	bool bInfiniteAmmo = false;
	bool bNoSpread = false;

	bool bNoClip = false;
	USHORT noClipKey = 0;
	bool bFlyHack = false;
	USHORT flyKey = 0;

	bool bSpeedHack = false;
	float speed = 3000.f;

	bool bSuperJump = false;
	float jumpHeight = 500.f;

	bool bModGravity = false;
	float modGravity = 1.f;

	__forceinline cfg_Exploits_t* operator->() { return this; }
};

struct cfg_Triggerbot_t
{
	bool bEnabled = false;
	USHORT key = 'F';

	bool bDelay = false;
	int delayMode = 0;

	int customDelay = 100; // in ms
	int minRanDelay = 100; // in ms
	int maxRanDelay = 300; // in ms

	__forceinline cfg_Triggerbot_t* operator->() { return this; }
};

struct cfg_Aimbot_t
{
	bool bEnabled = false;
	float smoothness = 1.f;

	bool bVisibleOnly = true;

	bool bFOV = true;
	int FOV = 150;
	bool bFOVCircle = true;

	USHORT aimKey = 'J';

	bool bSilentAim = false;

	__forceinline cfg_Aimbot_t* operator->() { return this; }
};

struct cfg_ESP_t
{
	bool bBoxes = false;
	int boxType = 0;
	bool bSnaplines = false;
	bool bSkeleton = false;

	__forceinline cfg_ESP_t* operator->() { return this; }
};

struct cfg_Menu_t
{
	bool bInit = false;
	bool bShowMenu = true;
	bool bShutdown = false;

	USHORT ToggleKey = VK_F1;

	__forceinline cfg_Menu_t* operator->() { return this; }
};

struct cfg_t
{
	cfg_Menu_t menu;
	cfg_ESP_t esp;
	cfg_Aimbot_t aimbot;
	cfg_Triggerbot_t trg;
	cfg_Exploits_t exploit;

	__forceinline cfg_t* operator->() { return this; }
};
inline cfg_t cfg;
