#pragma once
#include "../../Include/SDK Extensions.h"

class Utility
{
private:
	static int LowEnemiesInRange(float range, Vector3 from)
	{
		auto count = 0;
		auto enemies = pSDK->EntityManager->GetEnemyHeroes (range, &from);
		for (auto enemy : enemies)
		{
			if (enemy.second->GetHealthPercent () < 20)
				count++;
		}
		return count;
	}
	static int EnemiesInRange(float range, Vector3 from)
	{
		auto count = 0;
		auto enemies = pSDK->EntityManager->GetEnemyHeroes (range, &from);
		return enemies.size ();
	}
	static int AlliesInRange(float range, Vector3 from)
	{
		auto count = 0;
		auto enemies = pSDK->EntityManager->GetAllyHeroes (range, &from);
		return enemies.size ();
	}

public:
	static bool IsUnderTurret (AIBaseClient* AI)
	{
		if (AI->GetTeamID () == TEAM_TYPE_CHAOS)
		{
			const auto turret = pSDK->EntityManager->GetNearestTurret (AI, &AI->GetPosition (), TEAM_TYPE_ORDER);
			if (turret)
			{
				if (AI->Distance (turret) <= 775)
					return true;
			}
		}
		else
		{
			const auto turret = pSDK->EntityManager->GetNearestTurret (AI, &AI->GetPosition (), TEAM_TYPE_CHAOS);
			if (turret)
			{
				if (AI->Distance (turret) <= 775)
					return true;
			}
		}
		return false;
	}
	static bool IsUnderOwnTurret (AIBaseClient* AI)
	{
		if (AI->GetTeamID () == TEAM_TYPE_CHAOS)
		{
			const auto turret = pSDK->EntityManager->GetNearestTurret (AI, &AI->GetPosition (), TEAM_TYPE_CHAOS);
			if (turret)
			{
				if (AI->Distance (turret) <= 775)
					return true;
			}
		}
		else
		{
			const auto turret = pSDK->EntityManager->GetNearestTurret (AI, &AI->GetPosition (), TEAM_TYPE_ORDER);
			if (turret)
			{
				if (AI->Distance (turret) <= 775)
					return true;
			}
		}
		return false;
	}
	static bool IsPosUnderTurret (Vector3 pos)
	{
		if (Player.GetTeamID () == TEAM_TYPE_ORDER)
		{
			auto turret = pSDK->EntityManager->GetNearestTurret (&Player, &Player.GetPosition (), TEAM_TYPE_CHAOS);
			auto tPos = turret->GetPosition ();
			if (pos.Distance (tPos) <= 775)
				return true;
		}
		else
		{
			auto turret = pSDK->EntityManager->GetNearestTurret (&Player, &Player.GetPosition (), TEAM_TYPE_ORDER);
			auto tPos = turret->GetPosition ();
			if (pos.Distance (tPos) <= 775)
				return true;
		}
		return false;
	}
	static bool IsPosUnderOwnTurret (Vector3 pos)
	{
		if (Player.GetTeamID () == TEAM_TYPE_ORDER)
		{
			auto turret = pSDK->EntityManager->GetNearestTurret (&Player, &Player.GetPosition (), TEAM_TYPE_ORDER);
			auto tPos = turret->GetPosition ();
			if (pos.Distance (tPos) <= 775)
				return true;
		}
		else
		{
			auto turret = pSDK->EntityManager->GetNearestTurret (&Player, &Player.GetPosition (), TEAM_TYPE_CHAOS);
			auto tPos = turret->GetPosition ();
			if (pos.Distance (tPos) <= 775)
				return true;
		}
		return false;
	}

	static bool IsPositionSafe(Vector3 Position, float range) // Range = Radius of check
	{
		auto allies = 1 + AlliesInRange (range, Position); // count myself in
		const auto enemies = EnemiesInRange (range, Position);
		const auto lowEnemies = LowEnemiesInRange (range, Position);

		if (IsPosUnderOwnTurret (Position))
			allies += 2;

		return allies >= enemies - lowEnemies || enemies < 2;
	}
};
