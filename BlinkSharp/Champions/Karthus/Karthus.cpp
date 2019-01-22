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
		lay_waste();
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

///Your menu settings go here
void Karthus::DrawMenu(void * UserData) {
	Menu::Checkbox("Draw Q?", "Menu.DrawQ", true);
}

auto Karthus::get_circle_prediction(const float spell_range, const float cast_time) -> Vector3 {
	auto target = pCore->TS->GetTarget(900.f);
	//if (target->IsDashing()) {
	//	return target->NavInfo().EndPos;
	//}
	auto velocity = target->NavInfo().Velocity;
	//SdkGetAINavData(target, nullptr, nullptr, nullptr, nullptr, nullptr, PSDKVECTOR(&velocity), nullptr, nullptr, nullptr);
	velocity.y = 0;
	const auto orientation = velocity.Normalize();

	if (!velocity.IsValid())
	{
		return target->GetPosition();
	}

	auto result = target->GetPosition() + orientation * target->GetMovementSpeed() * cast_time;

	return result;
}

void Karthus::lay_waste() {
	auto target = pCore->TS->GetTarget(900.f);
	if (is_spell_up(0) && Player.Distance(target) <= 900.f)
	{
		SDK_SPELL Q = Player.GetSpell(0);
		
		auto predPos = Pred->BasePrediction(target, 900.f, 220.f, 1000.f, false);
		if (predPos.IsValid()) {
			SdkCastSpellLocalPlayer(nullptr, &predPos, 0, SPELL_CAST_START);
		}
	}
}

void Karthus::defile()
{
	auto target = pCore->TS->GetTarget(400.f);
	if (is_spell_up(2) && Player.Distance(target) <= 400.f && target->IsTargetable())
	{
		pSDK->Control->CastSpell(2, false);
	}
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

		if (enemy.second->GetHealth().Current < pSDK->DamageLib->CalculateMagicalDamage(&Player, enemy.second, GetRDamage()) && enemy.second->IsAlive() && CanNotify() && is_spell_up(3) && enemy.second->IsVisible())
		{
			Game::PrintChat("[" + std::to_string((int)(floor(Game::Time() / 60))) + ":" + std::to_string((int)Game::Time() % 60) + "] " + std::string(enemy.second->GetCharName()) + " killable with R.", CHAT_FLAG_TURQUOISE);
			//const char* killableMsg = enemy.second->GetCharName();
			//SdkDrawText(nullptr, &textPos, killableMsg, "Arial", &Color::Black, 15, 15, 15, false);
			//pSDK->Control->CastSpell(3, false);
			LastNotifyTick = GetTickCount();
		}
	}
}

auto Karthus::CalcDamage(AIHeroClient* enemy) -> float
{
	auto RDamage = GetRDamage();
	auto MResist = (enemy->GetMagicResist() * (100 - Player.GetPercentMagicPen())) - Player.GetFlatMagicPen();
	return RDamage * (100 / (100 + MResist));
}

auto Karthus::GetRDamage() -> float
{
	auto level = Player.GetSpell(3).Level;
	std::vector<float> RDamage = { 0.f, 250.f, 400.f, 550.f };
	return RDamage[level] + 0.75f * Player.GetAbilityPower();
}

std::unique_ptr<Karthus> pKarthus = std::make_unique<Karthus>();