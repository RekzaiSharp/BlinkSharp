#pragma once
#include "Polygon.h"

namespace Geometry
{
	class Rectangle : public Polygon
	{
	public:
		Rectangle(Vector2 start, Vector2 end, float width);
		~Rectangle();
		const Vector2& GetStartPoint() const;
		const Vector2& GetEndPoint() const;
		const float GetWidth() const;
	private:
		Vector2 m_direction;
		Vector2 m_perpendicular;
		Vector2 m_start;
		Vector2 m_end;
		float m_width;
	};
}
