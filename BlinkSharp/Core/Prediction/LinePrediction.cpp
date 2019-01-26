#include "LinePrediction.h"
#include "Utility.h"
#include "Geometry\Helpers.h"

LinePrediction::LinePrediction(SPrediction& engine)
	: ISPredictionBase::ISPredictionBase(engine)
{

}

PredictionResult LinePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type,
		Player.GetServerPosition(), Player.GetServerPosition(), target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult LinePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type, from, range_check_from,
		target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult LinePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from);
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	input.path_ = path;
	return GetPrediction(input);
}

PredictionResult LinePrediction::GetPrediction(PredictionInput input)
{
	PredictionResult result(input, input.target_, Vector2(0, 0), Vector2(0, 0), kHitChanceImpossible, CollisionResult());

	if (input.spell_.type_ == kCircleCast)
	{
		input.spell_.range_ += input.spell_.width_;
	}

	if (input.path_.size() <= 1 && input.last_mov_change_time_ > 100 && (GetTickCount() - g_EnemyInfo[input.target_->GetNetworkID()].last_aa_tick_ > 300))
	{
		result.hit_chance_ = kHitChanceVeryHigh;
		result.cast_position_ = Geometry::Helpers::To2D(input.target_->GetServerPosition());
		result.unit_position_ = result.cast_position_;

		result.Lock();
		return result;
	}

	if (input.target_->IsHero())
	{
		float end_t = 1.f;
		if (input.target_->IsCasting())
		{
			float arrival_t = m_prediction_engine.GetArrivalTime(Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.from_), Geometry::Helpers::To2D(input.target_->GetServerPosition())), input.spell_.delay_, input.spell_.missile_speed_);
			if (end_t - Game::Time() + input.spell_.width_ * 0.5f / input.target_->GetMovementSpeed() < arrival_t)
			{
				result.hit_chance_ = kHitChanceImpossible;
			}
			else
			{
				result.hit_chance_ = kHitChanceImmobile;
				result.cast_position_ = Geometry::Helpers::To2D(input.target_->GetServerPosition());
				result.unit_position_ = result.cast_position_;

				result.Lock();
			}
			return result;
		}

		//if (GetTickCount() - g_EnemyInfo[input.target_->GetNetworkID()].last_aa_tick_ < 300)
		//{
		//	float arrival_t = m_prediction_engine.GetArrivalTime(Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.from_), Geometry::Helpers::To2D(input.target_->ServerPosition())), input.spell_.delay_, input.spell_.missile_speed_);
		//	if (GSpellData->GetAnimationDelay(input.target_->auto().Data_) + m_prediction_engine.GetPathTrackerEngine().GetAvgOrbwalkTime(input.target_) * 0.001f + input.avg_reaction_time_ * 0.001f - input.spell_.width_ * 0.5f / input.target_->MovementSpeed() >= arrival_t)
		//	{
		//		result.hit_chance_ = kHitChanceHigh;
		//		result.cast_position_ = Geometry::Helpers::To2D(input.target_->ServerPosition());
		//		result.unit_position_ = result.cast_position_;

		//		result.Lock();
		//		return result;
		//	}
		//}

		/*if (input.avg_path_lenght_ < 400 && input.last_mov_change_time_ < 100 && Geometry::Helpers::PathLenght(input.path_) < input.avg_path_lenght_)
		{
			result.hit_chance_ = kHitChanceHigh;
			result.cast_position_ = Geometry::Helpers::To2D(input.path_.back());
			result.unit_position_ = result.cast_position_;

			result.Lock();
			return result;
		}*/
	}

	if (input.target_->IsDashing())
		return m_prediction_engine.GetDashingPrediction(input);

	if (Utility::IsImmobileTarget(input.target_))
		return m_prediction_engine.GetImmobilePrediction(input);

	result = m_prediction_engine.WaypointAnalysis(input);

	/*float d = Geometry::Helpers::Distance(result.cast_position_, Geometry::Helpers::To2D(input.target_->ServerPosition()));
	if (d >= (input.avg_reaction_time_ - input.last_mov_change_time_) * input.target_->MovementSpeed() && d >= input.avg_path_lenght_)
	{
		result.hit_chance_ = kHitChanceMedium;
	}*/

	result.Lock();
	return result;
}

PredictionAoeResult LinePrediction::GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from)
{
	throw std::exception("not implemented");
}

LinePrediction::~LinePrediction()
{
}
