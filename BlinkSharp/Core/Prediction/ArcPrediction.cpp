#include "ArcPrediction.h"
#include "ISPredictionBase.h"
#include "LinePrediction.h"
#include "CirclePrediction.h"


ArcPrediction::ArcPrediction(SPrediction& engine)
	: ISPredictionBase::ISPredictionBase(engine)
{
}

PredictionResult ArcPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type,
		Player.GetServerPosition(), Player.GetServerPosition(), target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult ArcPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type, from, range_check_from,
		target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult ArcPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from);
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	input.path_ = path;
	return GetPrediction(input);
}

PredictionResult ArcPrediction::GetPrediction(PredictionInput input)
{
	if (Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.target_->GetServerPosition()), Geometry::Helpers::To2D(input.from_)) > input.spell_.range_ * 0.75f)
	{
		return CirclePrediction(m_prediction_engine).GetPrediction(input);
	}

	auto result = LinePrediction(m_prediction_engine).GetPrediction(input);
	if (result.hit_chance_ >= kHitChanceLow)
	{
		result.cast_position_ = (Geometry::Helpers::To2D(input.from_) + (result.cast_position_ - Geometry::Helpers::To2D(input.from_).Normalize() * input.spell_.range_));
		float c = cos((1 - Geometry::Helpers::Distance(result.unit_position_, Geometry::Helpers::To2D(input.from_)) / input.spell_.range_) * M_PI / 2);
		float s = sin((1 - Geometry::Helpers::Distance(result.unit_position_, Geometry::Helpers::To2D(input.from_)) / input.spell_.range_) * M_PI / 2);
		float x = c * (result.cast_position_.x - input.from_.x) - s * (result.cast_position_.y - input.from_.y) + input.from_.x;
		float y = s * (result.cast_position_.y - input.from_.x) + c * (result.cast_position_.y - input.from_.y) + input.from_.y;
		result.cast_position_ = Vector2(x, y);
	}

	return result;
}

PredictionAoeResult ArcPrediction::GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from)
{
	throw std::exception("not implemented");
}

ArcPrediction::~ArcPrediction()
{
}
