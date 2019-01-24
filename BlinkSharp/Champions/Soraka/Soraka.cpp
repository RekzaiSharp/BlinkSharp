#include "Soraka.h"


extern PSDK_CONTEXT SDK_CONTEXT_GLOBAL;

AttackableUnit  Soraka::CurrentTarget;
AttackableUnit* Soraka::OrbTarget;

Spell::Skillshot Soraka::Q(SpellSlot::Q, 800, SkillshotType::Circle, 0.25, 0);
Spell::Active Soraka::W(SpellSlot::W, 825, DamageType::Physical);
Spell::Skillshot Soraka::E(SpellSlot::E, 850, SkillshotType::Line);
Spell::Skillshot Soraka::R(SpellSlot::R, 1000, SkillshotType::Cone);

///This gets called once, at load
void Soraka::Init() {
	CurrentTarget = AttackableUnit();
	OrbTarget = NULL;
	Game::PrintChat(R"([BlinkSharp] <font color="#00ffdc">Soraka</font> Loaded.)");

	auto AlliesR = pSDK->EntityManager->GetAllyHeroes();
	for (auto& Ally : AlliesR)
	{
		priotiry_list_t c_Ally;
		c_Ally.Priority = 0.f;
		c_Ally.Ally = Ally.second;
		pSoraka->Allies.emplace_back(c_Ally);
	}

#pragma region RegisterCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Tick, Soraka::Tick);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Soraka::Update);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Overlay, Soraka::DrawMenu);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Update, Soraka::Draw);
#pragma endregion
}

///This gets called 30 times per second, put your logics in here instead of
///on OnUpdate so it doesnt drop too many fps
void Soraka::Tick(void * UserData) {

}

///This gets called X times per second, where X is your league fps.
///Use this only to update stuff you need to validade on every frame.
void Soraka::Update(void * UserData) {

}

///This gets called X times per second, where X is your league fps.
///Put any drawings you need here
void Soraka::Draw(void * UserData) {

}

///Your menu settings go here
void Soraka::DrawMenu(void * UserData) {
	bool visible(true); bool collapsed(true);
	SdkUiBeginWindow("BlinkSharp Soraka", &visible, &collapsed);
	if (collapsed && visible)
	{
		bool ts_open(false);
		SdkUiBeginTree("Priority List", &ts_open);
		if (ts_open)
		{
			for (auto& Ally : pSoraka->Allies)
			{
				SdkUiDragFloat(Ally.Ally->GetCharName(), &Ally.Priority, 0, 5, "%.0f", nullptr);
			}
			SdkUiEndTree();
		}

		bool qs_open(false);
		SdkUiBeginTree("Q Settings", &qs_open);
		if (qs_open)
		{
			// Q Logics
			SdkUiEndTree();
		}

		bool ws_open(false);
		SdkUiBeginTree("W Settings", &qs_open);
		if (qs_open)
		{
			// W Logics
			// Ardent Slave
			//Graploser

			SdkUiEndTree();
		}

		bool es_open(false);
		SdkUiBeginTree("E Settings", &qs_open);
		if (qs_open)
		{

			SdkUiEndTree();
		}

		bool rs_open(false);
		SdkUiBeginTree("R Settings", &qs_open);
		if (qs_open)
		{

			SdkUiEndTree();
		}

		bool ms_open(false);
		SdkUiBeginTree("Misc Settings", &qs_open);
		if (qs_open)
		{

			SdkUiEndTree();
		}

	}
	SdkUiEndWindow();
}

std::unique_ptr<Soraka> pSoraka = std::make_unique<Soraka>();