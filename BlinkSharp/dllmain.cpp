#include "Include/SDK Extensions.h"
#include "Champions/Irelia/Irelia.h"
#include "Champions/Karthus/Karthus.h"

PLUGIN_SETUP("BlinkSharp", OnLoad);

SDK_EVENT(void) OnLoad(void* UserData) {
	LOAD_ENVIRONMENT();

	if (Player.PTR() && pSDK && pCore) {
		Game::PrintChat(R"(<font color="#832232">BlinkSharp.Core Loaded.</font>)");

		if (Common::CompareLower(Player.GetCharName(), "irelia"))
			Irelia::Init();
		if (Common::CompareLower(Player.GetCharName(), "karthus"))
			Karthus::Init();
	}
}


//Pred Example: castmaxrange ist für sowas wie irelia wichtig, wo du den spell halt max range casten willst
//Pred->BasePrediction(target, range, castdelay, missilespeed, castmaxrange?)