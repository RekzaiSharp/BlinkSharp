#pragma once
#include "Polygon.h"

namespace Geometry
{
	class Sector : public Polygon
	{
	public:
		Sector(Vector2 center, Vector2 direction, float angle, float radius);
		~Sector();
		const float GetAngle() const;
		const Vector2& GetCenter() const;
		const Vector2& GetDirection() const;
		const float GetRadius() const;
	private:
		float m_angle;
		Vector2 m_center;
		Vector2 m_direction;
		float m_radius;
	};
}

