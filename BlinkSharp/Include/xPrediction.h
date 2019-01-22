#pragma once
#include "SDK Extensions.h"

class xPrediction
{
public:
	auto BasePrediction(AIHeroClient* target, float range, float castdelay, float missilespeed, bool castMaxRange)->Vector3;
	auto CircularPrediction(AIHeroClient* target, float castTime, float range, float radius);
};

extern std::unique_ptr<xPrediction> Pred;