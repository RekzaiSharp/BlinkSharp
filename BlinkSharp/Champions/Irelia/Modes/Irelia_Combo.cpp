#include "Irelia_Combo.h"
#include "../Irelia.h"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR red = {0, 0, 255, 255};
SDKVECTOR direction_vector = {0, 0, 1.f};

auto Irelia_Combo::Combo_Tick() -> void
{
	const auto target = pCore->TS->GetTarget(700);

	if (!target)
	{
		Combo_Gapcloser();
	}

	if (Player.Distance(target) <= 700)
	{
		pIreliaCombo->BasicTrade();
		pIreliaCombo->Combo_Killable();
	}

	if (Player.Distance(target) <= Player.GetSpell(1).CastRange && pIrelia->W.IsReady())
		pIreliaCombo->Combo_Melee();
}

auto Irelia_Combo::BasicTrade() -> void
{
	const auto target = pCore->TS->GetTarget(850);
	const auto targetq = pCore->TS->GetTarget(600);

	if (pIrelia->msc_stack)
		Gather_Stacks(targetq);

	if (pIrelia->E.IsReady() && !pIrelia->active_blade.net_id && pIrelia->e_mode == 0)
	{
		SdkCastSpellLocalPlayer(nullptr, &Player.GetPosition(), 2, 0);
		pIreliaCombo->first_cast = true;
	}

	if (pIrelia->E.IsReady() && pIrelia->active_blade.net_id && Player.Distance(target) <= 850)
	{
		auto delay = Player.GetSpell(2).CastDelay;
		if (target->IsValid() && target->IsAlive())
		{
			auto pPos = Pred->FastPrediction(pIrelia->active_blade.position, target, 50, 850);

			if (pPos.IsValid())
				SdkCastSpellLocalPlayer(nullptr, &pPos, 2, 0);

			pIreliaCombo->first_cast = false;
		}
	}

	auto enemies = pSDK->EntityManager->GetEnemyHeroes(600);

	for (auto& enemy : enemies)
	{
		if (pIrelia->Q.IsReady() && enemy.second->HasBuff("ireliamark", false, true))
			pSDK->Control->CastSpell(0, enemy.second);
	}
	
}

auto Irelia_Combo::Combo_Fast() -> void
{
}

auto Irelia_Combo::Combo_Melee() -> void
{
	auto target = pCore->TS->GetTarget(Player.GetSpell(1).CastRange);

	if (target->IsValid() && target->IsAlive() && pIrelia->w_mode == 1)
	{
		SdkCastSpellLocalPlayer(nullptr, &target->GetPosition(), 1, SPELL_CAST_START);
	}
}

auto Irelia_Combo::Combo_Killable() -> void
{
	auto target = pCore->TS->GetTarget(600, DamageType::Physical);

	if (target)
	{
		if (target->GetHealth().Current <= Pred->PhysicalDamage(
			target, pIrelia->QDamage(false) + (Player.GetAttackDamage() * 3)) && pIrelia->Q.IsReady())
			pSDK->Control->CastSpell(0, target);
	}
}

auto Irelia_Combo::Gather_Stacks(AIBaseClient* target) -> void
{
	if (Player.GetBuffCount("ireliapassivestacks", false, true) < 4 && pIrelia->Q.IsReady())
	{
		auto minions = pSDK->EntityManager->GetEnemyMinions(600);
		for (auto& minion : minions)
		{
			if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)) &&
				pIrelia->Q.IsReady() && minion.second->Distance(target) <= 600)
			{
				pSDK->Control->CastSpell(0, minion.second);
			}
		}
	}
}

auto Irelia_Combo::Combo_Gapcloser() -> void
{
	auto minions = pSDK->EntityManager->GetEnemyMinions(600);
	for (auto& minion : minions)
	{
		if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)))
		{
			auto distance = Player.Distance(minion.second) + 600;
			auto target = pCore->TS->GetTarget(distance);
			if (target && pIrelia->Q.IsReady())
			{
				pSDK->Control->CastSpell(0, minion.second);
			}
		}
	}
}

std::unique_ptr<Irelia_Combo> pIreliaCombo = std::make_unique<Irelia_Combo>();
