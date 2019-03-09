#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class IonianFervor
{
public:
	static float GetDamage ()
	{
		if (Player.GetBuffCount("ireliapassivestacks") == 5)
		{
			return 12 + 3 * Player.GetLevel () + Player.GetBonusAttackDamage () * 0.25;
		}
		return 0;
	}
};

class BladeSurge
{
public:
	static float GetDamage (bool minion)
	{
		const auto level = Player.GetSpell (0).Level;
		std::vector<float> damage = { 0.f, 5.f, 25.f, 45.f, 65.f, 85.f };
		std::vector<float> minion_damage{ 0.f, 55.f, 75.f, 95.f, 115.f, 135.f };

		if (minion)
			return damage[level] + (Player.GetAttackDamage () * 0.6) + minion_damage[level] + IonianFervor::GetDamage();

		return damage[level] + (Player.GetAttackDamage () * 0.6) + IonianFervor::GetDamage();
	}

	static void Cast()
	{
		if (Menu::Get<bool>("combo.q") && Irelia::Q.IsReady())
		{
			const auto target = pCore->TS->GetTarget (Irelia::Q.Range);

			if (target && pCore->TS->IsValidTarget(target))
			{
				if (Menu::Get<bool> ("combo.q.ks")) // Execute
				{
					auto enemies = pSDK->EntityManager->GetEnemyHeroes (Irelia::Q.Range);
					for (auto& enemy : enemies)
					{
						if (enemy.second->Distance (target) <= Irelia::E.Range && enemy.second->GetHealth ().Current + enemy.second->GetHealth ().PhysicalShield + 10 <=
							Pred->PhysicalDamage (enemy.second, GetDamage (false)) && pIrelia->lastCheck + 250 <= GetTickCount ())
						{
							pIrelia->lastCheck = GetTickCount ();
							Irelia::Q.Cast (enemy.second);
						}
					}
				}

				if (Menu::Get<bool> ("combo.q.spam")) // Jump on others first
				{
					auto enemies = pSDK->EntityManager->GetEnemyHeroes (Irelia::Q.Range);
					for (auto& enemy : enemies)
					{
						if (enemy.second->PTR () == target->PTR ())
							continue;

						if (enemy.second->HasBuff ("ireliamark", false) && enemy.second->Distance (target) <= 600 && enemy.second->IsAlive ())
							Irelia::Q.Cast (enemy.second);
					}
				}

				if (Menu::Get<int> ("combo.q.mode") == 0) // Smart Mode
				{
					if (Player.Distance (target) >= 450.f && Player.Distance (target) <= 600 && target->GetHealth ().Current + target->GetHealth ().PhysicalShield + 10 <= Pred->PhysicalDamage (target, GetDamage (false) + Player.GetAttackDamage () * 2))
					{
						Irelia::Q.Cast (target);
					}
						

					if (target->HasBuff ("ireliamark", false) && pIrelia->lastCheck + 250 <= GetTickCount())
					{
						pIrelia->lastCheck = GetTickCount ();
						Irelia::Q.Cast (target);
					}
				}

				if (Menu::Get<int> ("combo.q.mode") == 1) // Marked Only
				{
					if (target->HasBuff ("ireliamark", false) && pIrelia->lastCheck + 250 <= GetTickCount ())
					{
						pIrelia->lastCheck = GetTickCount ();
						Irelia::Q.Cast (target);
					}
				}
			}
		}
	}
};

class DefiantDance
{
public:
	static float GetDamage ()
	{
		std::vector<float> damage = { 0.f, 10.f, 25.f, 40.f, 55.f, 70.f };
		return damage[Player.GetSpell(1).Level] + (Player.GetAttackDamage () * 0.5) + (Player.GetAbilityPower() * 0.4);
	}
};

class FlawlessDuet
{
public:
	static float GetDamage ()
	{
		std::vector<float> damage = { 0.f, 70.f, 110.f, 150.f, 190.f, 230.f };
		return damage[Player.GetSpell (2).Level] + (Player.GetAbilityPower () * 0.8);
	}

	static void Cast()
	{
		if (Menu::Get<int> ("combo.e.prediction") == 0)
			Reborn ();
		else
			Divine ();
	}

	static void Reborn()
	{
		const auto target = pCore->TS->GetTarget (Irelia::E.Range - 50.f);
		if (Menu::Get<bool> ("combo.e"))
		{
			if (strcmp (Player.GetSpell (2).Name, "IreliaE") == 0 && Irelia::E.IsReady () && target)
			{
				auto ePred = Irelia::E.GetPrediction (target); auto x = Player.GetPosition ();
				auto finalPos = ePred->CastPosition.Extended (x, -(Irelia::E.Range - Player.Distance (target)));
				if (finalPos.IsValid () && ePred->CastPosition.IsValid () && ePred->Hitchance >= HitChance(3 + Menu::Get<int>("combo.e.hitchance")))
				{
					if (Menu::Get<bool> ("combo.e.multi"))
						pIrelia->firstTarget = target;

					Irelia::E.Cast (&finalPos);
				}
			}

			if (Menu::Get<bool> ("combo.e.multi"))
			{
				if (Player.CountEnemiesInRange(850.f) >= 2 && pIrelia->firstTarget)
				{
					auto enemies = pSDK->EntityManager->GetEnemyHeroes (850.f);
					for (auto& enemy : enemies)
					{
						if (enemy.second->PTR () == pIrelia->firstTarget->PTR ())
							continue;

						if (Irelia::E.IsReady())
						{
							auto fastPred = pSDK->Prediction->GetFastPrediction (enemy.second, 0.25f, 2000.f);  auto x = Player.GetPosition ();
							auto castPos = fastPred.Extended (x, -(Irelia::E.Range - Player.Distance (target)));
							if (fastPred.IsValid () && castPos.IsValid ())
								Irelia::E.Cast (&castPos.To3D ());
						}
					}
					pIrelia->firstTarget = nullptr;
				}
				else
				{
					if (strcmp (Player.GetSpell (2).Name, "IreliaE2") == 0)
						Irelia::E.Cast (&Player.GetPosition ());
				}
			}
			else
			{
				if (strcmp (Player.GetSpell (2).Name, "IreliaE2") == 0)
					Irelia::E.Cast (&Player.GetPosition ());
			}
		}
	}

	static void Divine()
	{
		const auto target = pCore->TS->GetTarget (Irelia::E.Range - 50.f);
		if (Menu::Get<bool> ("combo.e"))
		{
			if (strcmp (Player.GetSpell (2).Name, "IreliaE") == 0 && Irelia::E.IsReady () && target)
			{
				if (Menu::Get<int> ("combo.e.first") == 0)
				{
					if (Renderer::MousePos ().IsValid () && !Renderer::MousePos ().IsWall ())
						Irelia::E.Cast (&Renderer::MousePos ());
				}

				if (Menu::Get<int> ("combo.e.first") == 1)
					Irelia::E.Cast (&Player.GetPosition ());
			}

			if (Menu::Get<int> ("combo.e.mode") == 0)
			{
				if (strcmp (Player.GetSpell (2).Name, "IreliaE2") == 0 && target)
				{
					auto fastPred = pSDK->Prediction->GetFastPrediction (target, 0.25f, 2000.f);  auto x = Player.GetPosition ();
					auto castPos = fastPred.Extended(x, -(Irelia::E.Range - Player.Distance (target)));
					if (fastPred.IsValid () && castPos.IsValid ())
						Irelia::E.Cast (&castPos.To3D());
				}
			}

			if (Menu::Get<int> ("combo.e.mode") == 1)
			{
				if (strcmp (Player.GetSpell (2).Name, "IreliaE2") == 0 && Irelia::E.IsReady () && target)
				{
					auto ePred = Irelia::E.GetPrediction (target); auto x = Player.GetPosition ();
					auto finalPos = ePred->CastPosition.Extended (x, -(Irelia::E.Range - Player.Distance (target)));
					if (finalPos.IsValid () && ePred->CastPosition.IsValid () && ePred->Hitchance >= HitChance::Medium)
						Irelia::E.Cast (&finalPos);
				}
			}
		}
	}
};

class VanguardsEdge
{
public:
	static float GetDamage ()
	{
		std::vector<float> damage = { 0.f, 125.f, 225.f, 325.f};
		return damage[Player.GetSpell (3).Level] + (Player.GetAbilityPower () * 0.8);
	}

	static void Cast()
	{
		const auto target = pCore->TS->GetTarget (Irelia::R.Range - 50.f);
		if (target && Irelia::R.IsReady())
		{
			if (Menu::Get<bool>("combo.r"))
			{
				if (Menu::Get<int> ("combo.r.amount") >= 1)
				{
					auto test = Irelia::R.GetPrediction (target);
					if (test->CastPosition.IsValid ())
					{
						auto PlayerPosition = Player.GetPosition ().To2D (); auto CastPosition = test->CastPosition.To2D ();
						const auto col = pSDK->Collision->GetCollisions (PlayerPosition, CastPosition, 120, 0.25f, 2000, false, CollisionFlags::Heroes);
						if (col->Units.size() >= Menu::Get<int> ("combo.r.amount") - 1)
							Irelia::R.Cast (&test->CastPosition);
					}
				}

				const auto damage = BladeSurge::GetDamage (false) * 2 + FlawlessDuet::GetDamage () + DefiantDance::GetDamage () + GetDamage () + Player.GetAttackDamage () * 2;
				if (target->IsAlive () && target->GetHealth ().Current <= Pred->PhysicalDamage (target, damage))
				{
					auto fastPred = Pred->FastPrediction (target, .25, Irelia::R.Range, 2000.f);
					if (fastPred.IsValid())
						Irelia::R.Cast (&fastPred);
				}
			}
		}
	}
};

class IreliaDamage
{
public:
	static float GetDamage ()
	{
		auto damage = 0.f;

		if (Irelia::Q.IsReady ())
			damage += BladeSurge::GetDamage (false);
		if (Irelia::E.IsReady ())
			damage += FlawlessDuet::GetDamage () + BladeSurge::GetDamage (false);
		if (Irelia::W.IsReady())
			damage += DefiantDance::GetDamage ();
		if (Irelia::R.IsReady ())
			damage += VanguardsEdge::GetDamage ();

		damage += IonianFervor::GetDamage () + Player.GetAttackDamage () * 4;
		
		return damage;
	}
};