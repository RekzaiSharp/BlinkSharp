#include "Irelia_Combo.h"
#include "../Irelia.h"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR red = { 0, 0, 255, 255 };
SDKVECTOR direction_vector = { 0, 0, 1.f };

auto Irelia_Combo::Combo_Tick() -> void
{
	const auto target = pCore->TS->GetTarget(600);

	if (!target || pIreliaCombo->ActiveMode == Irelia_Combo::ActiveModes::GapCloser)
	{
		Combo_Gapcloser();
		pIreliaCombo->ActiveMode = Irelia_Combo::ActiveModes::GapCloser;
	}
	 /*if (E->Ready() && Q->Ready() && R->Ready() && Player.GetHealthPercent() > 50)
		Combo_Fast();*/

	if (Player.Distance(target) <= 600 || pIreliaCombo->ActiveMode == Irelia_Combo::ActiveModes::Trading)
	{
		pIreliaCombo->Combo_Killable();
		pIreliaCombo->BasicTrade();
		pIreliaCombo->ActiveMode = Irelia_Combo::ActiveModes::Trading;
	}
}

auto Irelia_Combo::BasicTrade() -> void
{
	const auto target = pCore->TS->GetTarget(850);

	if (pIrelia->msc_stack)
		Gather_Stacks();

	if (pIrelia->E.IsReady() && !pIreliaCombo->first_cast)
	{
		SdkCastSpellLocalPlayer(nullptr, &target->GetPosition(), 2, 0);
		pIreliaCombo->first_cast = true;
	}

	if (pIrelia->E.IsReady() && pIreliaCombo->first_cast)
	{
		auto delay = Player.GetSpell(2).CastDelay;
		auto pPos = Pred->BasePrediction(target, 850, delay, 1000, false);

		if (pPos.IsValid())
			SdkCastSpellLocalPlayer(nullptr, &pPos, 2, 0);

		pIreliaCombo->first_cast = false;
		pIreliaCombo->ActiveMode = ActiveModes::None;
	}

	if (pIrelia->Q.IsReady() && target->HasBuff("ireliamark", false, true))
		pSDK->Control->CastSpell(0, target);
}

auto Irelia_Combo::Combo_Fast() -> void
{
	//
}

auto Irelia_Combo::Combo_Killable() -> void
{
	auto target = pCore->TS->GetTarget(600, DamageType::Physical);

	if (target)
	{
		if (target->GetHealth().Current <= Pred->PhysicalDamage(target, pIrelia->QDamage(false) + Player.GetAttackDamage()) && pIrelia->Q.IsReady())
			pSDK->Control->CastSpell(0, target);
	}
}

auto Irelia_Combo::Gather_Stacks() -> void
{
	if (Player.GetBuffCount("ireliapassivestacks", false, true) < 4 && pIrelia->Q.IsReady())
	{
		auto minions = pSDK->EntityManager->GetEnemyMinions(600);
		for (auto& minion : minions)
		{
			if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)) && pIrelia->Q.IsReady())
				pSDK->Control->CastSpell(0, minion.second);
		}
	}
}

auto Irelia_Combo::Combo_Gapcloser() -> void
{
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
	{
		const auto target = pCore->TS->GetTarget(600, DamageType::Physical);

		if (!target)
		{
			auto minions = pSDK->EntityManager->GetEnemyMinions(600);
			for (auto& minion : minions)
			{
				if (minion.second->GetHealth().Current <= Pred->PhysicalDamage(minion.second, pIrelia->QDamage(true)) && !pIreliaCombo->gap_minion)
				{
					const auto distance = Player.Distance(minion.second);
					const auto gap_target = pCore->TS->GetTarget(600 + distance);
					if (gap_target && pIrelia->Q.IsReady() && minion.second->IsAlive())
					{
						pIreliaCombo->gap_minion = minion.second;
						SdkCastSpellLocalPlayer(nullptr, &Player.GetPosition(), 2, SPELL_CAST_START);
						pSDK->Control->CastSpell(0, pIreliaCombo->gap_minion);
						gap_closed = true;
						pIreliaCombo->gap_target = gap_target;
					}
				}
			}
		}

		if (Player.Distance(pIreliaCombo->gap_target) >= 600 && pIreliaCombo->gap_minion)
		{
			if (!pIreliaCombo->gap_minion->IsAlive())
			{
				pIreliaCombo->gap_minion = nullptr;
				pIreliaCombo->ActiveMode = ActiveModes::None;
				gap_closed = false;
			}
		}

		if (pIreliaCombo->gap_target && pIrelia->Q.IsReady() && Player.Distance(pIreliaCombo->gap_target) <= 600)
		{
			auto delay = Player.GetSpell(2).CastDelay;
			auto pPos = Pred->BasePrediction(target, 850, delay, 1000, true);

			if (pPos.IsValid())
				SdkCastSpellLocalPlayer(nullptr, &pPos, 2, SPELL_CAST_START);

			if (pIreliaCombo->gap_target->HasBuff("ireliamark", false, true) && pIrelia->Q.IsReady())
			{
				pSDK->Control->CastSpell(0, pIreliaCombo->gap_target);
				pIreliaCombo->gap_target = nullptr;
				pIreliaCombo->gap_closed = false;
				pIreliaCombo->gap_minion = nullptr;
				pIreliaCombo->ActiveMode = ActiveModes::None;
			}
		}
	}
}

std::unique_ptr<Irelia_Combo> pIreliaCombo = std::make_unique<Irelia_Combo>();
