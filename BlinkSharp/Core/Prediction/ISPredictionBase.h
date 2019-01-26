#pragma once
#include "SDK/PluginData.h"

class SPrediction;
struct PredictionInput;
struct PredictionResult;
struct PredictionAoeResult;

class ISPredictionBase
{
public:
	ISPredictionBase(SPrediction& engine)
		: m_prediction_engine(engine)
	{

	}
	virtual PredictionResult GetPrediction(PredictionInput input) = 0;
	virtual PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type) = 0;
	virtual PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from) = 0;
	virtual PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff) = 0;
	virtual PredictionAoeResult GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from) = 0;
protected:
	SPrediction& m_prediction_engine;
};