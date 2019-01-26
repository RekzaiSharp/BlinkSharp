#include "RingPrediction.h"
#include "CirclePrediction.h"


RingPrediction::RingPrediction(SPrediction& engine)
	: ISPredictionBase::ISPredictionBase(engine)
{
}

PredictionResult RingPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type,
		Player.GetServerPosition(), Player.GetServerPosition(), target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult RingPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type, from, range_check_from,
		target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult RingPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from);
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	input.path_ = path;
	return GetPrediction(input);
}

PredictionResult RingPrediction::GetPrediction(PredictionInput input)
{
	auto result = CirclePrediction(m_prediction_engine).GetPrediction(input);
	if (result.hit_chance_ > kHitChanceLow)
	{
		Vector2 direction = (result.cast_position_ - Geometry::Helpers::To2D(input.from_) + Geometry::Helpers::To2D(input.target_->GetDirection())).Normalize();
		result.cast_position_ -= direction * (input.spell_.width_ - input.spell_.ring_radius_);

		if (Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.range_check_from_), result.cast_position_) > input.spell_.range_)
		{
			result.hit_chance_ = kHitChanceOutOfRange;
		}
	}

	return result;
}

PredictionAoeResult RingPrediction::GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from)
{
	throw std::exception("not implemented");
}

RingPrediction::~RingPrediction()
{
}
