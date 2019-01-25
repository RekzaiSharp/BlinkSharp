#include "Irelia_Clear.hpp"
#include "../../../Include/SDK Extensions.h"
#include "../Irelia.h"

auto Irelia_Clear::Clear_Tick() -> void
{
	if (pIrelia->clear_q && Player.GetManaPercent() > pIrelia->clear_qmana)
		Clear_Basic();
}

auto Irelia_Clear::Clear_Basic() -> void
{
	auto minions = pSDK->EntityManager->GetEnemyMinions(600);
	for (auto& minion : minions)
	{
		if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)) && pIrelia->Q.IsReady() && minion.second->IsAlive())
		{
			Game::PrintChat("Minion Health: " + std::to_string(minion.second->GetHealth().Current));
			Game::PrintChat("Estimated Damage: " + std::to_string(Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true))));
			pSDK->Control->CastSpell(0, minion.second);
		}
			
	}
}

std::unique_ptr<Irelia_Clear> pIreliaClear = std::make_unique<Irelia_Clear>();