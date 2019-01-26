#pragma once
#include "../../../Include/Vectors.hpp"

namespace Geometry
{
	class Helpers
	{
	public:
		static Vector2 Normalized(const Vector2& v);
		static Vector3 Normalized(const Vector3& v);
		static Vector2 Perpendicular(const Vector2& v);
		static Vector2 Rotated(const Vector2& v, float angle);
		static float CrossProduct(const Vector2& v, const Vector2& u);
		static float RadianToDegree(float angle);
		static float DegreeToRadian(float angle);
		static bool Close(float a, float b, float eps);
		static float Polar(const Vector2& v);
		static float AngleBetween(const Vector2& v, const Vector2& u);
		static Vector2 RotateAroundPoint(const Vector2& v, const Vector2& u, float angle);
		static float Distance(const Vector2& v, const Vector2& u, bool squared = false);
		static float Distance(const Vector3& v, const Vector3& u, bool squared = false);
		static bool IsBetween(const Vector2& b, const Vector2& a, const Vector2& c);
		static Vector2 ClosestCirclePoint(const Vector2& center, float radius, const Vector2& point);
		static float PathLenght(const std::vector<Vector2>& path);
		static float PathLenght(const std::vector<Vector3>& path);
		static Vector2 To2D(const Vector3& v);
	};
}
