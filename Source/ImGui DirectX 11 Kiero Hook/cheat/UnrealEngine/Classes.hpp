#pragma once
#include <math.h>
#include <corecrt_math_defines.h>
#include "../../includes.h"

struct vec2;
struct vec3;
struct vec4;
struct Matrix;

struct vec2
{
	float x, y;

	__forceinline float Length() const { return sqrtf(x * x + y * y); }
	__forceinline static float Length(const vec2 a) { return sqrtf(a.x * a.x + a.y * a.y); }

	__forceinline float Dot(const vec2 b) const { return x * b.x + y * b.y; }
	__forceinline static float Dot(const vec2 a, const vec2 b) { return a.x * b.x + a.y * b.y; }

	__forceinline float Distance(const vec2 b) const { return sqrtf((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y)); }
	__forceinline static float Distance(const vec2 a, const vec2 b) { return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); }

	__forceinline vec2 GetPitchTo(const vec2 b) const { return { 1.f, (b.y - y) / (b.x - x) }; }
	__forceinline static vec2 GetPitchTo(const vec2 a, const vec2 b) { return { 1.f, (b.y - a.y) / (b.x - a.x) }; }

	// for aimbot
	// NOTE: works only on the specific angle system that Sword With Sauce uses
	__forceinline void Normalize()
	{
		// pitch
		if (x > 90.f)
			x = 90.f;
		else if (x < -90.f)
			x = -90.f;

		// convert to Sword With Sauce's angle system
		if (x < 0.f)
			x += 360.f;
		y = fmodf(y + 360.f, 360.f);
	}

	__forceinline vec2 operator+(const vec2 a) const { return { x + a.x, y + a.y }; }
	__forceinline void operator+=(const vec2 a) { x += a.x, y += a.y; }
	__forceinline vec2 operator-(const vec2 a) const { return { x - a.x, y - a.y }; }
	__forceinline void operator-=(const vec2 a) { x -= a.x, y -= a.y; }
	__forceinline vec2 operator*(const vec2 a) const { return { x * a.x, y * a.y }; }
	__forceinline void operator*=(const vec2 a) { x *= a.x, y *= a.y; }
	__forceinline vec2 operator/(const vec2 a) const { return { x / a.x, y / a.y }; }
	__forceinline void operator/=(const vec2 a) { x /= a.x, y /= a.y; }
	__forceinline vec2 operator+(const float a) const { return { x + a, y + a }; }
	__forceinline void operator+=(const float a) { x += a, y += a; }
	__forceinline vec2 operator-(const float a) const { return { x - a, y - a }; }
	__forceinline void operator-=(const float a) { x -= a, y -= a; }
	__forceinline vec2 operator*(const float a) const { return { x * a, y * a }; }
	__forceinline void operator*=(const float a) { x *= a, y *= a; }
	__forceinline vec2 operator/(const float a) const { return { x / a, y / a }; }
	__forceinline void operator/=(const float a) { x /= a, y /= a; }

	__forceinline bool operator==(const vec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const vec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const ImVec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const ImVec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const float a) const { return x == a && y == a; }
	__forceinline bool operator!=(const float a) const { return x != a || y != a; }

	__forceinline bool operator>(const vec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const vec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const vec2 a) const { return x < a.x && y < a.y; }
	__forceinline bool operator<=(const vec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const ImVec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const ImVec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const ImVec2 a) const { return x < a.x&& y < a.y; }
	__forceinline bool operator<=(const ImVec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const float a) const { return x > a && y > a; }
	__forceinline bool operator>=(const float a) const { return x >= a && y >= a; }
	__forceinline bool operator<(const float a) const { return x < a && y < a; }
	__forceinline bool operator<=(const float a) const { return x <= a && y <= a; }

	__forceinline operator vec3() const;
	__forceinline operator vec4() const;
	__forceinline operator ImVec2() const { return ImVec2(x, y); }
};

struct vec3
{
	float x, y, z;

	__forceinline float Length() const { return sqrtf(x * x + y * y + z * z); }
	__forceinline static float Length(const vec3& a) { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); }

	__forceinline float Dot(const vec3& b) const { return x * b.x + y * b.y + z * b.z; }
	__forceinline static float Dot(const vec3& a, const vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

	__forceinline float Distance(const vec3& b) const { return sqrtf((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y) + (z - b.z) * (z - b.z)); }
	__forceinline static float Distance(const vec3& a, const vec3& b) { return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z)); }

	__forceinline vec3 GetPitchTo(const vec3& b) const { return { 1.f, (b.y - y) / (b.x - x), (b.z - z) / (b.x - x) }; }
	__forceinline static vec3 GetPitchTo(const vec3& a, const vec3& b) { return { 1.f, (b.y - a.y) / (b.x - a.x), (b.z - a.z) / (b.x - a.x)  }; }

	__forceinline vec3 operator+(const vec3& a) const { return { x + a.x, y + a.y, z + a.z }; }
	__forceinline void operator+=(const vec3& a) { x += a.x, y += a.y, z += a.z;  };
	__forceinline vec3 operator-(const vec3& a) const { return { x - a.x, y - a.y, z - a.z }; }
	__forceinline void operator-=(const vec3& a) { x -= a.x, y -= a.y, z -= a.z; }
	__forceinline vec3 operator*(const vec3& a) const { return { x * a.x, y * a.y, z * a.z }; }
	__forceinline void operator*=(const vec3& a) { x *= a.x, y *= a.y, z *= a.z; }
	__forceinline vec3 operator/(const vec3& a) const { return { x / a.x, y / a.y, z / a.z }; }
	__forceinline void operator/=(const vec3& a) { x /= a.x, y /= a.y, z /= a.z; }
	__forceinline vec3 operator+(const float a) const { return { x + a, y + a, z + a }; }
	__forceinline void operator+=(const float a) { x += a, y += a, z += a; }
	__forceinline vec3 operator-(const float a) const { return { x - a, y - a, z - a }; }
	__forceinline void operator-=(const float a){ x -= a, y -= a, z -= a; }
	__forceinline vec3 operator*(const float a) const { return { x * a, y * a, z * a }; }
	__forceinline void operator*=(const float a) { x *= a, y *= a, z *= a; }
	__forceinline vec3 operator/(const float a) const { return { x / a, y / a, z / a }; }
	__forceinline void operator/=(const float a) { x /= a, y /= a, z /= a; }

	__forceinline bool operator==(const vec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const vec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const vec3& a) const { return x == a.x && y == a.y && z == a.z; }
	__forceinline bool operator!=(const vec3& a) const { return x != a.x || y != a.y || z != a.z; }
	__forceinline bool operator==(const ImVec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const ImVec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const float a) const { return x == a && y == a && z == a; }
	__forceinline bool operator!=(const float a) const { return x != a || y != a || z != a; }

	__forceinline bool operator>(const vec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const vec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const vec2 a) const { return x < a.x&& y < a.y; }
	__forceinline bool operator<=(const vec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const vec3& a) const { return x > a.x && y > a.y && z > a.z; }
	__forceinline bool operator>=(const vec3& a) const { return x >= a.x && y >= a.y && z >= a.z; }
	__forceinline bool operator<(const vec3& a) const { return x < a.x&& y < a.y && z < a.z; }
	__forceinline bool operator<=(const vec3& a) const { return x <= a.x && y <= a.y && z <= a.z; }
	__forceinline bool operator>(const ImVec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const ImVec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const ImVec2 a) const { return x < a.x&& y < a.y; }
	__forceinline bool operator<=(const ImVec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const float a) const { return x > a && y > a; }
	__forceinline bool operator>=(const float a) const { return x >= a && y >= a; }
	__forceinline bool operator<(const float a) const { return x < a&& y < a; }
	__forceinline bool operator<=(const float a) const { return x <= a && y <= a; }

	__forceinline operator vec2() const { return { x, y }; }
	__forceinline operator vec4() const;
	__forceinline operator ImVec2() const { return ImVec2(x, y); }
};

__forceinline vec2::operator vec3() const { return { x, y, 0.f }; }

struct vec4
{
	float x, y, z, w;

	__forceinline float Length() const { return sqrtf(x * x + y * y + z * z + w * w); }
	__forceinline static float Length(const vec4& a) { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w); }

	__forceinline float Dot(const vec4& b) const { return x * b.x + y * b.y + z * b.z + w * b.w; }
	__forceinline static float Dot(const vec4& a, const vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

	__forceinline float Distance(const vec4& b) const { return sqrtf((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y) + (z - b.z) * (z - b.z) + (w - b.w) * (w - b.w)); }
	__forceinline static float Distance(const vec4& a, const vec4& b) { return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) + (a.w - b.w) * (a.w - b.w)); }

	__forceinline vec4 GetPitchTo(const vec4& b) const { return { 1.f, (b.y - y) / (b.x - x), (b.z - z) / (b.x - x), (b.w - w) / (b.x - x) }; }
	__forceinline static vec4 GetPitchTo(const vec4& a, const vec4& b) { return { 1.f, (b.y - a.y) / (b.x - a.x), (b.z - a.z) / (b.x - a.x), (b.w - a.w) / (b.x - a.x) }; }

	__forceinline vec4 operator+(const vec4& a) const { return { x + a.x, y + a.y, z + a.z, w + a.w }; }
	__forceinline void operator+=(const vec4& a) { x += a.x, y += a.y, z += a.z, w += a.w; }
	__forceinline vec4 operator-(const vec4& a) const { return { x - a.x, y - a.y, z - a.z, w - a.w }; }
	__forceinline void operator-=(const vec4& a) { x -= a.x, y -= a.y, z -= a.z, w -= a.w; }
	__forceinline vec4 operator*(const vec4& a) const { return { x * a.x, y * a.y, z * a.z, w * a.w }; }
	__forceinline void operator*=(const vec4& a) { x *= a.x, y *= a.y, z *= a.z, w *= a.w; }
	__forceinline vec4 operator/(const vec4& a) const { return { x / a.x, y / a.y, z / a.z, w / a.w }; }
	__forceinline void operator/=(const vec4& a) { x /= a.x, y /= a.y, z /= a.z, w /= a.w; }
	__forceinline vec4 operator+(const float a) const { return { x + a, y + a, z + a, w + a }; }
	__forceinline void operator+=(const float a) { x += a, y += a, z += a, w += a; }
	__forceinline vec4 operator-(const float a) const { return { x - a, y - a, z - a, w - a }; }
	__forceinline void operator-=(const float a) { x -= a, y -= a, z -= a, w -= a; }
	__forceinline vec4 operator*(const float a) const { return { x * a, y * a, z * a, w * a }; }
	__forceinline void operator*=(const float a) { x *= a, y *= a, z *= a, w *= a; }
	__forceinline vec4 operator/(const float a) const { return { x / a, y / a, z / a, w / a }; }
	__forceinline void operator/=(const float a) { x /= a, y /= a, z /= a, w /= a; }

	__forceinline bool operator==(const vec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const vec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const vec3& a) const { return x == a.x && y == a.y && z == a.z; }
	__forceinline bool operator!=(const vec3& a) const { return x != a.x || y != a.y || z != a.z; }
	__forceinline bool operator==(const vec4& a) const { return x == a.x && y == a.y && z == a.z && w == a.w; }
	__forceinline bool operator!=(const vec4& a) const { return x != a.x || y != a.y || z != a.z || w == a.w; }
	__forceinline bool operator==(const ImVec2 a) const { return x == a.x && y == a.y; }
	__forceinline bool operator!=(const ImVec2 a) const { return x != a.x || y != a.y; }
	__forceinline bool operator==(const float a) const { return x == a && y == a && z == a && w == a; }
	__forceinline bool operator!=(const float a) const { return x != a || y != a || z != a || w != a; }

	__forceinline bool operator>(const vec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const vec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const vec2 a) const { return x < a.x&& y < a.y; }
	__forceinline bool operator<=(const vec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const vec3& a) const { return x > a.x && y > a.y && z > a.z; }
	__forceinline bool operator>=(const vec3& a) const { return x >= a.x && y >= a.y && z >= a.z; }
	__forceinline bool operator<(const vec3& a) const { return x < a.x&& y < a.y&& z < a.z; }
	__forceinline bool operator<=(const vec3& a) const { return x <= a.x && y <= a.y && z <= a.z; }
	__forceinline bool operator>(const vec4& a) const { return x > a.x && y > a.y && z > a.z && w > a.w; }
	__forceinline bool operator>=(const vec4& a) const { return x >= a.x && y >= a.y && z >= a.z && w >= a.w; }
	__forceinline bool operator<(const vec4& a) const { return x < a.x&& y < a.y&& z < a.z && z < a.w; }
	__forceinline bool operator<=(const vec4& a) const { return x <= a.x && y <= a.y && z <= a.z && w < a.w; }
	__forceinline bool operator>(const ImVec2 a) const { return x > a.x && y > a.y; }
	__forceinline bool operator>=(const ImVec2 a) const { return x >= a.x && y >= a.y; }
	__forceinline bool operator<(const ImVec2 a) const { return x < a.x&& y < a.y; }
	__forceinline bool operator<=(const ImVec2 a) const { return x <= a.x && y <= a.y; }
	__forceinline bool operator>(const float a) const { return x > a && y > a; }
	__forceinline bool operator>=(const float a) const { return x >= a && y >= a; }
	__forceinline bool operator<(const float a) const { return x < a&& y < a; }
	__forceinline bool operator<=(const float a) const { return x <= a && y <= a; }

	__forceinline operator vec2() const { return { x, y }; }
	__forceinline operator vec3() const { return { x, y, z }; }
	__forceinline operator ImVec2() const { return ImVec2(x, y); }
};

__forceinline vec2::operator vec4() const { return { x, y, 0.f, 0.f }; }
__forceinline vec3::operator vec4() const { return { x, y, z, 0.f }; }

struct Matrix
{
	float m[4][4];

	Matrix() { ZeroMemory(this, sizeof(Matrix)); }
	Matrix(const vec3& rotation, const vec3& origin = { 0.f, 0.f, 0.f })
	{
		float radPitch = rotation.x * M_PI / 180.f;
		float radYaw = rotation.y * M_PI / 180.f;
		float radRoll = rotation.z * M_PI / 180.f;

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		m[0][0] = CP * CY;
		m[0][1] = CP * SY;
		m[0][2] = SP;
		m[0][3] = 0.f;
		m[1][0] = SR * SP * CY - CR * SY;
		m[1][1] = SR * SP * SY + CR * CY;
		m[1][2] = -SR * CP;
		m[1][3] = 0.f;
		m[2][0] = -(CR * SP * CY + SR * SY);
		m[2][1] = CY * SR - CR * SP * SY;
		m[2][2] = CR * CP;
		m[2][3] = 0.f;
		m[3][0] = origin.x;
		m[3][1] = origin.y;
		m[3][2] = origin.z;
		m[3][3] = 1.f;
	}

	Matrix operator*(const Matrix& m2) const
	{
		Matrix out;
		for (byte r = 0; r < 4; r++)
		{
			for (byte c = 0; c < 4; c++)
			{
				float sum = 0.f;

				for (byte i = 0; i < 4; i++)
					sum += m[r][i] * m2.m[i][c];

				out.m[r][c] = sum;
			}
		}

		return out;
	}
	__forceinline void operator*=(const Matrix& m2) { *this = this->operator*(m2); }
};
