#include "Arc.h"
#include "Helpers.h"

using namespace Geometry;


Arc::Arc(Vector2 center, Vector2 direction, float angle, float w, float h)
	: m_center(center), m_direction(direction), m_angle(angle), m_width(w), m_height(h)
{
	float angle_step;            // Angle Step (rad)

	if (m_width < m_height)
	{
		if (m_width < 1.0e-4)
			angle_step = 1.0;
		else
			angle_step = asin(2.0 / m_width);
	}
	else
	{
		if (m_height < 1.0e-4)
			angle_step = 1.0;
		else
			angle_step = asin(2.0 / m_height);
	}

	if (angle_step < 0.05)
		angle_step = 0.05;

	Vector2 v(center.x + cos(0) * m_width, center.y - sin(0) * m_height);

	float rot_angle = atan2(direction.y - v.y, direction.x - v.x) - (M_PI * 180.0f / 180.0f);
	for (float a = 0; a <= m_angle; a += angle_step)
	{
		this->AddPoint(Helpers::RotateAroundPoint(Vector2(center.x + cos(a) * m_width, center.y - sin(a) * m_height), v, rot_angle));
	}
}


const Vector2& Arc::GetCenter() const
{
	return m_center;
}

const Vector2& Arc::GetDirection() const
{
	return m_direction;
}

const float Arc::GetAngle() const
{
	return m_angle;
}

const float Arc::GetWidth() const
{
	return m_width;
}

const float Arc::GetHeight() const
{
	return m_height;
}


Arc::~Arc()
{
}
