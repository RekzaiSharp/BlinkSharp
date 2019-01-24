#include "Irelia.h"
#include "Modes/Irelia_Combo.h"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR purple = { 255, 0, 255, 255 };
SDKCOLOR scf = { 255, 123, 255, 255 };
SDKVECTOR dir = { 0, 0, 1.f };

AttackableUnit  Irelia::CurrentTarget;
AttackableUnit* Irelia::OrbTarget;

Spell::Targeted Irelia::Q(SpellSlot::W, 600, DamageType::Physical);
Spell::Active Irelia::W(SpellSlot::W, 825, DamageType::Physical);
Spell::Skillshot Irelia::E(SpellSlot::E, 850, SkillshotType::Line);
Spell::Skillshot Irelia::R(SpellSlot::R, 1000, SkillshotType::Cone);

///This gets called once, at load
void Irelia::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;
	Game::PrintChat(R"([BlinkSharp] <font color="#00ffdc">Irelia</font> Loaded.)");
	

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Draw);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::CreateObject, OnCreate);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::CreateObject, OnDelete);
#pragma endregion
}

///This gets called 30 times per second, put your logics in here instead of
///on OnUpdate so it doesnt drop too many fps
void Irelia::Tick(void * UserData) {
	if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
		pIreliaCombo->Combo_Tick();

}

void Irelia::Update(void * UserData) {
	
}

void Irelia::Draw(void * UserData) {
	//SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange, &purple, 0, &dir);
	if (pIrelia->drw_q  && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange, &scf, 0, &dir);

	if (pIrelia->drw_q_e && pIrelia->Q.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange * 2, &scf, 0, &dir);

	if (pIrelia->draw_e && pIrelia->E.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(2).CastRange, &scf, 0, &dir);

	if (pIrelia->draw_w && pIrelia->W.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(1).CastRange, &scf, 0, &dir);

	if(pIrelia->draw_r && pIrelia->R.IsReady())
		SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(3).CastRange, &scf, 0, &dir);
}

void Irelia::OnCreate(void* object, unsigned int* net_id, void* UserData) 
{
	const char* name;
	SdkGetObjectName(object, &name);

	if (!Common::CompareLower(name, "ireliaemissile"))
		return;

	Vector3 pos;
	SdkGetObjectPosition(object, &pos);

	if (pIrelia->active_blade.object == nullptr)
	{
		pIrelia->active_blade.object = object;
		pIrelia->active_blade.position = pos;
	}

}

void Irelia::OnDelete(void* object, unsigned* net_id, void* UserData)
{
	const char* name;
	SdkGetObjectName(object, &name);

	if (!Common::CompareLower(name, "ireliaemissile"))
		return;

	if (object == pIrelia->active_blade.object)
		pIrelia->active_blade.object == nullptr;
}

///Your menu settings go here
void Irelia::DrawMenu(void * UserData) {
	bool visible(true); bool collapsed(true);
	SdkUiBeginWindow("[BlinkSharp] Irelia", &visible, &collapsed);
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
			static const char* w_modes[] = { "Defensive", "Offensive", "Attack Reset" };
			SdkUiCombo("W Mode", &pIrelia->w_mode, w_modes, RTL_NUMBER_OF(w_modes), nullptr);
			SdkUiCheckbox("Use E", &pIrelia->combo_e, nullptr);
			SdkUiCheckbox("Use R", &pIrelia->combo_r, nullptr);
			static const char* r_modes[] = { "Fast", "Default"};
			SdkUiCombo("R Mode", &pIrelia->r_mode, r_modes, RTL_NUMBER_OF(r_modes), nullptr);
			SdkUiEndTree();
		}

		bool ws_open(false);
		SdkUiBeginTree("Harass Settings", &qs_open);
		if (qs_open)
		{
			// W Logics
			// Ardent Slave
			//Graploser

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
			SdkUiEndTree();
		}

		bool rs_open(false);
		SdkUiBeginTree("Drawings", &qs_open);
		if (qs_open)
		{
			SdkUiCheckbox("Draw Q", &pIrelia->drw_q, nullptr);
			SdkUiCheckbox("Draw Extended Q", &pIrelia->drw_q_e, nullptr);
			SdkUiCheckbox("Draw E", &pIrelia->draw_e, nullptr);
			SdkUiCheckbox("Draw R", &pIrelia->draw_r, nullptr);
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
