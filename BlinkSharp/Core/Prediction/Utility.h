#pragma once
#include "../../Include/SDK Extensions.h"
#include "SDK/PluginData.h"

class Utility
{
public:
	static bool IsImmobilizeBuff(eBuffType type);
	static bool IsImmobileTarget(AIBaseClient *target);
	static float LeftImmobileTime(AIBaseClient *target);
};

