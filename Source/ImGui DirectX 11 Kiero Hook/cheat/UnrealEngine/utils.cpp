#include "utils.h"
#include <Psapi.h>



namespace mem
{
	__forceinline uintptr_t staticAddress(uintptr_t base, const std::vector<uintptr_t>& offsets)
	{
		for (const uintptr_t& offset : offsets)
			base = *(uintptr_t*)base + offset;
		return base;
	}

	template <typename T>
	__forceinline T readSafe(uintptr_t address)
	{
		T buffer;
		static const HANDLE hProcess = GetCurrentProcess();
		ReadProcessMemory(hProcess, (void*)address, &buffer, sizeof(T), nullptr);
		return buffer;
	}

	// sometimes some pointers will be nullptrs and by using this function we can check if so without crashing our game
	__forceinline uintptr_t IsValidPointer(uintptr_t base, const std::vector<uintptr_t>& offsets)
	{
		DWORD oldProtect;
		for (const uintptr_t& offset : offsets)
		{
			if (VirtualProtect((void*)base, sizeof(base), PAGE_EXECUTE_READWRITE, &oldProtect))
				VirtualProtect((void*)base, sizeof(base), oldProtect, &oldProtect);
			else
				return 0;
			base = readSafe<uintptr_t>(base) + offset;
		}
		return base;
	}

	const char* GetGName(const int FNameIndex)
	{
		#define SIZE_GNAME_CHUNK 0x4000

		static const uintptr_t GNames = *(uintptr_t*)(ModuleBase + GNamesOffset);
		if (!GNames || !IsValidPointer(GNames + (FNameIndex / SIZE_GNAME_CHUNK) * sizeof(void*), { 0x0 }))
			return "";

		uintptr_t chunk = *(uintptr_t*)(GNames + (FNameIndex / SIZE_GNAME_CHUNK) * sizeof(void*));
		if (!chunk || !IsValidPointer(chunk + (sizeof(void*) * (FNameIndex % SIZE_GNAME_CHUNK)), { 0x0 }))
			return "";

		uintptr_t nameptr = *(uintptr_t*)(chunk + (sizeof(void*) * (FNameIndex % SIZE_GNAME_CHUNK)));
		if (!nameptr)
			return "";

		return (char*)(nameptr + 0x10);
	}

	uintptr_t GetModuleSize(const char* szModule)
	{
		if (HMODULE hModule = GetModuleHandleA(szModule))
		{
			MODULEINFO mInfo = { 0 };
			K32GetModuleInformation(GetCurrentProcess(), hModule, &mInfo, sizeof(MODULEINFO));
			return (uintptr_t)mInfo.SizeOfImage;
		}

		return 0;
	}

	uintptr_t PatternScan(const bool usedIdaStyleMask, std::string idaSignature)
	{
		auto idaSigToBytesArray = [&idaSignature, &usedIdaStyleMask](byte*& out_pattern, char*& out_mask, USHORT& out_len)->void
		{
			idaSignature.erase(remove(idaSignature.begin(), idaSignature.end(), ' '), idaSignature.end());

			out_len = idaSignature.length();
			out_pattern = new byte[out_len];
			out_mask = new char[out_len];

			USHORT realLen = 0;
			for (USHORT i = 0; i < out_len - 1; i++)
			{
				if (idaSignature[i] == '?')
				{
					out_pattern[realLen] = 0;
					out_mask[realLen] = '?';
					if (usedIdaStyleMask)
						i++;
					realLen++;
					continue;
				}

				char buf[2] = { idaSignature[i], idaSignature[i + 1] };
				sscanf_s(buf, "%02hhx", &out_pattern[realLen]);
				out_mask[realLen] = 'x';
				realLen++;
				i++;
			}

			out_len = realLen;
		};

		byte* pattern;
		char* mask;
		USHORT len;
		idaSigToBytesArray(pattern, mask, len);

		MEMORY_BASIC_INFORMATION mbi{};

		for (char* curr = (char*)ModuleBase; curr < (char*)(ModuleBaseEnd - len); curr += mbi.RegionSize)
		{
			if (!VirtualQuery(curr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) ||
				mbi.State != MEM_COMMIT ||
				mbi.Protect == PAGE_NOACCESS)
				continue;

			for (DWORD i = 0; i < mbi.RegionSize; i++)
			{
				bool bFound = true;
				for (DWORD j = 0; j < len; j++)
				{
					if (mask[j] != '?' && pattern[j] != *(byte*)(curr + i + j))
					{
						bFound = false;
						break;
					}
				}

				if (bFound)
				{
					delete[] pattern;
					delete[] mask;
					return (uintptr_t)(curr + i);
				}
			}
		}

		delete[] pattern;
		delete[] mask;
		return 0;
	}

	DWORD GetStaticOffset(byte offsetToRead, byte instructionLenTotal, const bool usedIdaStyleMask, const char* pattern)
	{
		uintptr_t pOffset = PatternScan(usedIdaStyleMask, pattern);
		if (!pOffset) return 0;

		DWORD oldProtect;
		if (VirtualProtect((void*)pOffset, sizeof(DWORD), PAGE_EXECUTE_READ, &oldProtect))
		{
			DWORD ret = (*(DWORD*)(pOffset + offsetToRead) + instructionLenTotal) + (pOffset - ModuleBase); // the first thing in brackets is the relative offset from the instruction, the second thing in brackets gives me the instruction offset, we need to add those numbers together to get the actual offset
			VirtualProtect((void*)pOffset, sizeof(DWORD), oldProtect, &oldProtect);
			return ret;
		}
		return 0;
	}
}

__forceinline float SmoothOutYaw(float targetYaw, float currentYaw, float smoothness)
{
	if (targetYaw > 0.f && currentYaw < 0.f)
	{
		float dist = 180.f - targetYaw + 180.f + currentYaw;
		if (dist < 180.f)
			return currentYaw - dist / smoothness;
	}
	else if (currentYaw > 0.f && targetYaw < 0.f)
	{
		float dist = 180.f - currentYaw + 180.f + targetYaw;
		if (dist < 180.f)
			return currentYaw + dist / smoothness;
	}

	return currentYaw + (targetYaw - currentYaw) / smoothness;
}

vec2 CalcAngle(const vec3& src, const vec3& dst, const vec2 currentAngles, float smoothness)
{
	vec3 delta = dst - src;

	float dist = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	float pitch = (-((acosf((delta.z / dist)) * 180.0f / M_PI) - 90.0f));
	float yaw = atan2f(delta.y, delta.x) * (180.0f / M_PI);

	vec2 ret;
	ret.x = currentAngles.x + (pitch - currentAngles.x) / smoothness;
	ret.y = SmoothOutYaw(yaw, currentAngles.y, smoothness);
	ret.Normalize(); // makes the angles work for Sword With Sauce's angle system

	return ret;
}

vec3 GetVectorForward(const vec3& angles)
{
	float sp, sy, cp, cy;
	float angle;

	angle = angles.y * (M_PI / 180.0f);
	sy = sinf(angle);
	cy = cosf(angle);
	angle = -angles.x * (M_PI / 180.0f);
	sp = sinf(angle);
	cp = cosf(angle);

	return { cp * cy, cp * sy, -sp };
}
