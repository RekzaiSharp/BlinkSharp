#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"
#include "../Data/Irelia.Spells.hpp"
#include "../Irelia.h"

class IreliaClear
{
public:
	static void LaneClear()
	{
		if (Menu::Get<bool> ("clear.q") && Player.GetManaPercent () >= Menu::Get<float> ("clear.q.mana"))
		{
			auto minions = pSDK->EntityManager->GetEnemyMinions (600);
			for (auto& minion : minions)
			{
				if (minion.second->GetHealth ().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage (true)) && pIrelia->Q.IsReady () && minion.second->IsAlive ())
				{
					if (Utility::IsUnderOwnTurret (minion.second) && Menu::Get<bool> ("combo.q.turret"))
					{
						auto pos = minion.second->GetPosition ();
						auto ally_minions = pSDK->EntityManager->GetAllyMinions (600, &pos);
						for (auto& aMinion : ally_minions)
						{
							if (Utility::IsUnderTurret (aMinion.second))
								pSDK->Control->CastSpell (0, minion.second);
						}
					}
					else
					{
						pSDK->Control->CastSpell (0, minion.second);
					}
				}
				else if (strcmp(minion.second->GetName(), "YorickGhoulMelee") == 0)
				{
					pSDK->Control->CastSpell (0, minion.second);
				}
			}
		}
	}

	static void JungleClear()
	{
		auto mobs = pSDK->EntityManager->GetJungleMonsters (600);

		if (Menu::Get<bool>("clear.q") && Player.GetManaPercent () >= Menu::Get<float> ("clear.q.mana"))
		{
			for (auto& mob : mobs)
			{
				if (mob.second->GetHealth ().Current < Pred->PhysicalDamage (mob.second, BladeSurge::GetDamage (false)) && mob.second->GetHealth ().Current > 1.f && pCore->TS->IsValidTarget(mob.second) && mob.second->IsValid() && !mob.second->IsImmobile())
					SdkCastSpellLocalPlayer (mob.second->PTR (), nullptr, 0, 0);
			}
		}
	}
};