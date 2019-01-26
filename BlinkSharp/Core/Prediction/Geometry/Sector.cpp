#include "Sector.h"
#include "Helpers.h"

using namespace Geometry;

Sector::Sector(Vector2 center, Vector2 direction, float angle, float radius)
	: m_center(center), m_direction(direction), m_angle(angle), m_radius(radius)
{
	const int segmentN = 22;
	float out_radius = radius / cos(2 * M_PI / segmentN);

	this->AddPoint(center);
	auto side = Helpers::Rotated(m_direction, -angle * 0.5f);
	for (int i = 1; i <= segmentN; i++)
	{
		auto direction = Helpers::Rotated(side, i * angle / segmentN).Normalize();
		this->AddPoint(Vector2(center.x + out_radius * direction.x, center.y + out_radius * direction.y));
	}
}

const Vector2& Sector::GetCenter() const
{
	return m_center;
}

const Vector2& Sector::GetDirection() const
{
	return m_direction;
}

const float Sector::GetAngle() const
{
	return m_angle;
}

const float Sector::GetRadius() const
{
	return m_radius;
}

Sector::~Sector()
{
}
