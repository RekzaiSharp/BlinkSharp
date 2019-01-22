#pragma once
#include "SDK Extensions.h"

class xPrediction
{
public:
	auto BasePrediction(AIHeroClient* target, float range, float castdelay, float missilespeed, bool castMaxRange)->Vector3;
};

extern std::unique_ptr<xPrediction> Pred;