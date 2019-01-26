#pragma once
#include "ISPredictionBase.h"
#include "SPrediction.h"

class VectorPrediction : public ISPredictionBase
{
public:
	VectorPrediction(SPrediction& engine);
	PredictionResult GetPrediction(PredictionInput input) override;
	PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type) override;
	PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from) override;
	PredictionResult GetPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from, const std::vector<Vector3>& path, float avgt, float movt, float avgp, float anglediff) override;
	PredictionAoeResult GetAoePrediction(float width, float delay, float missile_speed, float range, const Vector3& from, const Vector3& range_check_from) override;
	~VectorPrediction();
};

