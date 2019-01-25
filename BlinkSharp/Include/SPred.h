#pragma once
#include "SDK Extensions.h"

class SPred
{
public:
	auto GetDirectionDifferencePerc(Vector3 dir1, Vector3 dir2)->float;
	auto VectorMovementCollision(_In_ Vector3 startPoint1, _In_ Vector3 endPoint1, _In_ float v1, _In_ Vector3 startPoint2, _In_ float v2, _In_ float delay, _Out_ float t1, _Out_ Vector2 p1, _Out_ float t2, _Out_ Vector2 p2);
	auto GetCollisionPoint(float t, float sP1x, float sP1y, float S, float K)->Vector2;
	auto PredictPos(AIHeroClient* target, float delay, AIHeroClient* source, float speed, _Out_ float hitbox)->Vector3;
	auto GetBaitLevel(AIHeroClient* unit)->float;
};

extern std::unique_ptr<SPred> pSPred;