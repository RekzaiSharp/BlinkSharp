#pragma once
#include "Polygon.h"

namespace Geometry
{
	class Circle : public Polygon
	{
	public:
		Circle(float x, float y, float r);
		Circle(Vector2 c, float r);
		const Vector2& GetCenter() const;
		const float GetRadius() const;
		~Circle();
	private:
		Vector2 m_center;
		float m_radius;
	};
}

