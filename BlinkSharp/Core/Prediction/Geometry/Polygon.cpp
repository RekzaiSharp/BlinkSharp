#include "Polygon.h"

using namespace Geometry;

Polygon::Polygon()
{

}

Polygon::Polygon(std::initializer_list<Vector2> polys)
	: m_points(polys)
{

}

void Polygon::AddPoint(Vector2 p)
{
	m_points.push_back(p);
}

void Polygon::Draw()
{
	int i = 0;
	for (auto& p : m_points)
	{
		auto nextIdx = (m_points.size() - 1 == i) ? 0 : (i + 1);
	//	GRender->DrawLine(p, m_points.at(nextIdx), Vec4(255, 255, 255, 0));
	}
}

const std::vector<Vector2>& Polygon::GetPoints() const
{
	return m_points;
}

Polygon::~Polygon()
{
}
