#pragma once
#include "SDK Extensions.h"

class xPrediction
{
public:
	auto BasePrediction(AIHeroClient* target, float range, float castdelay, float missilespeed, bool castMaxRange)->Vector3;
	auto CircularPrediction(AIHeroClient* target, float castTime, float range, float radius)->Vector3;
	auto IreliaPrediction(Vector3 bladePos, AIHeroClient* target, float range, float castdelay, float missilespeed)->Vector3;
	auto PhysicalDamage(AIBaseClient* target, float damage)->float;
	auto MagicalDamage(AIBaseClient* target, float damage)->float;
   auto FastPrediction(Vector3 blade, AIBaseClient* Target, float delay, float range) -> Vector3;
   auto FastPrediction(AIHeroClient* enemy, float delay)->Vector3;
};

extern std::unique_ptr<xPrediction> Pred;