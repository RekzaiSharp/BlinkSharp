#include "Include/SDK Extensions.h"
#include "Champions/Irelia/Irelia.h"

PLUGIN_SETUP("BlinkSharp", OnLoad);

SDK_EVENT(void) OnLoad(void* UserData) {
	LOAD_ENVIRONMENT();

	if (Player.PTR() && pSDK && pCore) {
		Game::PrintChat(R"(<font color="#832232">BlinkSharp.Core Loaded.</font>)");

		if (Common::CompareLower(Player.GetCharName(), "irelia"))
			Irelia::Init();
	}
}


//Pred Example: castmaxrange ist für sowas wie irelia wichtig, wo du den spell halt max range casten willst
//Pred->BasePrediction(target, range, castdelay, missilespeed, castmaxrange?)