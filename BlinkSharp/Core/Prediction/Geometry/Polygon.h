#pragma once
#include <vector>
#include "../../../Include/Vectors.hpp"

namespace Geometry
{
	class Polygon
	{
	public:
		Polygon();
		Polygon(std::initializer_list<Vector2> polys);
		template<typename... Args>
		Polygon(const Args&... args)
		{
			for (const Polygon& p : { args... })
			{
				auto& points = p.GetPoints();
				m_points.reserve(points.size());
				m_points.insert(m_points.end(), std::make_move_iterator(points.begin()), std::make_move_iterator(points.end()));
			}
		}
		~Polygon();
		void AddPoint(Vector2 p);
		virtual void Draw();
		const std::vector<Vector2>& GetPoints() const;
	private:
		std::vector<Vector2> m_points;
	};
}

