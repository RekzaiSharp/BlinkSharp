#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"
#include "../Data/Irelia.Spells.hpp"
#include "../Irelia.h"
#include <algorithm>
#include <set>

class IreliaCombo
{
public:
	static void Tick()
	{
		const auto target = pCore->TS->GetTarget (700);
		
		if (!target)
		{
			Gapcloser ();
		}

		if (target)
		{
			Combo ();
			if (Menu::Get<bool>("combo.w") && Menu::Get<int>("combo.w.mode") == 0 && pIrelia->W.IsReady())
			{
				SdkCastSpellLocalPlayer (nullptr, &target->GetPosition (), 1, 0);
			}
		}

	}

	static void Combo()
	{
		const auto target = pCore->TS->GetTarget (600.f);
		if (target)
			if (Menu::Get<bool> ("combo.q.stack") && Player.GetManaPercent () >= Menu::Get<float> ("clear.q.mana"))
				GatherStacks (target);

		BladeSurge::Cast ();
		VanguardsEdge::Cast ();
		FlawlessDuet::Cast ();
	}

	static void SmartTrade()
	{
		/*auto target = pCore->TS->GetTarget (750);

		if (Irelia::Q.IsReady() && target)
		{

			FlawlessDuet::Cast ();

			if (pIrelia->Q.IsReady () && target->HasBuff ("ireliamark", false))
				pSDK->Control->CastSpell (0, target);

			if (Player.Distance (target) <= Player.GetAttackRange () + 100.f && pIrelia->W.IsReady () && !pIrelia->isCastingW && !target->IsImmobile())
			{
				pSDK->Control->CastSpell (1, &target->GetPosition (), false);
			
				pSDK->EventHandler->DelayedAction ([&target]
				{
					pSDK->Control->CastSpell (1, &target->GetPosition(), true);
				}, 2000);
			}

			if (!Irelia::W.IsReady() && Irelia::Q.IsReady() && !Irelia::E.IsReady() && target->GetHealthPercent() >= Menu::Get<float>("harass.q.threshold") && Player.Distance (target) <= Player.GetAttackRange () + 50.f)
			{
				auto minions = pSDK->EntityManager->GetEnemyMinions (600);
				for (auto& minion : minions)
				{
					if (minion.second->GetHealth ().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage (true)) && Player.Distance(target) < minion.second->Distance(target))
					{
						pSDK->Control->CastSpell (0, minion.second);
					}
				}
			}
		}*/
	}

	static void GatherStacks(AIBaseClient* target)
	{
		if (target->HasBuff ("ireliamark", false) && !pIrelia->E.IsReady ())
		{
			if (Player.GetBuffCount ("ireliapassivestacks", false) < 5 && pIrelia->Q.IsReady ())
			{
				auto minions = pSDK->EntityManager->GetEnemyMinions (600);
				for (auto& minion : minions)
				{
					if (minion.second->GetHealth ().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage (true)) &&
						pIrelia->Q.IsReady () && Player.Distance (target) >= minion.second->Distance (target))
					{
						if (Utility::IsUnderOwnTurret (minion.second) && Menu::Get<bool> ("combo.q.turret"))
						{
							auto ally_minions = pSDK->EntityManager->GetAllyMinions (600, &minion.second->GetPosition ());
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
					else if (strcmp (minion.second->GetName (), "YorickGhoulMelee") == 0 && Player.Distance (target) >= minion.second->Distance (target))
					{
						pSDK->Control->CastSpell (0, minion.second);
					}
				}
			}
		}

		if (!target->HasBuff ("ireliamark", false))
		{
			if (Player.GetBuffCount ("ireliapassivestacks", false) < 5 && pIrelia->Q.IsReady ())
			{
				auto minions = pSDK->EntityManager->GetEnemyMinions (600);
				for (auto& minion : minions)
				{
					if (minion.second->GetHealth ().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage (true)) &&
						pIrelia->Q.IsReady () && minion.second->Distance(target) <= 600)
					{
						if (Utility::IsUnderOwnTurret (minion.second) && Menu::Get<bool>("combo.q.turret"))
						{
							auto ally_minions = pSDK->EntityManager->GetAllyMinions (600, &minion.second->GetPosition ());
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
					else if (strcmp (minion.second->GetName (), "YorickGhoulMelee") == 0 && minion.second->Distance (target) <= 600)
					{
						pSDK->Control->CastSpell (0, minion.second);
					}
				}
			}
		}
	}

	static void Gapcloser()
	{
		auto minions = pSDK->EntityManager->GetEnemyMinions (600);
		for (auto& minion : minions)
		{
			if (minion.second->GetHealth ().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage (true)))
			{
				const auto distance = Player.Distance (minion.second) + 600; const auto target = pCore->TS->GetTarget (distance);
				if (target && pIrelia->Q.IsReady ())
					if (Player.Distance (target) > minion.second->Distance (target))
					{
						pSDK->Control->CastSpell (0, minion.second);
					}
			}
		}
	}
};
