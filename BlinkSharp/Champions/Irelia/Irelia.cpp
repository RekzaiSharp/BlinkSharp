#include "Irelia.h"
#include "../../Core/Misc/DamageLib.hpp"
#include "Modes/Irelia.Combo.hpp"
#include "Modes/Irelia.Clear.hpp"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR purple = { 255, 0, 255, 255 };
SDKCOLOR scf = { 255, 123, 255, 255 };
SDKVECTOR dir = { 0, 0, 1.f };

AIHeroClient*  Irelia::CurrentTarget;
AttackableUnit* Irelia::OrbTarget;

Spell::Targeted Irelia::Q(SpellSlot::Q, 600, DamageType::Physical);
Spell::Active Irelia::W(SpellSlot::W, 825, DamageType::Physical);
Spell::Skillshot Irelia::E(SpellSlot::E, 850.f, SkillshotType::Line, 0.45f, 2000.f, 60.f, DamageType::Physical, false, CollisionFlags::YasuoWall);
Spell::Skillshot Irelia::R(SpellSlot::R, 975.f, SkillshotType::Line, 0.25f, 2000.f, 200.f, DamageType::Magical, true, CollisionFlags::Heroes);

void Irelia::Init () {
	CurrentTarget = nullptr;
	OrbTarget = NULL;
	Game::PrintChat(R"([RiftSharp] <font color="#00ffdc"><b>Divine Irelia Reborn</b></font> Loaded.)");
	
#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Draw);
#pragma endregion
}

void Irelia::Tick(void * UserData) {
	if (!Game::IsChatOpen () && !Game::IsOverlayOpen () && GetActiveWindow () == GetForegroundWindow ())
	{
		if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::LaneClear))
			IreliaClear::LaneClear ();

		if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::JungleClear))
			IreliaClear::JungleClear ();

		if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Mixed))
			IreliaCombo::SmartTrade ();


		if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Flee))
			Game::PrintChat(std::to_string(E.Delay));
	}
}

void Irelia::Update(void * UserData) 
{
	if (!Game::IsChatOpen () && !Game::IsOverlayOpen () && GetActiveWindow () == GetForegroundWindow ())
	{
		if (pCore->Orbwalker->IsModeActive (OrbwalkingMode::Combo))
			IreliaCombo::Tick ();

		if (Menu::Get<Hotkey> ("R-Key").Active && R.IsReady () && Irelia::R.LastCastTick + 1000 < GetTickCount ())
		{
			const auto target = pCore->TS->GetTarget (R.Range - 50.f);
			if (target)
			{
				auto pred = R.GetPrediction (target);
				if (pred->Hitchance >= HitChance::Medium && pred->CastPosition.IsValid ())
					R.Cast (&pred->CastPosition);
			}
		}
	}
}


void Irelia::Draw(void * UserData) {
	if (Menu::Get<bool>("draw.q")  && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange, &scf, 0, &dir);

	if (Menu::Get<bool> ("draw.q.extended") && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange * 2, &scf, 0, &dir);

	if (Menu::Get<bool> ("draw.e") && pIrelia->E.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(2).CastRange, &scf, 0, &dir);

	if (Menu::Get<bool>("draw.minion.q"))
	{
		auto minions = pSDK->EntityManager->GetEnemyMinions (600);
		for (auto& minion : minions)
		{
			if (minion.second->GetHealth().Current <= Pred->PhysicalDamage (minion.second, BladeSurge::GetDamage(true)) && minion.second->GetHealth().Current > 1)
				SdkDrawCircle (&minion.second->GetPosition(), minion.second->GetBoundingRadius() + 15.f, &Color::White, 0, &dir);
		}
	}

	if (Menu::Get<bool>("draw.hero.q"))
	{
		auto enemies = pSDK->EntityManager->GetEnemyHeroes (1000.f);
		for (auto& enemy : enemies)
		{
			if (enemy.second->IsVisibleOnScreen())
				DamageLib::DrawDamage (enemy.second, Pred->PhysicalDamage (enemy.second, IreliaDamage::GetDamage()));
		}
	}

	if (Menu::Get<bool>("draw.passive"))
	{
		if (Player.GetBuffCount("ireliapassivestacks") == 5)
		{
			Draw::Circle (&Player.GetPosition (), (Player.GetBuff ("ireliapassivestacks").EndTime - Game::Time ()) * 100.f, &Color::Orange, 0, &Vector3::DirectionVector);
		}
	}
}

bool Irelia::OnCreate(void* Object, unsigned int NetworkID, void* UserData)
{
	auto blade = pSDK->EntityManager->GetObjectFromPTR(Object);

	if (blade == nullptr)
		return true;
	if (!blade->IsValid())
		return true;

	if (std::string (blade->GetName ()).find ("_E_Blades") == std::string::npos)
		return true;

	if (pIrelia->active_blade.net_id == NULL)
	{
		pIrelia->active_blade.net_id = NetworkID;
		pIrelia->active_blade.creationTime = GetTickCount ();
		pIrelia->active_blade.position = blade->GetPosition();
	}

	return true;
}

void Irelia::OnDelete(void* Object, unsigned int NetworkID, void* UserData)
{
	if (NetworkID == pIrelia->active_blade.net_id)
	{
		pIrelia->active_blade.net_id = NULL;
		pIrelia->active_blade.creationTime = NULL;
		pIrelia->active_blade.position = Vector3(0, 0, 0);
	}
}


void Irelia::DrawMenu (void * UserData) {

	bool visible (true); bool collapsed (true);
	SdkUiBeginWindow ("[RiftSharp] Divine Irelia", &visible, &collapsed);
	if (collapsed && visible)
	{
		bool q_open (true);
		SdkUiBeginTree ("[Q] - Bladesurge", &q_open);
		if (q_open)
		{
			Menu::Checkbox ("Use Q", "combo.q", true);
			Menu::DropList ("Q Mode", "combo.q.mode", std::vector<std::string> ({ "Smart", "Marked" }));
			Menu::Checkbox ("Q Killsteal", "combo.q.ks", true);
			Menu::Checkbox ("Use Q to Clear", "clear.q", true);
			Menu::Checkbox ("Stack before Engage", "combo.q.stack", true);
			Menu::Checkbox ("Dont Q under Turrets", "combo.q.turret", true);
			Menu::Checkbox ("Q Every Mark", "combo.q.spam", true);
			Menu::SliderFloat ("Mana % Clear", "clear.q.mana", 20.f, 1.f, 100.f, "%.0f");
			SdkUiEndTree ();
		}

		bool w_open (true);
		SdkUiBeginTree ("[W] - Defiant Dance", &w_open);
		if (w_open)
		{			
			Menu::Checkbox ("Use W", "combo.w", true);
			Menu::DropList ("W Mode", "combo.w.mode", std::vector<std::string> ({ "Offensive", "Manual" }));
			SdkUiEndTree ();
		}

		bool e_open (true);
		SdkUiBeginTree ("[E] - Flawless Duet", &e_open);
		if (e_open)
		{
			Menu::Checkbox ("Use E", "combo.e", true);
			Menu::DropList ("Prediction", "combo.e.prediction", std::vector<std::string> ({ "Reborn", "Divine (old)" }), 1);

			if (Menu::Get<int>("combo.e.prediction") == 0)
			{
				Menu::DropList ("Min. Hitchance", "combo.e.hitchance", std::vector<std::string> ({ "Medium", "High", "Very High" }), 0);
				Menu::Checkbox ("Hit Multiple (Experimental)", "combo.e.multi", false);
			}
			else
			{
				Menu::DropList ("First E", "combo.e.first", std::vector<std::string> ({ "Mouse", "Under Player", "Manual" }), 1);
				Menu::DropList ("Mode", "combo.e.mode", std::vector<std::string> ({ "Fast", "Precise"}), 1);
			}

			SdkUiEndTree ();
		}

		bool r_open (true);
		SdkUiBeginTree ("[R] - Vanguard's Edge", &r_open);
		if (r_open)
		{
			Menu::Checkbox ("Auto R", "combo.r", true);
			Menu::SliderInt ("Min. Enemies", "combo.r.amount", 2, 1, 5);
			Menu::Checkbox ("Killable with Combo", "combo.r.killable", true);
			Menu::Hotkey ("Assisted R Key", "R-Key", 84);
			SdkUiEndTree ();
		}

		bool d_open (true);
		SdkUiBeginTree ("[Misc] - Drawings", &d_open);
		if (d_open)
		{
			Menu::Checkbox ("Draw Q", "draw.q", true);
			Menu::Checkbox ("Draw Gapcloser Q", "draw.q.extended", true);
			Menu::Checkbox ("Draw E", "draw.e", true);
			Menu::Checkbox ("Draw Q Minions", "draw.minion.q", true);
			Menu::Checkbox ("Draw Damage", "draw.hero.q", true);
			Menu::Checkbox ("Draw Passive", "draw.passive", true); 
			SdkUiEndTree ();
		}
		SdkUiEndWindow ();
	}
}

std::unique_ptr<Irelia> pIrelia = std::make_unique<Irelia> ();
