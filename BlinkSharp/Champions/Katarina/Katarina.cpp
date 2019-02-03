#include "Katarina.h"
#include "../Karthus/Karthus.h"
#include <string>
#include "../../Include/Spell.hpp"
#include "../../Include/Menu.hpp"

AttackableUnit  Katarina::CurrentTarget;
AttackableUnit* Katarina::OrbTarget;
SDKCOLOR redd = { 0, 0, 255, 255 };
SDKCOLOR ColorPurple = { 5, 15, 0, 22 };
SDKVECTOR DirectionVector = { 0, 0, 1.f };
bool CastingR = false;

std::vector<SDagger> VDagger;

///This gets called once, at load
void Katarina::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Katarina::Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Katarina::Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, Katarina::DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Katarina::Draw);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::CreateObject, Katarina::OnCreateObject);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::DeleteObject, Katarina::OnDeleteObject);
#pragma endregion
}

///This gets called 30 times per second, put your logics in here instead of
///on OnUpdate so it doesnt drop too many fps
void Katarina::Tick(void * UserData) {
	EnemiesAroundR();
	if (Menu::Get<bool>("Menu.QKS")) { TriHard(); }
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
	{
		//if (Menu::Get<int>("Menu.ComboPick") == 0) {
			BaseCombo();
		//}
		//else {
			//andereCombo();
		//}
	}
	if (Player.HasBuff("katarinarsound", false, false)) { CastingR = true; }
	else { CastingR = false; }
}

///This gets called X times per second, where X is your league fps.
///Use this only to update stuff you need to validade on every frame.
void Katarina::Update(void * UserData) {
	
}

///This gets called X times per second, where X is your league fps.
///Put any drawings you need here
void Katarina::Draw(void * UserData) {
	for (auto& dagger : VDagger) {
		SdkDrawLine(&Player.GetPosition(), &dagger.Dagger->GetPosition(), 15, &redd, 0);
		SdkDrawCircle(&dagger.Dagger->GetPosition(), 150.f, &redd, 0, &DirectionVector);
	}
}

///Your menu settings go here
void Katarina::DrawMenu(void * UserData) {

	Menu::Tree("Combo Settings", "QMenu", false, [&]() {
		//Menu::DropList("Choose combo", "Menu.ComboPick", { "Default", "Idk" }, 0);
		Menu::Checkbox("Use R", "Menu.UseR", true);
	});
	Menu::Tree("Misc.", "Misc", false, [&]() {
		Menu::Checkbox("KS with Q", "Menu.QKS", true);
	});
}

void Katarina::Update_Dagger()
{
	std::vector<SDagger>::iterator it;
	for (it = VDagger.begin(); it < VDagger.end(); it++)
	{
		SdkDrawLine(&(it)->Dagger->GetPosition(), &Player.GetPosition(), 15, &redd, 1);
	}
}

void Katarina::OnCreateObject(void* Object, unsigned int NetworkID, void* UserData) {
	GameObject* potDagger = pSDK->EntityManager->GetObjectFromPTR(Object);
	
	if (strstr(potDagger->GetName(), "W_Indicator_Ally") || strstr(potDagger->GetName(), "W_Mis"))
	{
		SDagger data;
		data.Dagger = potDagger;
		data.netID = NetworkID;
		data.startTime = Game::Time();
		VDagger.push_back(data);
	}
}

void Katarina::OnDeleteObject(void* Object, unsigned int NetworkID, void* UserData) {
	
	std::vector<SDagger>::iterator it;
	for (it = VDagger.begin(); it < VDagger.end(); it++)
	{
		if ((it)->netID == NetworkID)
			VDagger.erase(it);
	}
}

auto Katarina::BestDagger(AIHeroClient* target) -> SDagger
{
	SDagger best;
	float minDist = INFINITY;
	for (auto& dagger : VDagger) {
		if (target->Distance(&dagger.Dagger->GetPosition()) < minDist) {
			minDist = target->Distance(&dagger.Dagger->GetPosition());
			best = dagger;
		}
	}
	return best;
}//:( xd

void Katarina::BaseCombo() { //combo1
	auto target = pCore->TS->GetTarget(725.f);
	if (Karthus::is_spell_up(2) && Player.Distance(target) <= 725.f && !CastingR) {
		if (VDagger.size() > 0) {
			for (auto& dagger : VDagger) {
				if (dagger.startTime <= Game::Time()) {
					auto daggerPos = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * 200;
					if (target->Distance(dagger.Dagger) <= 375.f) {
						pSDK->Control->CastSpell(2, &daggerPos, false);
					}
					else {
						if (Player.Distance(target) >= 375.f && !CastingR) {
							pSDK->Control->CastSpell(2, &target->GetPosition(), false);
						}
					}
				}
			}
		}
	}
	if (Karthus::is_spell_up(1) && Player.Distance(target) <= 375.f && !CastingR) {
		pSDK->Control->CastSpell(1, false);
	}
	if (Karthus::is_spell_up(1) && Player.Distance(target) <= 625.f && !CastingR) {
		pSDK->Control->CastSpell(0, target, false);
	}
	if (Player.Distance(target) <= 500.f && Menu::Get<bool>("Menu.UseR")) {
		if (Karthus::is_spell_up(3) && target->GetHealth().Current < Pred->MagicalDamage(target, GetRDamage()) && Player.Distance(target) <= 375.f && !CastingR) {
			pSDK->Control->CastSpell(3, false);
		}
	}
}

void Katarina::andereCombo() { // combo 3
	auto target = pCore->TS->GetTarget(725.f);
	if (Karthus::is_spell_up(0) && Player.Distance(target) <= 625.f && !CastingR) {
		pSDK->Control->CastSpell(0, target, false);
	}
	if (Karthus::is_spell_up(2) && Player.Distance(target) <= 725.f && !CastingR) {
		if (Player.Distance(target) >= 375.f && !CastingR && VDagger.size() == 0) {
			pSDK->Control->CastSpell(2, &target->GetPosition(), false);
		}
		else {
			for (auto& dagger : VDagger) {
				if (dagger.startTime <= Game::Time()) {
					auto daggerPos = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * 200;
					if (target->Distance(dagger.Dagger) <= 375.f) {
						pSDK->Control->CastSpell(2, &daggerPos, false);
					}
				}
			}
		}
	}
	if (Karthus::is_spell_up(1) && Player.Distance(target) <= 375.f && !CastingR) {
		pSDK->Control->CastSpell(1, false);
	}
	if (Player.Distance(target) <= 500.f && Menu::Get<bool>("Menu.UseR")) {
		if (Karthus::is_spell_up(3) && target->GetHealth().Current < Pred->MagicalDamage(target, GetRDamage()) && Player.Distance(target) <= 375.f && !CastingR) {
			pSDK->Control->CastSpell(3, false);
		}
	}
}
//crashes the game :^)
void Katarina::TriHard() {
	auto target = pCore->TS->GetTarget(700);
	if (Player.Distance(target) <= 625.f) {
		if (target->GetHealth().Current < Pred->MagicalDamage(target, GetQDamage())) {
			if (Player.Distance(target) <= 625.f && !CastingR) {
				pSDK->Control->CastSpell(0, target, false);
			}
		}
	}
}

//donut use yet 
void Katarina::CastE(AIHeroClient* target) {
	for (auto& dagger : VDagger) {
		if (dagger.startTime <= Game::Time()) {
			auto daggerPos = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * 150;
			auto daggerIsRange = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * 50;
			auto daggerRange = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * -50;
			auto daggerPos2 = dagger.Dagger->GetPosition() + (target->GetPosition() - dagger.Dagger->GetPosition()).Normalize() * -150;
			if (target->Distance(dagger.Dagger) <= 450.f) {
				//port this
			}
		}
	}
}

auto Katarina::GetBestDaggerPoint(SDagger position, AIHeroClient* target) -> Vector3
{
	auto targetPos = target->GetPosition();
	auto posPos = position.Dagger->GetPosition();
	if (targetPos.DistanceSqr(posPos) <= 340 * 340) {
		return posPos;
	}
	return posPos.Extended(targetPos, 150.f);
}

void Katarina::EnemiesAroundR() {
	if (CastingR && pSDK->EntityManager->GetEnemyHeroes(560.f).size() == 0) {
		SDKPOINT Pos;
		SdkGetMouseScreenPosition(&Pos);
		SdkMoveLocalPlayer(&Pos.To3D(), false);
	}
}

auto Katarina::GetQDamage() -> float 
{
	auto level = Player.GetSpell(0).Level;
	std::vector<float> QDamage = { 0.f, 75.f, 105.f, 135.f, 165.f, 195.f };
	if (Karthus::is_spell_up(0)) {
		return QDamage[level] + 0.3f * Player.GetAbilityPower();
	}
	else {
		return 0.f;
	}
}

auto Katarina::GetRDamage() -> float
{
	auto level = Player.GetSpell(3).Level;
	std::vector<float> RDamage = { 0.f, 375.f, 562.5f, 750.f };
	return RDamage[level] + 2.85f * Player.GetAbilityPower() + 3.3f * Player.GetBonusAttackDamage();
}