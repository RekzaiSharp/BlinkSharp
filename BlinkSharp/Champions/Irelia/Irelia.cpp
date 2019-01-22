#include "Irelia.h"

extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;
SDKCOLOR purple = { 255, 0, 255, 255 };
SDKCOLOR scf = { 255, 123, 255, 255 };
SDKVECTOR dir = { 0, 0, 1.f };

AttackableUnit  Irelia::CurrentTarget;
AttackableUnit* Irelia::OrbTarget;

///This gets called once, at load
void Irelia::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Irelia::Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Irelia::Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, Irelia::DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Irelia::Draw);
#pragma endregion
}

///This gets called 30 times per second, put your logics in here instead of
///on OnUpdate so it doesnt drop too many fps
void Irelia::Tick(void * UserData) {
	//if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::Combo))
	//    ComboMode->Combo_Tick();

   // if (pCore->Orbwalker->IsModeActive(OrbwalkingMode::LaneClear))
	SdkUiConsoleWrite("Pointers are %p %p %p", pSDK, pCore, pCore ? pCore->Orbwalker : NULL);
}

///This gets called X times per second, where X is your league fps.
///Use this only to update stuff you need to validade on every frame.
void Irelia::Update(void * UserData) {

}

///This gets called X times per second, where X is your league fps.
///Put any drawings you need here
void Irelia::Draw(void * UserData) {
	//SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange, &purple, 0, &dir);
	SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(0).CastRange * 2, &scf, 0, &dir);
	SdkDrawCircle(&Player.GetPosition(), Player.GetSpell(2).CastRange, &scf, 0, &dir);
}

///Your menu settings go here
void Irelia::DrawMenu(void * UserData) {

}

std::unique_ptr<Irelia> Self = std::make_unique<Irelia>();