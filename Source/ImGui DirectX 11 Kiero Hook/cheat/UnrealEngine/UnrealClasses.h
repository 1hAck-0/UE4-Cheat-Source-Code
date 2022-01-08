#pragma once
#include "Classes.hpp"
#include <string>
#include <vector>



namespace mem
{
	uintptr_t IsValidPointer(uintptr_t base, const std::vector<uintptr_t>& offsets);

	const char* GetGName(const int FNameIndex);
}

inline uintptr_t ModuleBase;
inline uintptr_t ModuleBaseEnd;
inline vec2 winSize;
inline vec2 winCenter;

struct UtilClass
{
#define DEFINE_MEMBER_N(type, name, offset) struct { char __pad__##name[offset]; type name; }
#define DEFINE_MEMBER_N0(type, name) struct { type name; }

	template<typename T> T read(const USHORT& offset) { return *(T*)((uintptr_t)this + offset); }
	template<typename T> void write(const USHORT& offset, const T& value) { return *(T*)((uintptr_t)this + offset) = value; }
};

template <typename T>
struct TArray
{
	inline T& operator[](const int i) { return items[i]; }

	inline const T& operator[](const int i) const { return items[i]; }

	__forceinline static TArray<T> Init(const std::vector<T>& items)
	{
		TArray<T> ret;
		ret.count = ret.max = items.size();
		ret.items = new T[ret.count];
		for (int i = 0; i < ret.count; i++)
			ret.items[i] = items[i];
		return ret;
	}

	__forceinline void Uninit()
	{
		delete[] this->items;
	}

	T* items = nullptr;
	int count = 0;
	int max = 0;
};

struct FString
{
	inline wchar_t* GetWString() { return this->str; }

	inline char* GetString()
	{
		std::wstring buf(this->str);
		char ret[256];
		strcpy_s(ret, std::string(buf.begin(), buf.end()).c_str());
		return ret;
	}

private:
	wchar_t* str;
	int count;
	int max;
};

struct FTransform
{
	vec4 Rotation;
	vec3 Translation;
	int _pad1;
	vec3 Scale;
	int _pad2;

	Matrix ToMatrixWithScale() const
	{
		Matrix m;
		m.m[3][0] = Translation.x;
		m.m[3][1] = Translation.y;
		m.m[3][2] = Translation.z;

		float x2 = Rotation.x + Rotation.x;
		float y2 = Rotation.y + Rotation.y;
		float z2 = Rotation.z + Rotation.z;

		float xx2 = Rotation.x * x2;
		float yy2 = Rotation.y * y2;
		float zz2 = Rotation.z * z2;
		m.m[0][0] = (1.0f - (yy2 + zz2)) * Scale.x;
		m.m[1][1] = (1.0f - (xx2 + zz2)) * Scale.y;
		m.m[2][2] = (1.0f - (xx2 + yy2)) * Scale.z;

		float yz2 = Rotation.y * z2;
		float wx2 = Rotation.w * x2;
		m.m[2][1] = (yz2 - wx2) * Scale.z;
		m.m[1][2] = (yz2 + wx2) * Scale.y;

		float xy2 = Rotation.x * y2;
		float wz2 = Rotation.w * z2;
		m.m[1][0] = (xy2 - wz2) * Scale.y;
		m.m[0][1] = (xy2 + wz2) * Scale.x;

		float xz2 = Rotation.x * z2;
		float wy2 = Rotation.w * y2;
		m.m[2][0] = (xz2 + wy2) * Scale.z;
		m.m[0][2] = (xz2 - wy2) * Scale.x;

		m.m[0][3] = 0.0f;
		m.m[1][3] = 0.0f;
		m.m[2][3] = 0.0f;
		m.m[3][3] = 1.0f;

		return m;
	}
}; // size: 0x30

struct FName
{
	union
	{
		struct
		{
			int32_t	ComparisonIndex;
			//int32_t DisplayIndex;
			uint32_t Number;
		};
		//uint64_t CompositeComparisonValue;
	};
};

struct TWeakObjectPtr
{
	int ObjectIndex;
	int ObjectSerialNumber;
};

struct FHitResult
{
	uint32_t bBlockingHit : 1;
	uint32_t bStartPenetrating : 1;
	float Time;
	float Distance;
	vec3 Location;
	vec3 ImpactPoint;
	vec3 Normal;
	vec3 ImpactNormal;
	vec3 TraceStart;
	vec3 TraceEnd;
	float PenetrationDepth;
	int32_t Item;
	void* PhysMaterial; // TWeakObjectPtr<class UPhysicalMaterial>
	int ActorID;
	int ActorSerialNumber;
	void* Component; // TWeakObjectPtr<class UPrimitiveComponent>
	FName BoneName;
	int32_t FaceIndex;

	struct AEnemy* GetActor();
};

enum EStereoscopicPass
{
	eSSP_FULL,
	eSSP_LEFT_EYE,
	eSSP_RIGHT_EYE,
	eSSP_MONOSCOPIC_EYE
};

struct FMinimalViewInfo
{
	vec3 Location;
	vec3 Rotation;
	float FOV;
	float OrthoWidth;
	float OrthoNearClipPlane;
	float OrthoFarClipPlane;
	float AspectRatio;
	uint32_t bConstrainAspectRatio : 1;
	uint32_t bUseFieldOfViewForLOD : 1;
	byte ProjectionMode;
	float PostProcessBlendWeight;
	//struct FPostProcessSettings PostProcessSettings; // FPostProcessSettings is a huge ass structure so I didn't bother remaking it
	//vec2 OffCenterProjectionOffset;
};

struct UVTabel
{
	inline uintptr_t operator[](const int index) { return items[index]; }

	inline uintptr_t offset(const int offset) { return *(uintptr_t*)((uintptr_t)this + offset); }

	uintptr_t items[256];
};

#define UObject_d \
DEFINE_MEMBER_N0(UVTabel*, VTable);\
DEFINE_MEMBER_N(int, ObjectIndex, 0xC);\
DEFINE_MEMBER_N(void*, Type, 0x10);\
DEFINE_MEMBER_N(int, FNameIndex, 0x18);

struct UObject
{
	union
	{
		UObject_d;
	};
};

struct USceneComponent
{
	union
	{
		UObject_d;
	};
};

struct ACapsuleComponent
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(vec3, ModifiableLocation, 0x280);
		DEFINE_MEMBER_N(const vec3, RelativeLocation, 0x2C0);
	};
};

struct ARootComponent
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(vec3, Velocity, 0x338);
	};
};

struct ACharacterMovementComponent
{
	enum EMovementMode : byte
	{
		MOVE_None,
		MOVE_Walking,
		MOVE_NavWalking,
		MOVE_Falling,
		MOVE_Swimming,
		MOVE_Flying,
		MOVE_Custom,
		MOVE_MAX
	};

	union
	{
		UObject_d;
		DEFINE_MEMBER_N(float, GravityScale, 0x274);
		DEFINE_MEMBER_N(float, MaxJumpHeight, 0x278);
		DEFINE_MEMBER_N(float, JumpZVelocity, 0x27C);
		DEFINE_MEMBER_N(EMovementMode, MovementMode, 0x28C);
		DEFINE_MEMBER_N(byte, CustomMovementMode, 0x28D);
		DEFINE_MEMBER_N(float, MaxWalkSpeed, 0x2B4);
		DEFINE_MEMBER_N(float, MaxWalkSpeedCrouched, 0x2B8);
		DEFINE_MEMBER_N(float, MaxSwimSpeed, 0x2BC);
		DEFINE_MEMBER_N(float, MaxFlySpeed, 0x2C0);
		DEFINE_MEMBER_N(float, MaxCustomMovementSpeed, 0x2C4);
		DEFINE_MEMBER_N(float, MaxAcceleration, 0x2C8);
		DEFINE_MEMBER_N(float, Mass, 0x328);
		DEFINE_MEMBER_N(byte, bCheatFlying, 0x4A5);
	};
};

struct PlayerBP_C // local player class
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(byte, bActorEnableCollision, 0x86);
		DEFINE_MEMBER_N(struct ARootComponent*, RootComp, 0x168);
		DEFINE_MEMBER_N(struct ACharacterMovementComponent*, CharacterMovement, 0x3F0);
		DEFINE_MEMBER_N(struct ACapsuleComponent*, CapsuleComponent, 0x3F8);
		DEFINE_MEMBER_N(float, CrosshairExpand, 0x8CC);
		DEFINE_MEMBER_N(int, Ammo, 0x8D8);
		DEFINE_MEMBER_N(float, Health, 0x92C);
		DEFINE_MEMBER_N(byte, Crouched, 0x930);
		DEFINE_MEMBER_N(float, MaxMovementSpeed, 0x934);
		DEFINE_MEMBER_N(byte, Dead, 0x95C);
	};

	vec3 GetVelocity() const;
};

struct APlayerState
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(FString, PlayerName, 0x390);
	};
};

struct USkeletalMeshComponent
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(FTransform, CompToWorld, 0x270);
		DEFINE_MEMBER_N(TArray<FTransform>, CachedBoneArray, 0x920);
	};

	vec3 GetBonePos(const byte& boneIndex) const
	{
		Matrix matrix =
			this->CachedBoneArray[boneIndex].ToMatrixWithScale() * this->CompToWorld.ToMatrixWithScale();

		return { matrix.m[3][0], matrix.m[3][1], matrix.m[3][2] };
	}
};

struct AEnemy : UtilClass // actual class name: EnemyAI_BluePrintV2_C
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(struct ARootComponent*, RootComp, 0x168);
		DEFINE_MEMBER_N(struct USkeletalMeshComponent*, Mesh, 0x3E8);
		DEFINE_MEMBER_N(float, BulletRotation, 0x800);
		DEFINE_MEMBER_N(float, BulletDamage, 0x80C);
		DEFINE_MEMBER_N(byte, EnemyState, 0x839);
		DEFINE_MEMBER_N(byte, bCanSeePlayer, 0x892);
		DEFINE_MEMBER_N(byte, bInRange, 0x893);
		DEFINE_MEMBER_N(byte, CanSee, 0x9E8);
		DEFINE_MEMBER_N(vec3, Rotation, 0x41C);
		DEFINE_MEMBER_N(struct ULocalPlayer*, Player, 0x850);
	};

	__forceinline vec3 GetBonePos(const byte& boneIndex) const
	{
		return this->Mesh->GetBonePos(boneIndex);
	}

	vec3 GetVelocity() const;

	bool HasValidId() const
	{
		return mem::IsValidPointer((uintptr_t)this + 0x18, { 0x0 }); // 0x18 = offset of FNameIndex
	}

	bool IsDead() const
	{
		return !this->EnemyState;
	}

	bool IsPlayer() const
	{
		return
			this->HasValidId() &&																		// is it a invalid pointer
			std::string(mem::GetGName(this->FNameIndex))._Starts_with("EnemyAI_BluePrint") &&			// is enemy type
			this->EnemyState;																			// is alive
	}

	bool IsVisible() const;
};

struct APlayerCameraManager
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(vec3, Location, 0x410);
		DEFINE_MEMBER_N(vec3, Rotation, 0x41C);
		DEFINE_MEMBER_N(float, FOV, 0x428);
	};

	bool WorldToScreen(const vec3& worldLoc, vec2& screenPos) const
	{
		Matrix tempMatrix = this->Rotation;

		vec3 vDelta = worldLoc - this->Location;

		vec3 vTransformed = {
			vDelta.Dot({ tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2] }),
			vDelta.Dot({ tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2] }),
			vDelta.Dot({ tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2] })
		};

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		screenPos.x = winCenter.x + vTransformed.x * (winCenter.x / tanf(this->FOV * M_PI / 360)) / vTransformed.z;
		screenPos.y = winCenter.y - vTransformed.y * (winCenter.x / tanf(this->FOV * M_PI / 360)) / vTransformed.z;

		if (screenPos.x > winSize.x || screenPos.x < 0.f || screenPos.y > winSize.y || screenPos.y < 0.f)
			return false;

		return true;
	}
};

struct APlayerController
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(struct USceneComponent*, ActorComponent, 0x388);
		DEFINE_MEMBER_N(struct PlayerBP_C*, Character, 0x3A0);
		DEFINE_MEMBER_N(struct APlayerState*, PlayerState, 0x3A8);
		DEFINE_MEMBER_N(vec2, ControlRotation, 0x3B8);
		DEFINE_MEMBER_N(struct APlayerCameraManager*, CameraManager, 0x420);
		DEFINE_MEMBER_N(vec2, RotationVel, 0x588);
	};
};

struct ULocalPlayer
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(struct APlayerController*, PlayerController, 0x30);
		DEFINE_MEMBER_N(struct UGameViewportClient*, ViewportClient, 0x58);
	};
};

struct UGameInstance
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(TArray<struct ULocalPlayer*>, LocalPlayers, 0x38);
	};
};

struct ULevel
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(TArray<struct AEnemy*>, EntityList, 0xA0);
	};
};

struct _UWorld
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(TArray<struct ULevel*>, Levels, 0x110);
		DEFINE_MEMBER_N(struct ULevel*, PersistentLevel, 0x30);
		DEFINE_MEMBER_N(struct ULevel*, CurrentLevel, 0x138);
		DEFINE_MEMBER_N(struct UGameInstance*, GameInstance, 0x140);
	};
};

struct UGameViewportClient
{
	union
	{
		UObject_d;
		DEFINE_MEMBER_N(struct _UWorld*, World, 0x80);
		DEFINE_MEMBER_N(struct UGameInstance*, GameInstance, 0x88);
	};
};

inline struct _UWorld* UWorld;
inline struct ULocalPlayer* LocalPlayer;
inline struct TArray<struct AEnemy*>* EntityList;
inline struct APlayerCameraManager* CameraManager;

#undef UObject_d
#undef DEFINE_MEMBER_N
