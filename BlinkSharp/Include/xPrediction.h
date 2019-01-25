#pragma once
#include "SDK Extensions.h"
#include "../Champions/Irelia/Irelia.h"

class xPrediction
{
public:
	auto BasePrediction(AIHeroClient* target, float range, float castdelay, float missilespeed, bool castMaxRange)->Vector3;
	auto CircularPrediction(AIHeroClient* target, float castTime, float range, float radius)->Vector3;
	auto IreliaPrediction(Irelia::stun_blade blade, AIHeroClient* target, float range, float castdelay, float missilespeed)->Vector3;
	auto PhysicalDamage(AIBaseClient* target, float damage)->float;
	auto MagicalDamage(AIBaseClient* target, float damage)->float;
};

extern std::unique_ptr<xPrediction> Pred;