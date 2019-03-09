#include "Kassadin.h"
#include "Data/Kassadin.Spells.hpp"
#include "../../Core/Misc/DamageLib.hpp"

Spell::Targeted Kassadin::Q = {SpellSlot::Q, 650.f, DamageType::Magical};
Spell::Active Kassadin::W = { SpellSlot::W, 50.f, DamageType::Magical };
Spell::Skillshot Kassadin::E = {SpellSlot::E, 400, SkillshotType::Cone, 0.25f};
Spell::Skillshot Kassadin::R = {SpellSlot::R, 500.f, SkillshotType::Circle, 0.25f};

void Kassadin::Init()
{
	Game::PrintChat (R"([RiftSharp] <font color="#00ffdc"><b>Demon Kassadin</b></font> Loaded.)");

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Tick, Tick);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Overlay, DrawMenu);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Update, Draw);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::PostAttack, PostAttack);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Dash, OnDash);
	#pragma endregion
}

void Kassadin::Tick(void* UserData)
{
	if (!Game::IsChatOpen () && !Game::IsOverlayOpen () && GetActiveWindow () == GetForegroundWindow ())
	{
		if (Q.IsReady())
			NullSphere::CastOffensive ();

		if (E.IsReady())
			ForcePulse::Cast ();

		if (R.IsReady())
		Riftwalk::Cast ();
	}
}

void Kassadin::Draw(void* UserData)
{
	if (Q.IsReady () && Menu::Get<bool> ("draw.q"))
		Draw::Circle (&Player.GetPosition (), Q.Range, &Color::White);

	if (E.IsReady () && Menu::Get<bool> ("draw.e"))
		Draw::Circle (&Player.GetPosition (), E.Range, &Color::White);

	if (R.IsReady () && Menu::Get<bool> ("draw.r"))
		Draw::Circle (&Player.GetPosition (), R.Range, &Color::White);

	if (Menu::Get<bool>("draw.damage"))
	{
		auto enemies = pSDK->EntityManager->GetEnemyHeroes (1000.f);
		for (auto& enemy : enemies)
		{
			if (enemy.second->IsValid () && enemy.second->IsVisible () && enemy.second->IsAlive ())
				DamageLib::DrawDamage (enemy.second, Pred->MagicalDamage (enemy.second, KassadinDamage::TotalDamage ()));
		}
	}
}

void Kassadin::DrawMenu(void* UserData)
{
	bool q_open (true);
	SdkUiBeginTree ("[Q] - Null Sphere", &q_open);
	if (q_open)
	{
		Menu::Checkbox ("Use Q", "combo.q", true);
		Menu::Checkbox ("Use Q Killsteal", "killsteal.q", true);
		Menu::Checkbox ("Interrupt Spells", "combo.q.interrupt", true);
		Menu::DropList ("Q Logic", "combo.q.logic", std::vector<std::string> ({ "Smart", "Always" }), 0);
		SdkUiEndTree ();
	}

	bool w_open (true);
	SdkUiBeginTree ("[W] - Nether Blade", &w_open);
	if (w_open)
	{
		Menu::Checkbox ("Use W", "combo.w", true);
		Menu::Checkbox ("Use W Killsteal", "killsteal.w", true);
		SdkUiEndTree ();
	}

	bool e_open (true);
	SdkUiBeginTree ("[E] - Force Pulse", &e_open);
	if (e_open)
	{
		Menu::Checkbox ("Use E", "combo.e", true);
		Menu::Checkbox ("On Gapclosers", "combo.e.gap", true);
		Menu::Checkbox ("Use E Killsteal", "killsteal.e", true);
		SdkUiEndTree ();
	}

	bool r_open (true);
	SdkUiBeginTree ("[R] - Riftwalk", &r_open);
	if (r_open)
	{
		Menu::DropList ("Safety Check", "combo.r.safety", std::vector<std::string> ({ "Smart", "Disabled" }), 0);
		Menu::Checkbox ("Use R", "combo.r", true);
		SdkUiEndTree ();
	}

	bool c_open (true);
	SdkUiBeginTree ("[Misc] - Clear", &c_open);
	if (c_open)
	{
		Menu::Checkbox ("Use Q", "clear.q", true);
		Menu::Checkbox ("Use W", "clear.w", true);
		Menu::Checkbox ("Use E", "clear.e", true);
		Menu::SliderFloat ("Min. Mana%", "clear.mana", 20.f, 0.f, 100.f, "%.0f");
		SdkUiEndTree ();
	}

	bool h_open (true);
	SdkUiBeginTree ("[Misc] - Harass", &h_open);
	if (h_open)
	{
		Menu::Checkbox ("Use Q", "harass.q", true);
		Menu::Checkbox ("Use W", "harass.w", true);
		Menu::Checkbox ("Use E", "harass.e", true);
		SdkUiEndTree ();
	}

	bool f_open (true);
	SdkUiBeginTree ("[Misc] - Flee", &f_open);
	if (f_open)
	{
		Menu::BulletText ("Stacking for E");
		Menu::Checkbox ("Use Q", "flee.q", true);
		Menu::Checkbox ("Use E", "flee.e", true);
		Menu::Checkbox ("Use R", "flee.e", true);
		SdkUiEndTree ();
	}

	bool d_open (true);
	SdkUiBeginTree ("[Misc] - Drawings", &d_open);
	if (d_open)
	{
		Menu::Checkbox ("Draw Q", "draw.q", true);
		Menu::Checkbox ("Draw E", "draw.e", true);
		Menu::Checkbox ("Draw R", "draw.r", true);
		Menu::Checkbox ("Draw Damage", "draw.damage", true);
	}
}

void Kassadin::PostAttack(AttackableUnit* Target)
{
	if (Target)
	{
		if (Menu::Get<bool> ("combo.w"))
		{
			if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo))
				if (Menu::Get<bool> ("combo.w") && W.IsReady () && Target->IsValid () && Player.Distance (Target) <= Player.GetTrueAttackRange () && Target->IsHero ())
					W.Cast ();
		}

		if (Menu::Get<bool> ("clear.w") && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
		{
			if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::LaneClear))
				if (Target->IsValid () && Player.Distance (Target) <= Player.GetTrueAttackRange () && Target->IsMinion ())
				{
					auto t = Target->AsAIBaseClient ();
					if (t->GetHealth ().Current <= Pred->MagicalDamage (t, NetherBlade::GetDamage () + Player.GetAttackDamage ()))
						W.Cast ();
				}
		}

		if (Menu::Get<bool> ("clear.w") && Player.GetManaPercent () > Menu::Get<float> ("clear.mana"))
		{
			if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::JungleClear))
				if (Target->IsValid () && Player.Distance (Target) <= Player.GetTrueAttackRange () && Target->IsMonster ())
				{
					auto t = Target->AsAIBaseClient ();
					if (t->GetHealth ().Current > 10.f)
						W.Cast ();
				}
		}
	}
}

void Kassadin::ProcessSpell (void* AI, PSDK_SPELL_CAST SpellCast, void* UserData)
{
	if (AI != Player.PTR ())
		return;

}

void Kassadin::OnDash(AIHeroClient* Source, PSDKVECTOR StartPos, PSDKVECTOR EndPos, unsigned StartTick,
	unsigned Duration, float Speed)
{
	if (Source->IsAlly ())
		return;

	if (Source->PTR () == Player.PTR ())
		return;

	if (Menu::Get<bool>("combo.e.gap") && E.IsReady())
	{
		if (Player.Distance (StartPos) > Player.Distance (EndPos) && Player.Distance (EndPos) <= E.Range && Player.Distance(Source) <= E.Range)
			E.Cast (&Source->GetPosition ());
	}
}


extern std::unique_ptr<Kassadin> pKassadin = std::make_unique<Kassadin> ();
