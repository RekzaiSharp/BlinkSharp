#include "Irelia_Clear.hpp"
#include "../Irelia.h"

SDKCOLOR redx = { 0, 0, 255, 255 };
SDKVECTOR dirx = { 0, 0, 1.f };

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
			pSDK->Control->CastSpell(0, minion.second);
		}
			
	}
}

auto Irelia_Clear::Draw_Clear() -> void
{
	if (pIrelia->msc_draw_clear)
	{
		pIreliaClear->Clear_Targets.clear();
		auto minions = pSDK->EntityManager->GetEnemyMinions(1200);
		for (auto& minion : minions)
		{
			if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)) && pIrelia->Q.IsReady() && minion.second->IsAlive())
				pIreliaClear->Clear_Targets.emplace_back(minion.second);
		}

		if (pIreliaClear->Clear_Targets.size() > 1)
		{
			SdkDrawLine(&Player.GetPosition(), &pIreliaClear->Clear_Targets[0]->GetPosition(), 3, &redx, 0);
			SdkDrawCircle(&pIreliaClear->Clear_Targets[0]->GetPosition(), pIreliaClear->Clear_Targets[0]->GetBoundingRadius(), &redx, 1, &dirx);
			for (int i = 1; i <= pIreliaClear->Clear_Targets.size(); ++i)
			{
				if (pIreliaClear->Clear_Targets[i]->IsValid() && pIreliaClear->Clear_Targets[i - 1]->IsValid())
				{
					SdkDrawLine(&pIreliaClear->Clear_Targets[i - 1]->GetPosition(), &pIreliaClear->Clear_Targets[i]->GetPosition(), 3, &redx, 0);
					SdkDrawCircle(&pIreliaClear->Clear_Targets[i]->GetPosition(), pIreliaClear->Clear_Targets[i]->GetBoundingRadius(), &redx, 1, &dirx);
				}
			}
		}
	}
}

std::unique_ptr<Irelia_Clear> pIreliaClear = std::make_unique<Irelia_Clear>();
