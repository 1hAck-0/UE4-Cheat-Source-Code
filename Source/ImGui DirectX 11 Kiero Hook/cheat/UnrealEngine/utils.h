#pragma once
#include "UnrealClasses.h"
#include <array>
#include <vector>



namespace mem
{
	inline DWORD GNamesOffset = 0;
	inline uintptr_t TraceLineAddress = 0;

	template<typename T, typename U> constexpr size_t OffsetOf(U T::* member)
	{
		return (char*)&((T*)nullptr->*member) - (char*)nullptr;
	}

	extern __forceinline uintptr_t staticAddress(uintptr_t base, const std::vector<uintptr_t>& offsets);

	extern __forceinline uintptr_t IsValidPointer(uintptr_t base, const std::vector<uintptr_t>& offsets);

	const char* GetGName(const int FNameIndex);

	uintptr_t GetModuleSize(const char* szModule);

	uintptr_t PatternScan(const bool usedIdaStyleMask, std::string idaSignature);

	DWORD GetStaticOffset(byte offsetToRead, byte extra, const bool usedIdaStyleMask, const char* pattern);
}

vec2 CalcAngle(const vec3& src, const vec3& dst, const vec2 currentAngles, float smoothness);

vec3 GetVectorForward(const vec3& angles);

template<const byte actorsToIgnoreCount = 0>
__forceinline bool TraceLine(const vec3& src, const vec3& dst, FHitResult& hitinfo, const std::array<void*, actorsToIgnoreCount>& actorsToIgnore = {}, const int traceChannel = 0)
{
	struct TTArray
	{
		uintptr_t* items = 0;
		int count = actorsToIgnoreCount;
		int max = actorsToIgnoreCount;
	}toIgnore;
	uintptr_t items_t[actorsToIgnoreCount];
	toIgnore.items = items_t;

	for (byte i = 0; i < actorsToIgnoreCount; i++)
		toIgnore.items[i] = (uintptr_t)actorsToIgnore[i];

	bool ret = reinterpret_cast<bool(*)( // function name: UKismetSystemLibrary::LineTraceSingle
		_UWorld * uWorld,
		const vec3 Start,
		const vec3 End,
		int TraceChannel,
		bool bTraceComplex,
		const TTArray & ActorsToIgnore,
		int DrawDebugType,
		FHitResult & hit,
		bool bIgnoreSelf,
		vec4 TraceColor,
		vec4 TraceHitColor,
		float DrawTime)>

		(mem::TraceLineAddress)

		(UWorld,
			src,
			dst,
			traceChannel,
			true,
			toIgnore,
			0,
			hitinfo,
			true,
			{},
			{},
			0.f);

	return ret;
}
