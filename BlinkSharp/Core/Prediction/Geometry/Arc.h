#pragma once
#include "Polygon.h"

namespace Geometry
{
	class Arc : public Polygon
	{
	public:
		Arc(Vector2 center, Vector2 direction, float angle, float w, float h);
		~Arc();
		const Vector2& GetCenter() const;
		const Vector2& GetDirection() const;
		const float GetWidth() const;
		const float GetHeight() const;
		const float GetAngle() const;
	private:
		Vector2 m_center;
		Vector2 m_direction;
		float m_width;
		float m_height;
		float m_angle;
	};
}

