#pragma once

#pragma once

#include <vector>

#include "SDK Extensions.h"
#include "clipper.hpp"

using namespace ClipperLib;

namespace Geometry {
	class Polygon {
	public:
		std::vector<Vector2> Points;

		Polygon() {};
		Polygon(std::vector<Polygon>& poly) {
			this->Points.reserve(poly.size());
			for (auto polygon : poly) {
				for (auto point : polygon.Points) {
					this->Points.push_back(point);
				}
			}
		}
		Polygon(Path& clipperPath) {
			this->Points.reserve(clipperPath.size());
			for (auto point : clipperPath) {				
				this->Points.push_back({(float)point.X, (float)point.Y});
			}
		}

		inline void Add(Vector2 point) {
			Points.push_back(point);
		}

		inline void Add(Polygon poly) {
			this->Points.reserve(poly.Points.size());
			for (auto point : poly.Points) {
				this->Points.push_back(point);
			}
		}

		inline void Draw(int width = 1, PSDKCOLOR Color = &Color::White) {
			for (size_t i = 0; i < Points.size(); i++) {
				auto nextIndex = (Points.size() - 1 == i) ? 0 : (i + 1);
				auto start = Points[i].To3D(Points[i].GetTerrainHeight());
				auto end = Points[nextIndex].To3D(Points[nextIndex].GetTerrainHeight());
				Draw::Line(&start, &end, (float)width, Color);
			}
		};

		/// Creates Paths from polygon for Clipper
		inline Path ToClipperPath() {
			Path result;
			result.reserve(this->Points.size());

			for (auto point : this->Points)
				result.emplace_back((ClipperLib::cInt) point.x, (ClipperLib::cInt) point.y);

			return result;
		}

		virtual inline bool IsOutside(Vector2& point) {
			IntPoint p((ClipperLib::cInt)point.x, (ClipperLib::cInt)point.y);
			return PointInPolygon(p, this->ToClipperPath()) != 1;
		}

		virtual inline bool IsOutside(Vector3& point) {
			IntPoint p((ClipperLib::cInt)point.x, (ClipperLib::cInt)point.z);
			return PointInPolygon(p, this->ToClipperPath()) != 1;
		}

		inline bool IsInside(Vector2 point, float extraRadius = 0.0f);

		inline bool IsInside(Polygon& poly) {
			Vector2 center{};
			for (auto pos : poly.Points) {
				if (!IsOutside(pos)) {
					return true;
				}
				center += pos;
			}
			center.x /= poly.Points.size();
			center.y /= poly.Points.size();
			return !IsOutside(center);
		}		

		inline bool IsInside(Vector3& point, float extraRadius = 0.0f) {
			return IsInside(point.To2D(), extraRadius);
		}

		inline bool IsInside(GameObject* obj, float extraRadius = 0.0f) {
			return IsInside(obj->GetPosition().To2D(), obj->GetBoundingRadius() + extraRadius);
		}

		inline bool Intersects(Polygon& other);
	};

	class Circle : public Polygon {
		static const int CircleLineSegmentN = 22;
		int Quality;
	public:
		Vector2 Center;
		float Radius;

		Circle(Vector3 center, float radius, int quality = 20)
			: Circle(center.To2D(), radius, quality) {
		}

		Circle(Vector2 center, float radius, int quality = 20) {
			Center = center;
			Radius = radius;
			Quality = quality;
			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0, float overrideWidth = -1) {
			Points.clear();
			auto outRadius = (overrideWidth > 0 ? overrideWidth	: (offset + Radius) / (float)std::cos(2 * M_PI / Quality));
			for (auto i = 1; i <= Quality; i++)	{
				auto angle = i * 2 * M_PI / Quality;				
				Points.emplace_back(Center.x + outRadius * (float)std::cos(angle), Center.y + outRadius * (float)std::sin(angle));
			}
		}

		inline bool IsOutside(Vector2& point) final {			
			return Center.Distance(point) > Radius;
		}

		inline bool IsOutside(Vector3& point) final {
			return Center.Distance(point) > Radius;
		}
	};

	class Rectangle : public Polygon {
	public:
		Vector2 End;
		Vector2 Start;
		float Width;

		Rectangle(Vector3 start, Vector3 end, float width)
			: Rectangle(start.To2D(), end.To2D(), width) {
		}

		Rectangle(Vector2 start, Vector2 end, float width) {
			Start = start;
			End = end;
			Width = width;
			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0, float overrideWidth = -1) {
			Points.clear();
			auto _width{ (overrideWidth > 0 ? overrideWidth : Width + (float)offset) };
			auto dir{ (float)offset * Direction() };
			auto perp{ _width * Perpendicular() };

			Points.emplace_back(Start + perp - dir);
			Points.emplace_back(Start - perp - dir);
			Points.emplace_back(End   - perp + dir);
			Points.emplace_back(End   + perp + dir);
		}

		Vector2 Direction() {
			return (End - Start).Normalized();
		}

		Vector2 Perpendicular() {
			return Direction().Perpendicular();
		}

		inline bool IsOutside(Vector2& point) final {
			auto projection{ point.ProjectOn(Start, End) };
			return (!projection.IsOnSegment || projection.SegmentPoint.Distance(point) > Width);
		}

		inline bool IsOutside(Vector3& point) final {
			auto projection{ point.To2D().ProjectOn(Start, End) };
			return (!projection.IsOnSegment || projection.SegmentPoint.Distance(point) > Width);
		}		
	};

	class Ring : public Polygon {
		int Quality;
	public:
		Vector2 Center;
		float InnerRadius;
		float OuterRadius;

		Ring(Vector3 center, float innerRadius, float outerRadius, int quality = 20)
			: Ring(center.To2D(), innerRadius, outerRadius, quality) {
		}

		Ring(Vector2 center, float innerRadius, float outerRadius, int quality = 20) {
			Center = center;
			InnerRadius = innerRadius;
			OuterRadius = outerRadius;
			Quality = quality;
			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0) {
			Points.clear();
			auto outRadius = (OuterRadius + offset) / (float)std::cos(2 * M_PI / Quality);
			auto innerRadius = (InnerRadius - offset);
			for (int i = 0; i <= Quality; i++) {
				auto angle = i * 2.0 * M_PI / Quality;
				Points.emplace_back(Center.x - outRadius * (float)std::cos(angle), Center.y - outRadius * (float)std::sin(angle));
			}
			for (int i = 0; i <= Quality; i++) {
				auto angle = i * 2.0 * M_PI / Quality;
				Points.emplace_back(Center.x - innerRadius * (float)std::cos(angle), Center.y - innerRadius * (float)std::sin(angle));
			}
		}

		inline bool IsOutside(Vector2& point) final {
			return Center.Distance(point) > OuterRadius || Center.Distance(point) < InnerRadius;
		}

		inline bool IsOutside(Vector3& point) final {
			return Center.Distance(point) > OuterRadius || Center.Distance(point) < InnerRadius;
		}
	};

	class Line : public Polygon {
	public:
		Vector2 LineStart;
		Vector2 LineEnd;

		float Length() {
			return LineStart.Distance(LineEnd);
		}

		void SetLength(float value) {
			LineEnd = (LineEnd - LineStart).Normalized() * value + LineStart;
		}

		Line(Vector3 start, Vector3 end, float length = -1) : Line(start.To2D(), end.To2D(), length) {}

		Line(Vector2 start, Vector2 end, float length = -1) {
			LineStart = start;
			LineEnd = end;
			if (length > 0) {
				SetLength(length);
			}
			UpdatePolygon();
		}

		void UpdatePolygon() {
			Points.clear();
			Points.push_back(LineStart);
			Points.push_back(LineEnd);
		}

		inline bool IsOutside(Vector2& point) final {
			auto projection{ point.ProjectOn(LineStart, LineEnd) };
			return (!projection.IsOnSegment || projection.SegmentPoint.Distance(point) > 1.0f);
		}

		inline bool IsOutside(Vector3& point) final {
			auto projection{ point.To2D().ProjectOn(LineStart, LineEnd) };
			return (!projection.IsOnSegment || projection.SegmentPoint.Distance(point) > 1.0f);
		}
	};

	class Sector :public Polygon {
		static const int CircleLineSegmentN = 22;
		int Quality;
	public:
		float Angle;
		Vector2 Center;
		Vector2 Direction;
		float Radius;

		Sector(Vector3 center, Vector3 direction, float angle, float radius, int quality = 20)
			: Sector(center.To2D(), direction.To2D(), angle, radius, quality) {
		}

		Sector(Vector2 center, Vector2 direction, float angle, float radius, int quality = 20) {
			Center = center;
			Direction = (direction - center).Normalized();
			Angle = angle;
			Radius = radius;
			Quality = quality;
			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0) {
			Points.clear();
			auto outRadius{ (Radius + offset) / (float)std::cos(2 * M_PI / Quality) };
			Points.push_back(Center);
			auto side1{ Direction.Rotated(-Angle * 0.5f) };
			for (int i = 0; i <= Quality; i++) {
				auto cDirection{ side1.Rotated(i * Angle / Quality).Normalized() };
				Points.emplace_back(Center.x + outRadius * cDirection.x, Center.y + outRadius * cDirection.y);
			}
		}
	};

	class Arc : public Polygon {
		int Quality;
	public:
		Vector2 StartPos;
		Vector2 EndPos;
		float Radius;
		float Angle;

		Arc(Vector3 start, Vector3 direction, float angle, float radius, int quality = 20)
			: Arc(start.To2D(), direction.To2D(), angle, radius, quality) {
		}

		Arc(Vector2 start, Vector2 direction, float angle, float radius, int quality = 20) {
			StartPos = start;
			EndPos = (direction - start).Normalized();
			Angle = angle;
			Radius = radius;
			Quality = quality;
			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0) {
			Points.clear();
			auto outRadius = (Radius + offset) / (float)std::cos(2 * M_PI / Quality);
			auto side1 = EndPos.Rotated(-Angle * 0.5f);
			for (int i = 0; i <= Quality; i++) {
				auto cDirection = side1.Rotated(i * Angle / Quality).Normalized();
				Points.emplace_back(StartPos.x + outRadius * cDirection.x, StartPos.y + outRadius * cDirection.y);
			}
		}
	};

	/// Gets position with time
	inline Vector2 PositionAfter(std::vector<Vector2>& Path, float t, float speed, float delay = 0.0f) {
		auto distance = std::max<float>(0.0f, t - delay) * speed;

		for (size_t i = 1; i < Path.size(); i++) {
			auto a = Path[i - 1];
			auto b = Path[i];
			auto d = a.Distance(b);

			if (d < distance) {
				distance -= d;
			}
			else {
				return a + distance * (b - a).Normalized();
			}
		}
		return Path.size() > 0 ? Path[Path.size() - 1] : Vector2();
	}

	/// Gets closest point on circle
	inline Vector2 ClosestCirclePoint(Vector2* center, float radius, Vector2* point) {
		Vector2 v = (*point - *center).Normalized();
		return *center + v * radius;
	}

	inline Vector2 Deviation(Vector2* point1, Vector2* point2, double angle) {
		angle *= M_PI / 180.0;
		Vector2 temp = *point2 - *point1;
		Vector2 result = Vector2(0.0f, 0.0f);
		result.x = (float)(temp.x * std::cos(angle) - temp.y * std::sin(angle)) / 4;
		result.y = (float)(temp.x * std::sin(angle) + temp.y * std::cos(angle)) / 4;
		result = result + *point1;
		return result;
	}
}

inline bool Geometry::Polygon::IsInside(Vector2 point, float extraRadius) {
	if (extraRadius < 10.0f) {
		return !IsOutside(point);
	}

	Geometry::Circle poly(point, extraRadius, 6);
	return IsInside(poly);
}


namespace ClipperWrapper {
	/// Checks if polygons are intersecting
	inline static bool IsIntersects(Paths p1, Paths p2) {
		Clipper c{};
		Paths solution{};
		c.AddPaths(p1, PolyType::ptSubject, true);
		c.AddPaths(p2, PolyType::ptClip, true);

		c.Execute(ClipType::ctIntersection, solution, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd);

		return solution.size() != 0;
	}

	inline static bool IsIntersects(Paths p1, std::vector<Paths> p2) {
		Clipper c{};
		Paths solution{};
		c.AddPaths(p1, PolyType::ptSubject, true);

		for (size_t i = 0; i < p2.size(); i++)
			c.AddPaths(p2[i], PolyType::ptClip, true);

		c.Execute(ClipType::ctIntersection, solution, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd);

		return solution.size() != 0;
	}

	inline static std::vector<std::vector<IntPoint>> ClipPolygons(std::vector<Geometry::Polygon> polygons) {

		std::vector<std::vector<IntPoint>> subj = {};
		std::vector<std::vector<IntPoint>> clip = {};

		for (auto& polygon : polygons) {
			subj.push_back(polygon.ToClipperPath());
			clip.push_back(polygon.ToClipperPath());
		}

		std::vector<std::vector<IntPoint>> solution{};
		Clipper c{};
		c.AddPaths(subj, PolyType::ptSubject, true);
		c.AddPaths(clip, PolyType::ptClip, true);
		c.Execute(ClipType::ctUnion, solution, PolyFillType::pftPositive, PolyFillType::pftEvenOdd);
		return solution;
	}

	inline static std::vector<std::vector<IntPoint>> ClipPolygons(std::vector<Geometry::Polygon*> polygons) {

		std::vector<std::vector<IntPoint>> subj = {};
		std::vector<std::vector<IntPoint>> clip = {};

		for (auto& polygon : polygons) {
			subj.push_back(polygon->ToClipperPath());
			clip.push_back(polygon->ToClipperPath());
		}

		std::vector<std::vector<IntPoint>> solution{};
		Clipper c{};
		c.AddPaths(subj, PolyType::ptSubject, true);
		c.AddPaths(clip, PolyType::ptClip, true);
		c.Execute(ClipType::ctUnion, solution, PolyFillType::pftPositive, PolyFillType::pftEvenOdd);
		return solution;
	}

	/// Defines Line Polygon
	inline static Geometry::Polygon DefineLine(Vector2 start, Vector2 end, float scale) {
		return Geometry::Line(start, end, scale);
	}

	/// Defines Rectangle Polygon
	inline static Geometry::Polygon DefineRectangle(Vector2 start, Vector2 end, float scale) {
		return Geometry::Rectangle(start, end, scale);
	}

	/// Defines Circle Polygon
	inline static Geometry::Polygon DefineCircle(Vector2 c, float r) {
		return Geometry::Circle(c, r);
	}

	/// Defines Ring Polygon
	inline static Geometry::Polygon DefineRing(Vector2 c, float r1, float r2) {
		return Geometry::Ring(c, r1, r2);
	}

	/// Defines Sector Polygon
	inline static Geometry::Polygon DefineSector(Vector2 center, Vector2 direction, float angle, float radius) {
		return Geometry::Sector(center, direction, angle, radius);
	}

	/// Defines Arc Polygon
	inline static Geometry::Polygon DefineArc(Vector2 start, Vector2 direction, float angle, float radius, int quality = 20) {
		return Geometry::Arc(start, direction, angle, radius, quality);
	}

	//I felt like killing myself after this so if you use it give credits to RMAN thx
	inline static Geometry::Polygon DefineDianaArc(Vector2 sourcePosition, Vector2 cursorPos, float width, Vector3 misPos = {0,0,0}) {
		//Find center of gyration
		double totalDist{ cursorPos.Distance(sourcePosition) };
		double d = totalDist * 0.5 * (1 - std::pow(cursorPos.Length(), 2)) / std::pow(cursorPos.Length(), 2);
		double c1 = ((cursorPos.y - sourcePosition.y) / totalDist) * d + (sourcePosition.x + cursorPos.x) / 2;
		double c2 = ((sourcePosition.x - cursorPos.x) / totalDist) * d + (sourcePosition.y + cursorPos.y) / 2;
		Vector2 centerPoint{ (float)c1, (float)c2 };

		//Get Angles Between CoG, Start and End pos
		float angleToEnd{ std::atan2(sourcePosition.y - centerPoint.y, sourcePosition.x - centerPoint.x) };
		float angleToPlayer{ std::atan2(cursorPos.y - centerPoint.y, cursorPos.x - centerPoint.x) };
		double angleBetween{ std::fmod((double)angleToEnd - (double)angleToPlayer + 5.0 * M_PI, M_PI * 2.0) - M_PI };

		//Arc Data
		Geometry::Polygon arc{};
		float radius = centerPoint.Distance(sourcePosition) - width * 0.5f;
		auto innerRadius = (radius) / (float)std::cos(2 * M_PI / 20.0f);
		auto outerRadius = (radius + width) / (float)std::cos(2 * M_PI / 20.0f);
		auto midPos{ ((sourcePosition + cursorPos)*0.5f) };
		auto dir{ (midPos - centerPoint) };

		std::vector<Vector2> backwardTrip{};
		auto side1 = dir.Rotated(-(float)angleBetween * 0.5f);
		for (int i = 0; i <= 20.0f; i++) {
			auto cDirection = side1.Rotated((float)i * (float)angleBetween / 20.0f).Normalized();

			Vector2 tmpPos{ centerPoint.x + innerRadius * cDirection.x, centerPoint.y + innerRadius * cDirection.y };
			Vector2 extendedTmp{ centerPoint.Extended(tmpPos, radius) };

			//Cuts the polygon right behind the missile
			if (misPos.IsValid() && extendedTmp.Distance(cursorPos) > misPos.Distance(cursorPos)) { continue; }
			arc.Add(extendedTmp);

			Vector2 tmpPos2{ centerPoint.x + outerRadius * cDirection.x, centerPoint.y + outerRadius * cDirection.y };
			Vector2 extendedTmp2{ centerPoint.Extended(tmpPos2, radius + width) };
			backwardTrip.push_back(extendedTmp2);
		}

		for (auto it = backwardTrip.rbegin(); it < backwardTrip.rend(); ++it) { arc.Add(*it); }
		return arc;
	}

	/// Creates Paths from Polygon for Clipper
	inline static Paths MakePaths(Geometry::Polygon& plist) {
		return Paths{ plist.ToClipperPath() };
	}
	inline static Paths MakePaths(Geometry::Polygon* plist) {
		return Paths{ plist->ToClipperPath() };
	}

	/// Creates Paths from Polygon list for Clipper
	inline static Paths MakePaths(std::vector<Geometry::Polygon>& plist) {
		Paths ps;
		ps.reserve(plist.size());
		for (size_t i = 0; i < plist.size(); i++)
			ps.push_back(plist[i].ToClipperPath());
		return ps;
	}
	inline static Paths MakePaths(std::vector<Geometry::Polygon*>& plist) {
		Paths ps;
		ps.reserve(plist.size());
		for (size_t i = 0; i < plist.size(); i++) 		
			ps.push_back(plist[i]->ToClipperPath());
					
		return ps;
	}
}

inline bool Geometry::Polygon::Intersects(Geometry::Polygon& other) {
	auto p1{ ClipperWrapper::MakePaths(this) };
	auto p2{ ClipperWrapper::MakePaths(other) };
	return ClipperWrapper::IsIntersects(p1, p2);
}
