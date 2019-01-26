#include "Circle.h"

using namespace Geometry;

Circle::Circle(float x, float y, float r)
	: m_center(x, y), m_radius(r)
{
	const int segmentN = 22;
	float out_radius = r / cos(2 * M_PI / segmentN);

	for (int i = 1; i <= segmentN; i++)
	{
		float angle = i * 2 * M_PI / segmentN;
		this->AddPoint(Vector2(x + out_radius * cos(angle), y + out_radius * sin(angle)));
	}
}

Circle::Circle(Vector2 c, float r)
	: Circle::Circle(c.x, c.y, r)
{

}

const Vector2& Circle::GetCenter() const
{
	return m_center;
}

const float Circle::GetRadius() const
{
	return m_radius;
}

Circle::~Circle()
{
}
