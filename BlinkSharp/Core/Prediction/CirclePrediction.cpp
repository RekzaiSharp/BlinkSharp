#include "CirclePrediction.h"
#include "LinePrediction.h"
#include "Geometry\ClipperWrapper.hpp"

CirclePrediction::CirclePrediction(SPrediction& engine)
	: ISPredictionBase::ISPredictionBase(engine)
{
}

PredictionResult CirclePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type,
		Player.GetServerPosition(), Player.GetServerPosition(), target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult CirclePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type, from, range_check_from,
		target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult CirclePrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from);
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	input.path_ = path;
	return GetPrediction(input);
}

PredictionResult CirclePrediction::GetPrediction(PredictionInput input)
{
	return LinePrediction(m_prediction_engine).GetPrediction(input);
}

PredictionAoeResult CirclePrediction::GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from)
{
	PredictionAoeResult result(Vector2(0, 0), CollisionResult(), 0);

	auto enemies = pSDK->EntityManager->GetEnemyHeroes();

	std::vector<AIBaseClient*> possible_targets;
	Vector2 pos_sum(0, 0);

	for (auto& enemy : enemies)
	{
		if (enemy.second->IsValid() && Geometry::Helpers::Distance(m_prediction_engine.GetFastUnitPosition(enemy.second, from, delay), Geometry::Helpers::To2D(range_check_from)) < range)
		{
			pos_sum += m_prediction_engine.GetFastUnitPosition(enemy.second, from, delay, missile_speed);
			possible_targets.push_back(enemy.second);
		}
	}
	Vector2 center = pos_sum / possible_targets.size();

	float fly_time = 0.0f;
	if (std::fpclassify(missile_speed) != 0)
	{
		fly_time = Geometry::Helpers::Distance(Geometry::Helpers::To2D(from), center) / missile_speed;
	}

	pos_sum = Vector2(0, 0);

	std::vector<std::pair<PredictionResult, float>> prediction_results;
	for (auto enemy : possible_targets)
	{
		if (enemy->IsValid())
		{
			auto result = GetPrediction(enemy, width, delay, missile_speed, range, false, kCircleCast);
			if (result.hit_chance_ > kHitChanceMedium)
			{
				pos_sum += result.unit_position_;
				prediction_results.emplace_back(std::move(result), result.unit_->GetBoundingRadius());
			}
		}
	}

	if (!prediction_results.empty())
	{
		center = pos_sum / prediction_results.size();
		auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(center, width));
		for (auto& res : prediction_results)
		{
			auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(res.first.unit_position_, res.second));
			if (Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox))
			{
				result.hit_count_++;
			}
		}
	}

	return result;
}

CirclePrediction::~CirclePrediction()
{
}
