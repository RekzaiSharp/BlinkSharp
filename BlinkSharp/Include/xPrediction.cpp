#include "xPrediction.h"

auto xPrediction::PhysicalDamage(AIBaseClient* target, float damage) -> float
{
	auto ArmorPenPercent = Player.GetPercentArmorPen ();
	auto ArmorPenFlat = Player.GetFlatArmorPen ();
	auto Armor = target->GetArmor ();
	float BonusArmor;
	float BonusArmorPen;
	SdkGetAIBonusArmor (target->PTR (), &BonusArmor);
	SdkGetAIBonusArmorPenetration (Player.PTR (), &BonusArmorPen);

	auto value = 100 / (100 + (Armor * ArmorPenPercent) - (BonusArmor * (1 - BonusArmorPen)) - ArmorPenFlat);

	if (Armor < 0.0) { value = 2 - 100 / (100 - Armor); }
	else if (Armor * ArmorPenPercent - ArmorPenFlat < 0) { value = 1; }

	if (target->HasBuff("hand of baron", false) && target->IsMinion())
	{
		if (Game::Time () < 1800)
			damage *= 0.5;
		else if (Game::Time () < 2400)
			damage *= 0.42;
		else if (Game::Time () > 2400)
			damage *= 0.3;
	}

	return std::fmax (0, std::floor (value * damage));
}

auto xPrediction::MagicalDamage(AIBaseClient* target, float damage) -> float
{
	auto MR = target->GetMagicResist ();
	auto MagicPenPercent = Player.GetPercentMagicPen ();
	auto MagicPenFlat = Player.GetFlatMagicPen ();
	auto value = 100 / (100 + (MR * MagicPenPercent) - MagicPenFlat);

	if (MR < 0) { value = 2 - 100 / (100 - MR); }
	else if ((MR * MagicPenPercent) - MagicPenFlat < 0) { value = 1; }

	if (target->HasBuff ("hand of baron", false) && target->IsMinion ())
	{
		if (Game::Time () < 1800)
			damage *= 0.5;
		else if (Game::Time () < 2400)
			damage *= 0.42;
		else if (Game::Time () > 2400)
			damage *= 0.3;
	}

	return std::fmax (0, std::floor (value * damage));
}

auto xPrediction::FastPrediction(AIHeroClient* target, float delay, float range, float speed) -> Vector3
{
	if (!target->IsAlive ())
		return Vector3 (0, 0, 0);

	auto tNav = target->NavInfo ();

	if (tNav.NumberOfWaypoints < 1)
	{
		return target->GetPosition ();
	}

	if (tNav.IsDashing)
	{
		if (Player.GetPosition ().Distance (tNav.EndPos) < range)
			return tNav.EndPos;
	}

	if (tNav.Waypoints && tNav.NumberOfWaypoints)
	{
		float distance1 = Player.Distance (target);
		auto Waypoint = tNav.Waypoints[tNav.NextWaypoint];
		float distance2 = target->Distance (Waypoint);
		float unitperms = target->GetMovementSpeed () / 1000;
		float extensionValue = delay * unitperms;
		Vector3 tPos = target->GetPosition ();
		Vector3 pPos = Player.GetPosition();
		auto extensionvec = Waypoint.Extended (tPos, (extensionValue + (target->GetBoundingRadius () / 2)));
		auto extensionValue2 = ((extensionvec.Distance (pPos) / speed) * 1000) * unitperms;

		extensionvec = Waypoint.Extended (tPos, distance2 - (extensionValue + extensionValue2 + (target->GetBoundingRadius () / 2)));

		return extensionvec;
	}
}


std::unique_ptr<xPrediction> Pred = std::make_unique<xPrediction>();

auto xPrediction::IreliaPrediction(Vector3 blade, AIHeroClient* target, float range, float castdelay,
	float missilespeed) -> Vector3
{
	if (!target->IsAlive())
		return Vector3(0, 0, 0);

	auto tNav = target->NavInfo();

	if (tNav.NumberOfWaypoints < 1)
	{
		auto directiontVec = (target->GetPosition() - blade).Normalized ();
		auto distance = (target->Distance (blade) - Player.Distance (target)) + range;
		auto result = (distance * directiontVec) + blade;
		return result;
	}
		
	if (tNav.IsDashing)
	{
		if (Player.GetPosition().Distance(tNav.EndPos) < range)
			return tNav.EndPos;
	}

	if (tNav.Waypoints && tNav.NumberOfWaypoints)
	{
		float distance1 = target->Distance(blade);
		auto Waypoint = tNav.Waypoints[tNav.NextWaypoint];
		float distance2 = target->Distance(Waypoint);
		float unitperms = target->GetMovementSpeed() / 1000;
		float extensionValue = castdelay * unitperms;
		Vector3 tPos = target->GetPosition();
		Vector3 pPos = blade;
		auto extensionvec = Waypoint.Extended(tPos, (extensionValue + (target->GetBoundingRadius() / 2)));
		auto extensionValue2 = ((extensionvec.Distance(pPos) / missilespeed) * 1000) * unitperms;

		extensionvec = Waypoint.Extended(tPos, distance2 - (extensionValue + extensionValue2 + (target->GetBoundingRadius() / 2)));

		auto directiontVec = (extensionvec - blade).Normalized ();
		auto distance = (target->Distance (blade) - Player.Distance (target)) + range;
		auto result = (distance * directiontVec) + blade;

		return result;
	}
}
