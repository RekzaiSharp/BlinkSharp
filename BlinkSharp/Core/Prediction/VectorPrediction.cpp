#include "VectorPrediction.h"
#include "LinePrediction.h"


VectorPrediction::VectorPrediction(SPrediction& engine)
	: ISPredictionBase::ISPredictionBase(engine)
{
}

PredictionResult VectorPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type,
		Player.GetServerPosition(), Player.GetServerPosition(), target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult VectorPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetPrediction(target, width, delay, missile_speed, range, collisionable, type, from, range_check_from,
		target->GetWaypoints(),
		m_prediction_engine.GetPathTrackerEngine().GetAvgMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastMovChangeTime(target),
		m_prediction_engine.GetPathTrackerEngine().GetAvgPathLenght(target),
		m_prediction_engine.GetPathTrackerEngine().GetLastAngleDiff(target));
}

PredictionResult VectorPrediction::GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from);
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	input.path_ = path;
	return GetPrediction(input);
}

PredictionResult VectorPrediction::GetPrediction(PredictionInput input)
{
	throw std::exception("not implemented");
}

PredictionAoeResult VectorPrediction::GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from)
{
	throw std::exception("not implemented");
}

VectorPrediction::~VectorPrediction()
{
}
