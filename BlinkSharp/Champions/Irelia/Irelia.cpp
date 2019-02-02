#include "Irelia.h"
#include "Modes/Irelia_Combo.h"
#include "Modes/Irelia_Clear.hpp"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR purple = { 255, 0, 255, 255 };
SDKCOLOR scf = { 255, 123, 255, 255 };
SDKVECTOR dir = { 0, 0, 1.f };

AttackableUnit  Irelia::CurrentTarget;
AttackableUnit* Irelia::OrbTarget;

Spell::Targeted Irelia::Q(SpellSlot::Q, 600, DamageType::Physical);
Spell::Active Irelia::W(SpellSlot::W, 825, DamageType::Physical);
Spell::Skillshot Irelia::E(SpellSlot::E, 850, SkillshotType::Line);
Spell::Skillshot Irelia::R(SpellSlot::R, 1000, SkillshotType::Cone);

void Irelia::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;
	Game::PrintChat(R"([RiftSharp] <font color="#00ffdc"><b>Irelia</b></font> Loaded.)");
	
#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Draw);
	SdkRegisterOnAIProcessSpell (OnProcessSpell, nullptr);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::CreateObject, OnCreate);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::DeleteObject, OnDelete);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Dash, OnDelete);
#pragma endregion
/*
#pragma region PredictionCallbacks
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Update, Prediction::PathingObserver::OnUpdate);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::NewPath, Prediction::PathingObserver::OnMove);
	pSDK->EventHandler->RegisterCallback (CallbackEnum::Attack, Prediction::PathingObserver::OnAttack);
#pragma endregion*/
}

void Irelia::Tick(void * UserData) {
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
		pIreliaCombo->Combo_Tick();

	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::LaneClear))
		pIreliaClear->Clear_Tick();

}

void Irelia::Update(void * UserData) {
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::LaneClear))
		pIreliaClear->Draw_Clear();
}

void Irelia::Draw(void * UserData) {
	if (pIrelia->drw_q  && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange, &scf, 0, &dir);

	if (pIrelia->drw_q_e && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange * 2, &scf, 0, &dir);

	if (pIrelia->draw_e && pIrelia->E.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(2).CastRange, &scf, 0, &dir);

	if (pIrelia->msc_draw_blade && pIrelia->active_blade.net_id)
		SdkDrawCircle(&pIrelia->active_blade.position, 10, &scf, 0, &dir);
}

void Irelia::OnCreate(void* Object, unsigned int NetworkID, void* UserData)
{
	auto blade = pSDK->EntityManager->GetObjectFromPTR(Object);

	if (blade == nullptr)
		return;
	if (!blade->IsValid())
		return;

	const auto blade_name = Player.GetSkinID() > 0 ? "Irelia_Skin0" + std::to_string(Player.GetSkinID()) + "_E_Blades" : "Irelia_Base_E_Blades";
	if (!Common::CompareLower(blade->GetName(), blade_name))
		return;

	if (pIrelia->active_blade.net_id == NULL)
	{
		pIrelia->active_blade.net_id = NetworkID;
		pIrelia->active_blade.position = blade->GetPosition();
	}
}

void Irelia::OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData)
{
	if (SpellCast->TargetObject == Player.PTR() && pIrelia->W.IsReady() && pIrelia->w_mode == 0)
	{
		if ( std::find(pIrelia->wSpells.begin(), pIrelia->wSpells.end(), SpellCast->Spell.ScriptName) != pIrelia->wSpells.end())
		{
			pSDK->Control->CastSpell (1, false);
			
		}
	}
}

void Irelia::OnDelete(void* Object, unsigned int NetworkID, void* UserData)
{
	if (NetworkID == pIrelia->active_blade.net_id)
	{
		pIrelia->active_blade.net_id = NULL;
		pIrelia->active_blade.position = Vector3(0, 0, 0);
	}
}

void Irelia::OnDash(AIHeroClient* Source, PSDKVECTOR StartPos, PSDKVECTOR EndPos, unsigned StartTick, unsigned Duration,
	float Speed)
{
	if (Source->PTR() == Player.PTR())
	{
		pIrelia->inDash = true;
		pSDK->EventHandler->DelayedAction([&] {pIrelia->inDash = false; }, Duration);
	}
}

void Irelia::DrawMenu(void * UserData) {
	bool visible(true); bool collapsed(true);
	SdkUiBeginWindow("[RiftSharp] Irelia", &visible, &collapsed);
	if (collapsed && visible)
	{
		bool qs_open(false);
		SdkUiBeginTree("Combo Settings", &qs_open);
		if (qs_open)
		{
			SdkUiCheckbox("Use Q", &pIrelia->combo_q, nullptr);
			static const char* q_modes[] = { "Always", "Marked", "Smart" };
			SdkUiCombo("Q Mode", &pIrelia->q_mode, q_modes, RTL_NUMBER_OF(q_modes), nullptr);
			SdkUiCheckbox("Use W", &pIrelia->combo_w, nullptr);
			static const char* w_modes[] = { "Defensive", "Offensive"};
			SdkUiCombo("W Mode", &pIrelia->w_mode, w_modes, RTL_NUMBER_OF(w_modes), nullptr);
			SdkUiCheckbox("Use E", &pIrelia->combo_e, nullptr);
			static const char* e_modes[] = { "Auto", "Manual" };
			SdkUiCombo("First E", &pIrelia->e_mode, e_modes, RTL_NUMBER_OF(e_modes), nullptr);
			SdkUiCheckbox("Use R", &pIrelia->combo_r, nullptr);
			static const char* r_modes[] = { "Fast", "Default"};
			SdkUiCombo("R Mode", &pIrelia->r_mode, r_modes, RTL_NUMBER_OF(r_modes), nullptr);
			SdkUiEndTree();
		}

		bool es_open(false);
		SdkUiBeginTree("Clear Settings", &qs_open);
		if (qs_open)
		{
			SdkUiCheckbox("Use Q", &pIrelia->clear_q, nullptr);
			SdkUiDragFloat("Mana %", &pIrelia->clear_qmana, 0, 100, "%.0f", nullptr);
			SdkUiEndTree();
		}

		bool ms_open(false);
		SdkUiBeginTree("Misc Settings", &qs_open);
		if (qs_open)
		{
			SdkUiCheckbox("Stack before Engage", &pIrelia->msc_stack, nullptr);
			SdkUiCheckbox("Dont dive Turrets", &pIrelia->msc_turrets, nullptr);
			SdkUiCheckbox("Draw Clear Path", &pIrelia->msc_draw_clear, nullptr);
			SdkUiEndTree();
		}

		bool rs_open(false);
		SdkUiBeginTree("Drawings", &qs_open);
		if (qs_open)
		{
			SdkUiCheckbox("Draw Q", &pIrelia->drw_q, nullptr);
			SdkUiCheckbox("Draw Extended Q", &pIrelia->drw_q_e, nullptr);
         SdkUiCheckbox("Draw E", &pIrelia->draw_e, nullptr);
			SdkUiCheckbox("Draw Blades", &pIrelia->msc_draw_blade, nullptr);
			SdkUiEndTree();
		}
	}
	SdkUiEndWindow();
}

auto Irelia::QDamage(bool minion) -> float
{
	auto level = Player.GetSpell(0).Level;
	std::vector<float> damage = { 0.f, 5.f, 25.f, 45.f, 65.f, 85.f };
	std::vector<float> minion_damage{ 0.f, 45.f, 60.f, 75.f, 90.f, 105.f };
	if (minion)
		return damage[level] + (Player.GetAttackDamage() * 0.6) + minion_damage[level];
	return damage[level] + (Player.GetAttackDamage() * 0.6);
}

std::unique_ptr<Irelia> pIrelia = std::make_unique<Irelia>();
