#include "cheat.h"
#include "../includes.h"
#include "../cfg.hpp"
#include "../kiero/minhook/include/MinHook.h"
#include <ctime>



#define IsKeyHeld(key) (GetAsyncKeyState(key) & 0x8000)

#define MAIN_MENU_FNAME_INDEX 19867
#define BONE_HEAD_ID 48
#define BONE_NECK_ID 47
#define BONE_CHEST_ID 4
#define BONE_ROOT_ID 0

/// <summary>
/// All signatures/patterns/AOBs used in this project will work for ALL games using Unreal Engine 4.16.3
/// The cheat should work for almost all Sword With Sauce versions since it uses patterns/signatures
/// </summary>

namespace Cheat
{
	DWORD GWorldOffset = 0;

	__forceinline bool UpdatePointers()
	{
		UWorld = *(_UWorld**)(ModuleBase + GWorldOffset);
		if (!UWorld) return false;

		LocalPlayer = UWorld->GameInstance->LocalPlayers[0];
		CameraManager = LocalPlayer->PlayerController->CameraManager;
		EntityList = &UWorld->PersistentLevel->EntityList;

		return true;
	}

	__forceinline bool WorldToScreen(const vec3& worldLoc, vec2& screenPos)
	{
		return CameraManager->WorldToScreen(worldLoc, screenPos);
	}

	__forceinline bool WorldToScreen(const vec3& worldLoc, vec3& screenPos)
	{
		return CameraManager->WorldToScreen(worldLoc, *(vec2*)&screenPos);
	}

	AEnemy* GetClosestEnemy(vec3& outLocation, int& outId)
	{
		AEnemy* ret = nullptr;
		float closestDist = INFINITY;

		for (USHORT i = 0; i < EntityList->count; i++)
		{
			auto& ent = EntityList->items[i];
			if (!ent->IsPlayer() || (cfg->aimbot->bVisibleOnly && !ent->IsVisible()))
				continue;

			vec3 target = ent->GetBonePos(BONE_HEAD_ID);

			vec2 screenPos;
			if (!WorldToScreen(target, screenPos))
				continue;

			float dist = winCenter.Distance(screenPos);
			if (cfg->aimbot->bFOV && dist > cfg->aimbot->FOV)
				continue;

			if (dist < closestDist)
			{
				closestDist = dist;
				outLocation = target;
				outId = ent->ObjectIndex;
				ret = ent;
			}
		}

		return ret;
	}

	bool bRun = false;
	void DrawESP()
	{
		if (!bRun) return;

		ImDrawList* draw = ImGui::GetBackgroundDrawList();

		// FOV CIRCLE
		if (cfg->aimbot->bEnabled && cfg->aimbot->bFOV && cfg->aimbot->bFOVCircle)
			draw->AddCircle(
				winCenter,									// center
				cfg->aimbot->FOV,							// radius
				ImColor(255, 0, 0),							// color
				cfg->aimbot->FOV / 50 + 30,					// segments
				cfg->aimbot->FOV / 80 + 1.f);				// thickness

		bool BoxOrSnapline = cfg->esp->bBoxes || cfg->esp->bSnaplines;
		if (!BoxOrSnapline && !cfg->esp->bSkeleton)
			return;

		for (USHORT i = 0; i < EntityList->count; i++)
		{
			auto& ent = EntityList->items[i];

			if (!ent->IsPlayer())
				continue;

			// SKELETON ESP
			if (cfg->esp->bSkeleton)
			{
				auto DrawBone = [&](const byte bone1, const byte bone2)
				{
					vec2 screenPos1, screenPos2;

					bool
						b1 = WorldToScreen(ent->GetBonePos(bone1), screenPos1),
						b2 = WorldToScreen(ent->GetBonePos(bone2), screenPos2);

					if (!b1 && !b2)
						return;
					
					draw->AddLine(
						screenPos1,				// p1
						screenPos2,				// p2
						ImColor(255, 0, 0),		// color
						1.f);					// thickness
				};

				auto DrawSkeleton = [&]()
				{
					static constexpr const byte Skeleton_Struct[] = { // differs between player models and games
						// HEAD
						48, 47,

						// BACK
						47, 4,
						4, 3,
						3, 2,
						2, 1,

						// RIGHT ARM
						47, 27,
						27, 28,
						28, 29,
						29, 33,

						// LEFT ARM
						47, 6,
						6, 7,
						7, 8,
						8, 12,

						// RIGHT LEG
						1, 56,
						56, 58,
						58, 59,

						// LEFT LEG
						1, 50,
						50, 52,
						52,	53
					};
					static_assert(IM_ARRAYSIZE(Skeleton_Struct) % 2 == 0);

					for (byte j = 0; j < IM_ARRAYSIZE(Skeleton_Struct) - 1; j += 2)
						DrawBone(Skeleton_Struct[j], Skeleton_Struct[j + 1]);
				};

				DrawSkeleton();
			}

			if (BoxOrSnapline)
			{
				vec3 WHeadPos = ent->GetBonePos(BONE_HEAD_ID);
				WHeadPos.z += 5.f;

				vec2 headPos;
				if (!WorldToScreen(WHeadPos, headPos))
					continue;

				vec3 WFeetPos = ent->GetBonePos(BONE_ROOT_ID);

				vec2 feetPos;
				if (!WorldToScreen(WFeetPos, feetPos))
					continue;

				// SNAPLINE ESP
				if (cfg->esp->bSnaplines)
				{
					draw->AddLine(
						{ winCenter.x, winSize.y - 5 }, // p1
						feetPos,						// p2
						ImColor(255, 0, 0),				// color
						1.f);							// thickness
				}

				// BOX ESP
				if (cfg->esp->bBoxes)
				{
					switch (cfg->esp->boxType)
					{
					// 2D BOX
					case 0:
					{
						float boxWidth = sqrtf((headPos.y - feetPos.y) * (headPos.y - feetPos.y)) / 4;

						draw->AddRect(
							{ feetPos.x - boxWidth, headPos.y }, // tl
							{ feetPos.x + boxWidth, feetPos.y }, // br
							ImColor(255, 0, 0),					 // color
							0, 0,								 // rounding
							1.f);								 // thickness
					}
					break;

					// 3D BOX
					case 1:
					{
						static constexpr const vec3 boxRescaling = { 55.f /*thickness*/, 5.f /*top*/, 5.f /*bottom*/ };

						vec3 b1, b2, b3, b4, t1, t2, t3, t4;
						b1.z = b2.z = b3.z = b4.z = WFeetPos.z - boxRescaling.z;

						float yaw = ent->Rotation.y;
						#define TORAD(x) ((x) * (M_PI / 180.f))

						b1.x = WFeetPos.x + cosf(TORAD(yaw + 45.f)) * boxRescaling.x;
						b1.y = WFeetPos.y + sinf(TORAD(yaw + 45.f)) * boxRescaling.x;

						b2.x = WFeetPos.x + cosf(TORAD(yaw + 135.f)) * boxRescaling.x;
						b2.y = WFeetPos.y + sinf(TORAD(yaw + 135.f)) * boxRescaling.x;

						b3.x = WFeetPos.x + cosf(TORAD(yaw + 225.f)) * boxRescaling.x;
						b3.y = WFeetPos.y + sinf(TORAD(yaw + 225.f)) * boxRescaling.x;

						b4.x = WFeetPos.x + cosf(TORAD(yaw + 315.f)) * boxRescaling.x;
						b4.y = WFeetPos.y + sinf(TORAD(yaw + 315.f)) * boxRescaling.x;

						#undef TORAD

						t1 = b1, t2 = b2, t3 = b3, t4 = b4;
						t1.z = t2.z = t3.z = t4.z = WHeadPos.z + boxRescaling.y;

						bool a = WorldToScreen(b1, b1);
						bool b = WorldToScreen(b2, b2);
						bool c = WorldToScreen(b3, b3);
						bool d = WorldToScreen(b4, b4);

						bool e = WorldToScreen(t1, t1);
						bool f = WorldToScreen(t2, t2);
						bool g = WorldToScreen(t3, t3);
						bool h = WorldToScreen(t4, t4);

						bool bottom = a && b && c && d;
						if (bottom)
						{
							// BOTTOM PART
							draw->AddLine(b1, b2, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b2, b3, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b3, b4, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b4, b1, ImColor(255, 0, 0), 1.f);

							// COLUMNS TO TOP
							draw->AddLine(b1, t1, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b2, t2, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b3, t3, ImColor(255, 0, 0), 1.f);
							draw->AddLine(b4, t4, ImColor(255, 0, 0), 1.f);
						}

						// TOP PART
						bool top = e && f && g && h;
						if (top)
						{
							// TOP PART
							draw->AddLine(t1, t2, ImColor(255, 0, 0), 1.f);
							draw->AddLine(t2, t3, ImColor(255, 0, 0), 1.f);
							draw->AddLine(t3, t4, ImColor(255, 0, 0), 1.f);
							draw->AddLine(t4, t1, ImColor(255, 0, 0), 1.f);

							// COLUMNS TO TOP
							if (!bottom)
							{
								draw->AddLine(t1, b1, ImColor(255, 0, 0), 1.f);
								draw->AddLine(t2, b2, ImColor(255, 0, 0), 1.f);
								draw->AddLine(t3, b3, ImColor(255, 0, 0), 1.f);
								draw->AddLine(t4, b4, ImColor(255, 0, 0), 1.f);
							}
						}
					}
					break;

					// CORNERS
					case 2:
					{
						float dist = sqrtf((headPos.y - feetPos.y) * (headPos.y - feetPos.y));
						float boxWidth = dist / 4;
						float cornerSize = dist / 5;

						vec2 tl = { headPos.x - boxWidth, headPos.y };
						vec2 br = { headPos.x + boxWidth, feetPos.y };

						// TOP LEFT
						draw->AddLine({ tl.x, tl.y }, { tl.x, tl.y + cornerSize }, ImColor(255, 0, 0), 1.f);
						draw->AddLine({ tl.x, tl.y }, { tl.x + cornerSize, tl.y }, ImColor(255, 0, 0), 1.f);

						// TOP RIGHT
						draw->AddLine({ br.x, tl.y }, { br.x, tl.y + cornerSize }, ImColor(255, 0, 0), 1.f);
						draw->AddLine({ br.x, tl.y }, { br.x - cornerSize, tl.y }, ImColor(255, 0, 0), 1.f);

						// BOTTOM RIGHT
						draw->AddLine({ br.x, br.y }, { br.x, br.y - cornerSize }, ImColor(255, 0, 0), 1.f);
						draw->AddLine({ br.x, br.y }, { br.x - cornerSize, br.y }, ImColor(255, 0, 0), 1.f);

						// BOTTOM LEFT
						draw->AddLine({ tl.x, br.y }, { tl.x, br.y - cornerSize }, ImColor(255, 0, 0), 1.f);
						draw->AddLine({ tl.x, br.y }, { tl.x + cornerSize, br.y }, ImColor(255, 0, 0), 1.f);
					}
					break;
					}
				}
			}
		}
	}

	void Aimbot()
	{
		auto AimAtPos = [](const vec3& worldLocation)
		{
			// implementing my velocity, distance to enemy as well as the smoothness in the calculations for a more accurate aimbot
			vec3 src = CameraManager->Location
						+ (LocalPlayer->PlayerController->Character->GetVelocity()
						* CameraManager->Location.Distance(worldLocation)
						/ 49650.f
						* cfg->aimbot->smoothness); 

			vec2 angle = CalcAngle(
				src,							// source world location
				worldLocation,					// target world location
				CameraManager->Rotation,		// current angles/rotation
				cfg->aimbot->smoothness);		// smoothness

			LocalPlayer->PlayerController->ControlRotation = angle;
		};

		static int foundEntId = 0;
		if (!cfg->aimbot->bEnabled || !IsKeyHeld(cfg->aimbot->aimKey))
		{
			foundEntId = 0;
			return;
		}

		// IF ALREADY TARGET FOUND
		if (foundEntId)
		{
			for (USHORT i = 0; i < EntityList->count; i++)
			{
				auto& ent = EntityList->items[i];
				if (ent->IsPlayer() && ent->ObjectIndex == foundEntId)
				{
					if (cfg->aimbot->bVisibleOnly && !ent->IsVisible())
						break;
					AimAtPos(ent->GetBonePos(BONE_HEAD_ID));
					return;
				}
			}
			foundEntId = 0;
		}

		// FIND NEW TARGET
		vec3 closestEnt{};
		if (GetClosestEnemy(closestEnt, foundEntId))
			AimAtPos(closestEnt);
	}

	void Triggerbot(const float deltaTime)
	{
		static bool bShooting = false;
		static bool bDetected = false;
		static float delayCounter = 0.f;

		if (!cfg->trg->bEnabled || !IsKeyHeld(cfg->trg->key))
		{
			if (bShooting)
			{
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				bShooting = false;
			}

			delayCounter = 0.f;
			return;
		}

		if (bDetected)
		{
			if (delayCounter > 0.f)
				delayCounter -= deltaTime * 1000.f; // delta time is in seconds and multiplying it be 1000 will give us ms; delta time is almost always below 1.f because the frame updates a few times per second
			else
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				delayCounter = 0.f;
				bDetected = false;
				bShooting = true;
			}
		}
		else
		{
			vec3 src = CameraManager->Location;
			vec3 dst = src + GetVectorForward(CameraManager->Rotation) * 99999999.f;

			FHitResult hit;
			switch (TraceLine<1>(src, dst, hit, { LocalPlayer->PlayerController->Character })) // using a switch statement so I can break out of the statement, the if statement doesn't have that option
			{
			case true:
				AEnemy* actor = hit.GetActor();
				if (!actor || !actor->HasValidId()) break;

				std::string name = mem::GetGName(actor->FNameIndex);

				auto OnNewEntFound = []()
				{
					if (!bShooting)
					{
						if (cfg->trg->bDelay)
						{
							bDetected = true;
							if (!cfg->trg->delayMode) // custom delay
							{
								delayCounter = float(cfg->trg->customDelay);
							}
							else // random delay
							{
								srand(time(0));
								delayCounter = float(rand() % (cfg->trg->maxRanDelay - cfg->trg->minRanDelay) + cfg->trg->minRanDelay);
							}
						}
						else
						{
							mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
							bShooting = true;
							delayCounter = 0;
						}
					}
					else
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					}
				};

				// FOUND AN ENEMY IN OUR CROSSHAIR
				if (name._Starts_with("EnemyAI_BluePrint"))
				{
					if (actor->EnemyState)
					{
						OnNewEntFound();
						break;
					}
				}

				// FOUND GLASS IN OUR CROSSHAIR
				else if (name._Starts_with("Glass")) // maybe we hit glass
				{
					if (TraceLine<1>(hit.ImpactPoint, dst, hit, { actor }, 1)) // pass in 1 as the trace channel because for whatever reason the Glass actor on the 0 trace channel doesn't get ignored although I pass it as an argument to the function
					{
						actor = hit.GetActor();
						if (actor->IsPlayer())
						{
							OnNewEntFound();
							break;
						}
					}
				}

				// if we didn't find any enemy in our crosshair, we reset
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				bShooting = false;
				delayCounter = 0.f;
			}
		}
	}

	void Exploits()
	{
		auto player = LocalPlayer->PlayerController->Character;

		// INFITE AMMO
		if (cfg->exploit->bInfiniteAmmo)
			player->Ammo = 999;
		// INFINITE HEALTH
		if (cfg->exploit->bInfiniteHealth)
			player->Health = 999999.f;
		// NO SPREAD (not working very well bruh)
		if (cfg->exploit->bNoSpread)
			player->CrosshairExpand = 0.f;

		// NO CLIP
		static bool bResetNoClip = false;
		if (cfg->exploit->bNoClip)
		{
			if (!bResetNoClip) // we need to set the value only once per toggle!
			{
				player->bActorEnableCollision &= ~2; // Actor.bActorEnableCollision.Bit = 2
				bResetNoClip = true;
			}
		}
		else if (bResetNoClip)
		{
			player->bActorEnableCollision |= 2; // Actor.bActorEnableCollision.Bit = 2
			bResetNoClip = false;
		}

		// FLY HACK
		static bool bResetFly = false;
		if (cfg->exploit->bFlyHack)
		{
			player->CharacterMovement->MovementMode = ACharacterMovementComponent::MOVE_Flying; // set to fly mode so we don't fall down

			vec3 sum = { };
			vec3 newRot = { };
			float flySpeed = player->CharacterMovement->MaxFlySpeed / 90.f;

			if (IsKeyHeld(VK_SHIFT))
				flySpeed *= 1.5f; // hold shift to speed fly

			if (IsKeyHeld('W'))
				sum += GetVectorForward(CameraManager->Rotation) * flySpeed;
			else if (IsKeyHeld('S')) // we dont want to move backwards if we are holding W
			{
				newRot = {
					-CameraManager->Rotation.x,		   // when goind backwards, we need to flip the value here
					CameraManager->Rotation.y + 180.f, // + 180.f so we look behind us, 'S' is for going backwards after all...
					0.f								   // roll is always zero
				};
				sum += GetVectorForward(newRot) * flySpeed;
			}

			if (IsKeyHeld('D'))
			{
				newRot = {
					0.f,							  // when going left or right we dont't want to move up or down
					CameraManager->Rotation.y + 90.f, // + 90.f so we look to the right, 'D' is for going right after all...
					0.f								  // roll is always zero
				};
				sum += GetVectorForward(newRot) * flySpeed;
			}
			else if (IsKeyHeld('A')) // we dont want to move left if we are holding A
			{
				newRot = {
					0.f,							   // when going left or right we dont't want to move up or down
					CameraManager->Rotation.y + 270.f, // + 270.f so we look to the left, 'A' is for going left after all...
					0.f								   // roll is always zero
				};
				sum += GetVectorForward(newRot) * flySpeed;
			}

			if (IsKeyHeld(VK_LCONTROL)) // for extra control over our character...
				sum.z -= flySpeed;
			else if (IsKeyHeld(VK_SPACE)) // don't go up if we are holding CTRL
				sum.z += flySpeed;

			player->CapsuleComponent->ModifiableLocation += sum;
			bResetFly = true;
		}
		else if (bResetFly)
		{
			player->CharacterMovement->MovementMode = ACharacterMovementComponent::MOVE_Falling;
			bResetFly = false;
		}

		// SPPEDHACK
		if (cfg->exploit->bSpeedHack)
		{
			player->CharacterMovement->MaxAcceleration = cfg->exploit->speed;
			player->CharacterMovement->MaxWalkSpeed = cfg->exploit->speed;
			player->CharacterMovement->MaxFlySpeed = cfg->exploit->speed;
		}

		// SUPER JUMP
		static bool bResetSuperJump = false;
		if (cfg->exploit->bSuperJump)
		{
			player->CharacterMovement->JumpZVelocity = cfg->exploit->jumpHeight;
			bResetSuperJump = true;
		}
		else if (bResetSuperJump)
		{
			player->CharacterMovement->JumpZVelocity = 420.f; // 420.f is the default in Sword With Sauce
			bResetSuperJump = false;
		}

		// MODIFY GRAVITY
		static bool bResetModGravity = false;
		if (cfg->exploit->bModGravity)
		{
			player->CharacterMovement->GravityScale = cfg->exploit->modGravity;
			bResetModGravity = true;
		}
		else if (bResetModGravity)
		{
			player->CharacterMovement->GravityScale = 1.f;
			bResetModGravity = false;
		}

		// safety check if we are above/under the map
		if (player->CapsuleComponent->ModifiableLocation.z > 10000.f) player->CapsuleComponent->ModifiableLocation.z = 10000.f;
		else if (player->CapsuleComponent->ModifiableLocation.z < 0.f) player->CapsuleComponent->ModifiableLocation.z = 0.f;
	}
		
	void(*oPlayerController_PostProcessInput)(void*, const float, const bool) = nullptr;
	void hRun(void* this_, const float DeltaTime, const bool bGamePaused)
	{
		oPlayerController_PostProcessInput(this_, DeltaTime, bGamePaused);

		if (!UpdatePointers() || UWorld->FNameIndex == MAIN_MENU_FNAME_INDEX)
		{
			bRun = false;
			return;
		}

		bRun = true;
		Aimbot();
		Triggerbot(DeltaTime);
		Exploits();
	}

	__forceinline bool HOOK(const char* pattern, void* pDetour, void* pOriginal, const bool usedIdaStyleMask = true)
	{
		void* pFunc = (void*)mem::PatternScan(usedIdaStyleMask, pattern);
		if (!pFunc) return false;

		MH_CreateHook(pFunc, pDetour, (void**)pOriginal);
		MH_EnableHook(pFunc);
		return true;
	}

	bool Init()
	{
		if (MH_Initialize() != MH_OK)
		{
			MessageBoxA(0, "Couldn't intialize MinHook!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}
		
		ModuleBase = (uintptr_t)GetModuleHandleA(0); // gets the image base of the executable you inject the dll into
		ModuleBaseEnd = ModuleBase + mem::GetModuleSize(0);

		// GET WINDOW SIZE
		const DWORD winSizeOffset = mem::GetStaticOffset(2, 6, false, "39 15 ? ? ? ? 44 8b 44 24");
		if (!winSizeOffset)
		{
			MessageBoxA(0, "Couldn't find the Window Size!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}

		winSize = {
			float(*(int*)(ModuleBase + winSizeOffset)),				 // width
			float(*(int*)(ModuleBase + winSizeOffset + sizeof(int))) // height
		};
		winCenter = winSize / 2;

		// GET NEEDED OFFSETS
		GWorldOffset = mem::GetStaticOffset(3, 7, true, "48 8B 1D ?? ?? ?? ?? 48 8B 75 28 E8 ?? ?? ?? ?? 4C 8B C8 48 8D 4D 28 48 8B D3 4D 8B C7 FF 56 08");
		if (!GWorldOffset)
		{
			MessageBoxA(0, "Couldn't find GWorld!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}

		mem::GNamesOffset = mem::GetStaticOffset(7, 11, true, "48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 50 B9 08 04 00 00 48 89 5C 24 20");
		if (!mem::GNamesOffset)
		{
			MessageBoxA(0, "Couldn't find GNames!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}

		mem::TraceLineAddress = mem::PatternScan(false, "40 53 55 56 57 41 54 41 56 41 57 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24 ? ? ? ? 0f b6 ac 24 ? ? ? ? 48 8b d9 4c 8b b4 24 ? ? ? ? 41 8b c9 4c 8b bc 24 ? ? ? ? 49 8b f0 48 8b fa e8 ? ? ? ? 8b 15 ? ? ? ? 44 8b e0 65 ? 8b 0c 25 ? ? ? ? b8 ? ? ? ? 48 8b 0c ? 8b 14 ? 39 15 ? ? ? ? 0f 8f ? ? ? ? 0f b6 84 24 ? ? ? ? 48 8d 4c 24 ? 48 8b 15 ? ? ? ? 4d 8b ce 48 89 5c 24 ? 44 0f b6 ? 88 44 24 ? e8 ? ? ? ? 48 8b 0d ? ? ? ? 41 b0 ? 48 8b d3 e8 ? ? ? ? 48 8d 0d ? ? ? ? 4c 8b ce 48 89 4c 24 ? 4c 8b c7 48 8d 4c 24 ? 49 8b d7 48 89 4c 24 ? 48 8b c8 44 89 64 24 ? e8 ? ? ? ? 48 8b 8c 24 ? ? ? ? 0f b6 ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 8c 24 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 0f b6 ? 48 8b 8c 24 ? ? ? ? 48 33 cc e8 ? ? ? ? 48 81 c4 ? ? ? ? 41 5f 41 5e 41 5c 5f 5e 5d 5b c3 48 8d 0d ? ? ? ? e8 ? ? ? ? 83 3d ? ? ? ? ? 0f 85 ? ? ? ? 41 b8 ? ? ? ? 48 8d 15 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? e9 ? ? ? ? cc cc cc 4c 89 4c 24 ? 53 55 56 57 41 56 41 57 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24 ? ? ? ? 65 ? 8b 04 25 ? ? ? ? 48 8b d9 8b 0d ? ? ? ? 48 8b fa 0f b6 ac 24 ? ? ? ? 49 8b f0 4c 8b b4 24 ? ? ? ? 4c 8b bc 24 ? ? ? ? 48 8b 04 ? ba ? ? ? ? 8b 0c ? 39 0d ? ? ? ? 0f 8f ? ? ? ? 0f b6 84 24 ? ? ? ? 48 8d 4c 24 ? 48 8b 15 ? ? ? ? 4d 8b ce 48 89 5c 24 ? 44 0f b6 ? 88 44 24 ? e8 ? ? ? ? 48 8b 0d ? ? ? ? 41 b0 ? 48 8b d3 e8 ? ? ? ? 48 8d 4c 24 ? 4c 8b ce 48 89 4c 24 ? 4c 8b c7 48 8b 8c 24 ? ? ? ? 49 8b d7 48 89 4c 24 ? 48 8b c8 e8 ? ? ? ? 48 8b 8c 24 ? ? ? ? 0f b6 ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 4c 24 ? 48 85 c9 74 ? e8 ? ? ? ? 0f b6 ? 48 8b 8c 24 ? ? ? ? 48 33 cc e8 ? ? ? ? 48 81 c4 ? ? ? ? 41 5f 41 5e 5f 5e 5d 5b c3 48 8d 0d ? ? ? ? e8 ? ? ? ? 83 3d ? ? ? ? ? 0f 85 ? ? ? ? 41 b8 ? ? ? ? 48 8d 15 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 40 53 55 56 57 41 54 41 56 41 57 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24 ? ? ? ? 65 ? 8b 04 25 ? ? ? ? 48 8b d9 8b 0d ? ? ? ? 4c 8b f2 0f b6 b4 24 ? ? ? ? 49 8b f9 48 8b ac 24 ? ? ? ? 4d 8b f8 4c 8b a4 24 ? ? ? ? 48 8b 04 ? ba ? ? ? ? 8b 0c ? 39 0d ? ? ? ? 0f 8f ? ? ? ? 0f b6 84 24 ? ? ? ? 48 8d 4c 24 ? 48 8b 15 ? ? ? ? 4c 8b cd 48 89 5c 24 ? 44 0f b6 ? 88 44 24 ? e8 ? ? ? ? 48 8b d7 48 8d 4c 24 ? e8 ? ? ? ? 83 7c 24 ? ? 75 ? 32 db eb ? 48 8b 0d ? ? ? ? 41 b0 ? 48 8b d3 e8 ? ? ? ? 48 8d 4c 24 ? 4d 8b cf 48 89 4c 24 ? 4d 8b c6 48 8d 4c 24 ? 49 8b d4 48 89 4c 24 ? 48 8b c8 e8 ? ? ? ? 0f b6 ? 48 8b 8c 24 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 8c 24 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 0f b6 ? 48 8b 8c 24 ? ? ? ? 48 33 cc e8 ? ? ? ? 48 81 c4 ? ? ? ? 41 5f 41 5e 41 5c 5f 5e 5d 5b c3 48 8d 0d ? ? ? ? e8 ? ? ? ? 83 3d ? ? ? ? ? 0f 85 ? ? ? ? 41 b8 ? ? ? ? 48 8d 15 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 0d ? ? ? ? e8 ? ? ? ? e9 ? ? ? ? cc cc cc cc cc cc cc cc cc cc 40 56");
		if (!mem::TraceLineAddress)
		{
			MessageBoxA(0, "Couldn't find UKismetSystemLibrary::LineTraceSingle!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}

		// HOOK FUNCTIONS

		// APlayerController::PostProcessInput - run the main cheat there
		if (!HOOK(
			"40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 00 07 00 00 84 C0 74 1C F2 0F",
			&Cheat::hRun,
			&oPlayerController_PostProcessInput))
		{
			MessageBoxA(0, "Couldn't find APlayerController::PostProcessInput!", "1hAck Trainer", MB_ICONERROR);
			return false;
		}

		return true;
	}

	void Uninit()
	{
		MH_Uninitialize();
	}
}
