#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class DamageLib
{
public:
	static void DrawDamage (AIBaseClient* target, float Damage)
	{
		Vector2 StartPosition;
		Vector2 EndPosition = target->GetHealthBarScreenPos ();

		EndPosition.x += -46 + target->GetHealth ().Current / target->GetHealth ().Max * 105;
		EndPosition.y += -24;

		StartPosition = EndPosition;

		StartPosition.x += -(Damage / target->GetHealth ().Max * 105);

		if (StartPosition.x < target->GetHealthBarScreenPos ().x - 46)
		{
			StartPosition.x = target->GetHealthBarScreenPos ().x - 46;
		}
		SDKCOLOR WhiteHP = { 255, 255, 255, 120 };
		Draw::LineScreen (&StartPosition, &EndPosition, 12, &WhiteHP);
	}

	static void DrawSpellDamage(AIBaseClient* target, float QDamage, float WDamage, float EDamage, float RDamage)
	{
		
	}
};