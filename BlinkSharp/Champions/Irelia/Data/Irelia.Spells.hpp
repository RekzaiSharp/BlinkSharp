#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class IonianFervor
{
public:
	static float GetDamage ()
	{
		auto num = 0;

		if (Player.GetBuffCount("ireliapassivestacks") == 5)
		{
			num += 12 + 3 * Player.GetLevel () + Player.GetBonusAttackDamage () * 0.25;
		}

		if (Player.HasItem((int)ItemID::TitanicHydra))
			num += Player.HasBuff ("itemtitanichydracleavebuff") ? (40.f + 0.1f * Player.GetHealth ().Max) : (5.f + 0.01f * Player.GetHealth ().Max);

		if (Player.HasItem ((int)ItemID::TrinityForce) && Player.HasBuff ("TrinityForce", false)) {
			num += 2.f * Player.GetBaseAttackDamage ();
		}

		return num;
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
		if (Menu::Get<bool>("combo.q") && Irelia::Q.IsReady() && Irelia::Q.LastCastTick + 350 <= GetTickCount())
		{
			const auto target = pCore->TS->GetTarget (Irelia::Q.Range);

			if (target)
			{
				if (Menu::Get<bool> ("combo.q.ks")) // Execute
				{
					auto enemies = pSDK->EntityManager->GetEnemyHeroes (Irelia::Q.Range);
					for (auto& enemy : enemies)
					{
						if (enemy.second->Distance (target) <= Irelia::E.Range && enemy.second->GetHealth ().Current + enemy.second->GetHealth ().PhysicalShield + 10 <=
							pSDK->DamageLib->CalculatePhysicalDamage(&Player, enemy.second, GetDamage(false)))
						{
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
					if (Player.Distance (target) >= 500.f && Player.Distance (target) <= 600 && target->GetHealth ().Current + target->GetHealth ().PhysicalShield + 10 <= Pred->PhysicalDamage (target, GetDamage (false) + Player.GetAttackDamage () * 2))
					{
						Irelia::Q.Cast (target);
					}
						

					if (target->HasBuff ("ireliamark", false))
					{
						Irelia::Q.Cast (target);
					}
				}

				if (Menu::Get<int> ("combo.q.mode") == 1) // Marked Only
				{
					if (target->HasBuff ("ireliamark", false))
					{
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
		const auto target = pCore->TS->GetTarget (Irelia::E.Range);
		if (Menu::Get<bool> ("combo.e") && target)
		{
			if (strcmp (Player.GetSpell (2).Name, "IreliaE") == 0 && Irelia::E.IsReady ())
			{
				Vector3 x = Player.GetPosition ();
				const float distanceLeft = Irelia::E.Range - Player.Distance (target);
				const auto AngleToPlayer = target->GetPosition ().AngleBetween (x);

				if (target->IsFacing(&Player) && distanceLeft > 50.f)
				{
					auto dirPos = target->GetDirection ().Rotated (-180);
					auto castPos = target->GetPosition ().Extended (dirPos, distanceLeft);
					if (castPos.IsValid () && !castPos.IsWall ())
					{
						pIrelia->firstPos = castPos;
						Irelia::E.Cast (&castPos);
					}
				}

				auto minSize = 0;
				auto minions = pSDK->EntityManager->GetEnemyMinions (Irelia::Q.Range);
				for (auto& minion : minions) { if (minion.second->GetHealth ().Current <= BladeSurge::GetDamage (true)) { minSize++;}};
				if (!target->IsFacing (&Player) && distanceLeft < 50.f && minSize == 0)
					return;

				if (!target->IsFacing (&Player) && distanceLeft < 50.f && minSize > 0 && AngleToPlayer > 60.f && AngleToPlayer < -60.f)
				{
					auto dirPos = target->GetDirection ().Rotated (-180);
					auto y = target->NavInfo ().StartPos;
					if (target->NavInfo().EndPos.Distance(y) > 50.f || target->NavInfo().NumberOfWaypoints <= 1)
					{
						const auto tDistance = Player.Distance (target);
						auto castPos = target->GetPosition ().Extended (dirPos, tDistance);
						if (castPos.IsValid ())
						{
							pIrelia->firstPos = castPos;
							Irelia::E.Cast (&castPos);
						}
					}
				}

				if (!target->IsFacing (&Player) && distanceLeft > 50.f && minSize > 0 && AngleToPlayer > 60.f && AngleToPlayer < -60.f)
				{
					auto dirPos = target->GetDirection ().Rotated (-180);
					auto castPos = target->GetPosition ().Extended (dirPos, distanceLeft);
					if (castPos.IsValid () && !castPos.IsWall ())
					{
						pIrelia->firstPos = castPos;
						Irelia::E.Cast (&castPos);
					}
				}
			}

			if (strcmp (Player.GetSpell (2).Name, "IreliaE2") == 0)
			{
				auto x = Player.GetPosition ();
				const float distanceLeft = Irelia::E.Range - Player.Distance (target);
				if (distanceLeft >= 50.f)
				{
					auto ePred = Irelia::E.GetPrediction (target);
					if (ePred->Hitchance >= HitChance (3 + Menu::Get<bool> ("combo.e.hitchance")) && ePred->CastPosition.IsValid ())
					{
						auto y = target->GetPosition (); auto totalDistance = pIrelia->firstPos.Distance (y) + distanceLeft;
						auto finalPos = pIrelia->firstPos.Extended (ePred->CastPosition, totalDistance);
						if (finalPos.IsValid ())
						{
							pIrelia->firstPos = Vector3 (0, 0, 0);
							Irelia::E.Cast (&finalPos);
						}
					}
				}
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
					if (finalPos.IsValid () && ePred->CastPosition.IsValid () && ePred->Hitchance >= HitChance::High)
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
		if (target && Irelia::R.IsReady() && Irelia::R.LastCastTick + 1000 < GetTickCount())
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
						if (col->Units.size() >= Menu::Get<int> ("combo.r.amount"))
							Irelia::R.Cast (&test->CastPosition);
					}
				}

				const auto damage = BladeSurge::GetDamage (false) * 2 + FlawlessDuet::GetDamage () + DefiantDance::GetDamage () + GetDamage () + Player.GetAttackDamage () * 2;
				if (target->IsAlive () && target->GetHealth ().Current <= Pred->PhysicalDamage (target, damage) && target->GetHealth().Current > BladeSurge::GetDamage(false) + Player.GetAttackDamage() * 2)
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