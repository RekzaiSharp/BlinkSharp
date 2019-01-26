#include <algorithm>
#include "Utility.h"


bool Utility::IsImmobilizeBuff(eBuffType type)
{
	return type == BUFF_Snare || type == BUFF_Stun || type == BUFF_Charm || type == BUFF_Knockup || type == BUFF_Suppression;
}

bool Utility::IsImmobileTarget(AIBaseClient *target)
{
	return false;
}

float Utility::LeftImmobileTime(AIBaseClient *target)
{
	return 0;
}