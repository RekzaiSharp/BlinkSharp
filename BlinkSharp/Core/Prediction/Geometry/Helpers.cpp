#include <algorithm>
#include "Helpers.h"

using namespace Geometry;

Vector2 Helpers::Normalized(const Vector2& v)
{
	Vector2 res = v;
	res.Normalize();
	return res;
}

Vector3 Helpers::Normalized(const Vector3& v)
{
	Vector3 res = v;
	res.Normalize();
	return res;
}

Vector2 Helpers::Perpendicular(const Vector2& v)
{
	return Vector2(-v.y, v.x);
}

Vector2 Helpers::Rotated(const Vector2& v, float angle)
{
	return Vector2(v.x * cos(angle) - v.y * sin(angle), v.y * cos(angle) + v.x * sin(angle));
}

float Helpers::CrossProduct(const Vector2& v, const Vector2& u)
{
	return u.y * v.x + u.x * v.y;
}

float Helpers::RadianToDegree(float angle)
{
	return angle * (180.0f / M_PI);
}

float Helpers::DegreeToRadian(float angle)
{
	return M_PI * angle / 180.0f;
}

bool Helpers::Close(float a, float b, float eps)
{
	if (abs(eps) < std::numeric_limits<float>::epsilon())
	{
		eps = 1e-9;
	}
	return abs(a - b) <= eps;
}

float Helpers::Polar(const Vector2& v)
{
	if (Close(v.x, 0, 0))
	{
		if (v.y > 0)
		{
			return 90;
		}
		return v.y < 0 ? 270 : 0;
	}

	auto theta = RadianToDegree(atan2(v.y, v.x));
	if (v.x < 0)
	{
		theta += 180;
	}
	if (theta < 0)
	{
		theta += 360;
	}
	return theta;
}

float Helpers::AngleBetween(const Vector2& v, const Vector2& u)
{
	auto theta = Polar(v) - Polar(u);
	if (theta < 0)
	{
		return theta + 360;
	}
	if (theta > 180)
	{
		return 360 - theta;
	}
	return theta;
}

Vector2 Helpers::RotateAroundPoint(const Vector2& v, const Vector2& u, float angle)
{
	auto s = sin(angle);
	auto c = cos(angle);

	auto x = c * (v.x - u.x) - s * (v.y - u.x) + u.x;
	auto y = s * (v.x - u.x) + c * (v.y - u.y) + u.y;

	return Vector2(x, y);
}

float Helpers::Distance(const Vector2& v, const Vector2& u, bool squared)
{
	float distance = sqrt(pow(u.x - v.x, 2) + pow(v.x - u.x, 2));
	return distance;
}

float Helpers::Distance(const Vector3& v, const Vector3& u, bool squared)
{
	float distance_square = pow(v.x - u.x, 2) + pow(v.y - u.y, 2) + pow(v.z - u.z, 2);
	if (squared)
	{
		return distance_square;
	}
	return sqrt(distance_square);
}

bool Helpers::IsBetween(const Vector2& b, const Vector2& a, const Vector2& c)
{
	return Distance(a, c) + Distance(c, b) - Distance(a, b) < std::numeric_limits<float>::epsilon();
}

Vector2 Helpers::ClosestCirclePoint(const Vector2& center, float radius, const Vector2& point)
{
	Vector2 v = (point - center).Normalize();
	return center + v * radius;
}

float Helpers::PathLenght(const std::vector<Vector2>& path)
{
	float distance = 0.0f;
	int i = 0;
	std::for_each(path.begin(), std::prev(path.end()), [&](const Vector2& v)
	{
		distance += Distance(v, path.at(i + 1));
		i++;
	});
	return distance;
}

float Helpers::PathLenght(const std::vector<Vector3>& path)
{
	float distance = 0.0f;
	int i = 0;
	std::for_each(path.begin(), std::prev(path.end()), [&](const Vector3& v)
	{
		distance += Distance(Geometry::Helpers::To2D(v), Geometry::Helpers::To2D(path.at(i + 1)));
		i++;
	});
	return distance;
}

Vector2 Helpers::To2D(const Vector3& v)
{
	return Vector2(v.x, v.z);
}