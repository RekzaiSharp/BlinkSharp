#include "Karthus.h"
#include <string>
#include "../../Include/Spell.hpp"
#include "../../Include/Menu.hpp"

AttackableUnit  Karthus::CurrentTarget;
AttackableUnit* Karthus::OrbTarget;
SDKCOLOR _g_ColorPurple;
SDKVECTOR _g_DirectionVector;
DWORD LastNotifyTick;
SDKPOINT textPos;

///This gets called once, at load
void Karthus::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;
	_g_ColorPurple = { 5, 15, 0, 22 };
	_g_DirectionVector = { 0, 0, 1.f };
	LastNotifyTick = GetTickCount();
	textPos = new SDKPOINT(800.f, 800.f);

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Karthus::Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Karthus::Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, Karthus::DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Karthus::Draw);
#pragma endregion
}

///This gets called 30 times per second, put your logics in here instead of
///on OnUpdate so it doesnt drop too many fps
void Karthus::Tick(void * UserData) {
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
	{
		if (Menu::Get<bool>("Menu.UseQ")) { lay_waste(); }
		if (Menu::Get<bool>("Menu.UseE")) { defile(); }
		if (Menu::Get<bool>("Menu.UseW")) { wall(); }
	}
	else if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::LaneClear)) {
		if (Menu::Get<bool>("Menu.FarmQ")) { LaneClear(); }
		//JungleClear();
	}
	checkKillable();
	
}

///This gets called X times per second, where X is your league fps.
///Use this only to update stuff you need to validade on every frame.
void Karthus::Update(void * UserData) {

}

///This gets called X times per second, where X is your league fps.
///Put any drawings you need here
void Karthus::Draw(void * UserData) {
	if (Menu::Get<bool>("Menu.DrawQ")) {
		SdkDrawCircle(&Player.GetPosition(), 875.f, &_g_ColorPurple, 0, &_g_DirectionVector);
	}
}

//-----------

///Your menu settings go here
void Karthus::DrawMenu(void * UserData) {
	Menu::Tree("Q Settings", "QMenu", false, [&]() {
		Menu::Checkbox("Draw Q", "Menu.DrawQ", true);
		Menu::Checkbox("Use Q in Combo", "Menu.UseQ", true);
		Menu::Checkbox("Use Q in LaneClear (WIP)", "Menu.FarmQ", false);
	});
	Menu::Tree("W Settings", "WMenu", false, [&]() {
		Menu::Checkbox("Use W in Combo", "Menu.UseW", true);
	});
	Menu::Tree("E Settings", "EMenu", false, [&]() {
		Menu::Checkbox("Use E in Combo", "Menu.UseE", true);
		Menu::SliderFloat("E Min Mana Percent", "EMana", 40.f, 0.f, 100.f, "%.3f");
	});
	Menu::Tree("R Settings", "RMenu", false, [&]() {
		Menu::Checkbox("Notify Killable with R", "Menu.NotifyR", true);
	});
}

void Karthus::lay_waste() {
	auto target = pCore->TS->GetTarget(875.f);
	if (is_spell_up(0) && Player.Distance(target) <= 875.f)
	{
		auto predPos = Pred->CircularPrediction(target, 0.675f, 875.f, 200.f);
		if (predPos.IsValid()) {
			SdkCastSpellLocalPlayer(nullptr, &predPos, 0, SPELL_CAST_START);
		}
	}
}

void Karthus::defile()
{
	auto target = pCore->TS->GetTarget(400.f);
	auto E = Player.GetSpell(2);
	if (is_spell_up(2) && Player.Distance(target) <= 400.f && target->IsTargetable() && E.ToggleState == 1 && Player.GetManaPercent() > Menu::Get<float>("EMana"))
	{
		pSDK->Control->CastSpell(2, false);
	}
	else if (E.ToggleState == 2 && Player.Distance(target) > 400.f || E.ToggleState == 2 && Player.GetManaPercent() < Menu::Get<float>("EMana")) {
		pSDK->Control->CastSpell(2, false);
	}
}

void Karthus::wall()
{
	auto target = pCore->TS->GetTarget(900.f);
	if (is_spell_up(1) && Player.Distance(target) <= 900.f)
	{
		auto predPos = Pred->CircularPrediction(target, 0.7f, 900.f, 50.f);
		if (predPos.IsValid()) {
			SdkCastSpellLocalPlayer(nullptr, &predPos, 1, SPELL_CAST_START);
		}
	}
}

void Karthus::LaneClear() 
{
	auto minions = pSDK->EntityManager->GetEnemyMinions(900.f);

	for (auto& minion : minions) {
		if (is_spell_up(0) && pSDK->HealthPred->GetHealthPrediction(minion.second, 15, false) < GetQDamage() && pSDK->HealthPred->GetHealthPrediction(minion.second, 15, false) > 0.f) {
			pSDK->Control->CastSpell(0, minion.second, false);
		}
	}
}

void Karthus::JungleClear() 
{
	auto monsters = pSDK->EntityManager->GetJungleMonsters(900.f);

	for (auto& monster : monsters) {
		if (is_spell_up(0)) {
			pSDK->Control->CastSpell(0, monster.second, false);
		}
	}
}

float Karthus::GetQDamage() {
	auto level = Player.GetSpell(0).Level;
	std::vector<float> QDamage = { 0.f, 50.f, 70.f, 90.f, 110.f, 130.f };
	return QDamage[level] + 0.3f * Player.GetAbilityPower();
}

bool Karthus::is_spell_up(unsigned char Slot) {
	int SpellState;
	SdkCanAICastSpell(Player.PTR(), Slot, false, &SpellState);
	return SpellState == 0;
}

auto Karthus::CanNotify() -> bool
{
	return LastNotifyTick + 1000 < GetTickCount();
}

auto Karthus::checkKillable() -> void
{
	auto enemies = pSDK->EntityManager->GetEnemyHeroes();

	for (auto& enemy : enemies)
	{

		if (enemy.second->GetHealth().Current < Pred->MagicalDamage(enemy.second, GetRDamage()) && enemy.second->IsAlive() && CanNotify() && is_spell_up(3) && enemy.second->IsVisible())
		{
			if (Menu::Get<bool>("Menu.NotifyR")) {
				Game::PrintChat("[" + std::to_string((int)(floor(Game::Time() / 60))) + ":" + std::to_string((int)Game::Time() % 60) + "] " + std::string(enemy.second->GetCharName()) + " killable with R.", CHAT_FLAG_TURQUOISE);
			}
			LastNotifyTick = GetTickCount();
		}
	}
}

auto Karthus::GetRDamage() -> float
{
	auto level = Player.GetSpell(3).Level;
	std::vector<float> RDamage = { 0.f, 250.f, 400.f, 550.f };
	return RDamage[level] + 0.75f * Player.GetAbilityPower();
}

std::unique_ptr<Karthus> pKarthus = std::make_unique<Karthus>();