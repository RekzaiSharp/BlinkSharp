#pragma once
#include "clipper.hpp"
#include "Polygon.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Sector.h"
#include "Arc.h"

using namespace ClipperLib;

#define MAKE_RECTANGLE(start, end, scale) (Geometry::Rectangle(start, end, scale))
#define MAKE_CIRCLE(center, radius) (Geometry::Circle(center, radius))
#define MAKE_SECTOR(center, direction, angle, radius) (Geometry::Sector(center, direction, angle, radius))
#define MAKE_ARC(center, direction, angle, width, height) (Geometry::Arc(center, direction, angle, width, height))

namespace Geometry
{
	class ClipperWrapper
	{
	public:
		template<typename... Args>
		static bool IsIntersects(const Paths& subj, const Args&... args)
		{
			Clipper c;
			c.AddPaths(subj, ptSubject, true);

			for (const Paths& p : { args... })
			{
				c.AddPaths(p, ptClip, true);
			}

			Paths solution;
			c.Execute(ctIntersection, solution, pftEvenOdd, pftEvenOdd);

			return !solution.empty();
		}
		template<typename... Args>
		static Paths MakePaths(const Args&... args)
		{
			Paths ps;
			ps.reserve(sizeof...(Args));

			for (const Polygon& p : { args... })
			{
				ps.push_back(std::move(GetClipperPath(p)));
			}

			return ps;
		}
		static Path GetClipperPath(const Polygon& val)
		{
			auto& points = val.GetPoints();

			Path result;
			result.reserve(points.size());

			for (auto& point : points)
			{
				result.emplace_back(static_cast<int>(point.x), static_cast<int>(point.y));
			}

			return result;
		}
	};
}

