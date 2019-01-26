#include "Rectangle.h"
#include "Helpers.h"

using namespace Geometry;

Rectangle::Rectangle(Vector2 start, Vector2 end, float width)
	: m_start(start), m_end(end), m_width(width)
{
	m_direction = (end - start).Normalize();
	m_perpendicular = Helpers::Perpendicular(m_direction);

	this->AddPoint(m_start + m_width * m_perpendicular);
	this->AddPoint(m_start - m_width * m_perpendicular);
	this->AddPoint(m_end - m_width * m_perpendicular);
	this->AddPoint(m_end + m_width * m_perpendicular);
}

const Vector2& Rectangle::GetStartPoint() const
{
	return m_start;
}

const Vector2& Rectangle::GetEndPoint() const
{
	return m_end;
}

const float Rectangle::GetWidth() const
{
	return m_width;
}

Rectangle::~Rectangle()
{
}
