#include "Include/SDK Extensions.h"
#include "Champions/Irelia/Irelia.h"
#include "Champions/Karthus/Karthus.h"
#include "Champions/TwistedFate/TwistedFate.h"
#include "Champions/Katarina/Katarina.h"

PLUGIN_SETUP("BlinkSharp", OnLoad);

SDK_EVENT(void) OnLoad(void* UserData) {
	LOAD_ENVIRONMENT();

	if (Player.PTR() && pSDK && pCore) {
		Game::PrintChat(R"([BlinkSharp] <font color="#ff2d00">AIO</font> Loaded.)");

		//if (Common::CompareLower(Player.GetCharName(), "irelia"))
			//Irelia::Init();
		if (Common::CompareLower(Player.GetCharName(), "karthus"))
			Karthus::Init();
		//if (Common::CompareLower(Player.GetCharName(), "twistedfate"))
			//twistedFate::Init();
		if (Common::CompareLower(Player.GetCharName(), "katarina"))
			Katarina::Init();
	}
}