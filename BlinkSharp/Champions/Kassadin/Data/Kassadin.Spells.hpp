#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"
#include "../Kassadin.h"

/*
 * FULL COMBO: Q -> R -> E -> AA -> W -> AA 
 */

class NullSphere
{
public:
	static float GetDamage ()
	{
		std::vector<float> values = { 0.f, 65.f, 95.f, 125.f, 155.f, 185.f };
		return values[Player.GetSpell (0).Level] + Player.GetAbilityPower () * 0.7;
	}

	static float GetShield ()
	{
		std::vector<float> values = { 0.f, 60.f, 85.f, 110.f, 135.f, 160.f };
		return values[Player.GetSpell (0).Level] + Player.GetAbilityPower () * 0.3;
	}

	static void CastOffensive ()
	{
		const auto target = pCore->TS->GetTarget (pKassadin->Q.Range);
		if (Kassadin::Q.IsReady() && Game::Time () - Kassadin::Q.LastCast >= 1.f)
		{
			if (Menu::Get<int> ("combo.q.logic") == 0 && target) // Smart Q
			{
				if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo))
				{
					if (!Kassadin::R.IsLearned ())
						pSDK->Control->CastSpell (0, target);

					if (Player.GetBuffCount ("forcepulsecounter", false) < 5)
						pSDK->Control->CastSpell (0, target);

					if (Kassadin::R.IsReady () && Kassadin::E.IsReady () && Kassadin::W.IsReady () && Player.Distance (target) > Player.GetAttackRange () && Player.Distance (target) < pKassadin->R.Range)
						pSDK->Control->CastSpell (0, target);
				}
			}
			else
			{
				if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo))
				{
					pSDK->Control->CastSpell (0, target);
				}
			}
			
			if (Menu::Get<bool> ("killsteal.q"))
			{
				auto enemies = pSDK->EntityManager->GetEnemyHeroes (Kassadin::Q.Range);
				for (auto& enemy : enemies)
				{
					if (pCore->Orbwalker->CheckValidTarget(enemy.second, Kassadin::Q.Range) && enemy.second->IsAlive() && !enemy.second->IsInvulnerable())
					{
						if (enemy.second->GetHealth ().Current + enemy.second->GetHealth ().MagicalShield + 5.f <= Pred->MagicalDamage (target, GetDamage ()))
							Kassadin::Q.Cast (enemy.second);
					}
				}
			}

			if (Menu::Get<bool>("clear.q") && pCore->Orbwalker->IsModeActive(OrbwalkingMode::LaneClear) && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
			{
				auto minions = pSDK->EntityManager->GetEnemyMinions (Kassadin::Q.Range);
				for (auto& minion : minions)
				{
					if (minion.second->GetHealth ().Current <= Pred->MagicalDamage (minion.second, GetDamage ()) && minion.second->GetHealth().Current > 1.f)
						Kassadin::Q.Cast (minion.second);
				}
			}

			if (Menu::Get<bool> ("clear.q") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::JungleClear) && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
			{
				auto mobs = pSDK->EntityManager->GetJungleMonsters (Kassadin::Q.Range);
				for (auto& mob : mobs)
				{
					if (mob.second->IsAlive ())
					{
						Kassadin::Q.Cast (mob.second);
					}
				}
			}

			if (Menu::Get<bool>("combo.q.interrupt"))
			{
				auto enemies = pSDK->EntityManager->GetEnemyHeroes (Kassadin::Q.Range);
				for (auto& enemy : enemies)
				{
					if (enemy.second->IsAlive () && enemy.second->IsChannelingImportantSpell ())
						Kassadin::Q.Cast (enemy.second);
				}
			}

			if (Menu::Get<bool>("flee.q") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::Flee))
			{
				if (!Kassadin::R.IsReady () && !Kassadin::E.IsReady ())
					Kassadin::Q.Cast (target);
			}

			if (Menu::Get<bool>("harass.q") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::Mixed))
			{
				if (target->IsAlive () && target->IsAlive ())
					Kassadin::Q.Cast (target);
			}
		}
	}

	static void CastDefensive ()
	{

	}
};

class NetherBlade
{
public:
	static float PassiveDamage()
	{
		return 20 + (Player.GetAbilityPower() * .1f);
	}

	static float GetDamage()
	{
		std::vector<float> damage = { 0.f, 40.f, 65.f, 90.f, 115.f, 140.f };
		return damage[Player.GetSpell (1).Level] + (Player.GetAbilityPower () * 0.8) + PassiveDamage ();
	}

	static void Cast()
	{
		/* Todo: Will be handled in PostAttack */
	}
};

class ForcePulse
{
public:

	static float GetDamage()
	{
		std::vector<float> damage = { 0.f, 80.f, 105.f, 130.f, 155.f, 180.f };
		if (Kassadin::E.IsReady ())
			return damage[Player.GetSpell (2).Level] + (Player.GetAbilityPower () * 0.8);
		return 0;
	}

	static void Cast()
	{
		auto target = pCore->TS->GetTarget (Kassadin::E.Range);
		if (Kassadin::E.IsReady() && (Game::Time () - Kassadin::E.LastCast) >= 1.0f)
		{
			if (Menu::Get<bool>("combo.e.killsteal"))
			{
				auto enemies = pSDK->EntityManager->GetEnemyHeroes (Kassadin::E.Range);
				for (auto& enemy : enemies)
				{
					if (enemy.second->IsAlive () && enemy.second->GetHealth ().Current + enemy.second->GetHealth ().MagicalShield + 5.f <= Pred->MagicalDamage (enemy.second, GetDamage ()))
						Kassadin::E.Cast (&enemy.second->GetPosition ());
				}
			}

			if (Menu::Get<bool>("clear.e") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::JungleClear) && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
			{
				auto mobs = pSDK->EntityManager->GetJungleMonsters (Kassadin::E.Range);
				for (auto mob : mobs)
				{
					if (mob.second->IsAlive () && mob.second->GetHealth ().Current > 10.f)
						Kassadin::E.Cast (&mob.second->GetPosition ());
				}
			}

			if (Menu::Get<bool>("clear.e") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::LaneClear) && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
			{
				auto minions = pSDK->EntityManager->GetEnemyMinions (Kassadin::E.Range);
				for (auto& minion : minions)
				{
					if (minion.second->IsAlive () && minion.second->GetHealth ().Current > 5.f)
						Kassadin::E.Cast (&minion.second->GetPosition ());
				}
			}

			if (Menu::Get<int>("combo.q.logic") == 0 && pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo) && target)
			{
				if (!target->IsInvulnerable () && target->IsTargetable ())
				{
					if (!Kassadin::Q.IsReady () && !Kassadin::R.IsReady () && Player.Distance (target) <= Player.GetTrueAttackRange () + 50.f)
						Kassadin::E.Cast (&target->GetPosition ());
				}
			}
			else if (Menu::Get<int> ("combo.q.logic") == 1 && pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo) && target)
			{
				if (!target->IsInvulnerable () && target->IsTargetable ())
					Kassadin::E.Cast (&target->GetPosition());
			}

			if (Menu::Get<bool> ("flee.e") && pCore->Orbwalker->IsModeActive (OrbwalkingMode::Flee) && target)
			{
				if (!target->IsInvulnerable () && target->IsTargetable ())
					Kassadin::E.Cast (&target->GetPosition ());
			}
		}
	}
};

class Riftwalk
{
public:
	static float GetDamage()
	{
		std::vector<float> damage = { 0.f, 80.f, 100.f, 120.f };
		std::vector<float> bonusDamage = { 0.f, 40.f, 50.f, 60.f };
		return damage[Player.GetSpell (3).Level] + Player.GetAbilityPower () * 0.4 + Player.GetManaPercent () * 0.02 + Player.GetBuffCount("RiftWalk", false) * bonusDamage[Player.GetSpell(3).Level];
	}

	static void Cast()
	{
		const auto target = pCore->TS->GetTarget (Kassadin::Q.Range);
		if (target && Kassadin::R.IsReady () && Game::Time() - Kassadin::R.LastCast >= 1.0f)
		{
			if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo)) // Smart Q && R
			{
				if (Menu::Get<int> ("combo.r.safe") == 0)
				{
					Vector3 pPos = Player.GetPosition ();
					if (Kassadin::E.IsReady () && Kassadin::W.IsReady () && Utility::IsPositionSafe (target->GetPosition (), Kassadin::Q.Range))
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));

					if (Kassadin::W.IsReady () && Player.GetBuffCount ("forcepulsecounter", false) < 5 && Utility::IsPositionSafe (target->GetPosition (), Kassadin::Q.Range))
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));

					if (target->GetHealth().Current + target->GetHealth().MagicalShield + 5.f <= Pred->MagicalDamage(target, GetDamage() + 3 * Player.GetAttackDamage()))
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));
				}
				else
				{
					Vector3 pPos = Player.GetPosition ();
					if (Kassadin::E.IsReady () && Kassadin::W.IsReady ())
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));

					if (Kassadin::W.IsReady () && Player.GetBuffCount ("forcepulsecounter", false) < 5)
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));

					if (target->GetHealth ().Current + target->GetHealth ().MagicalShield + 5.f <= Pred->MagicalDamage (target, GetDamage () + 3 * Player.GetAttackDamage ()))
						Kassadin::R.Cast (&target->GetPosition ().Extended (pPos, 100.f));
				}
			}
		}
	}
};

class KassadinDamage
{
public:
	static float TotalDamage()
	{
		auto damage = 0;
		if (Kassadin::Q.IsReady ())
			damage += NullSphere::GetDamage ();
		if (Kassadin::W.IsReady ())
			damage += NetherBlade::GetDamage ();
		if (Kassadin::E.IsReady ())
			damage += ForcePulse::GetDamage ();
		if (Kassadin::R.IsReady ())
			damage += Riftwalk::GetDamage ();

		return damage;
	}
};