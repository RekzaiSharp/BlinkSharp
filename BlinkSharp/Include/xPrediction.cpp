#include "xPrediction.h"

auto xPrediction::BasePrediction(AIHeroClient* target, float range, float castdelay, float missilespeed, bool castMaxRange) -> Vector3
{
	auto tNav = target->NavInfo();

	if (tNav.IsDashing)
	{
		if (Player.GetPosition().Distance(tNav.EndPos) < range)
			return tNav.EndPos;
	}

	if (tNav.Waypoints && tNav.NumberOfWaypoints)
	{
		float distance1 = Player.Distance(target);
		auto Waypoint = tNav.Waypoints[tNav.NextWaypoint];
		float distance2 = target->Distance(Waypoint);
		float unitperms = target->GetMovementSpeed() / 1000;
		float extensionValue = castdelay * unitperms;
		Vector3 tPos = target->GetPosition();
		Vector3 pPos = Player.GetPosition();
		auto extensionvec = Waypoint.Extended(tPos, (extensionValue + (target->GetBoundingRadius() / 2)));
		auto extensionValue2 = ((extensionvec.Distance(pPos) / missilespeed) * 1000) * unitperms;

		extensionvec = Waypoint.Extended(tPos, distance2 - (extensionValue + extensionValue2 + (target->GetBoundingRadius() / 2)));

		float distance3 = tPos.Distance(Waypoint);
		float distance4 = tPos.Distance(extensionvec);

		if (distance3 < distance4)
			return Vector3(0, 0, 0);

		return extensionvec;
	}
}

std::unique_ptr<xPrediction> Pred = std::make_unique<xPrediction>();
