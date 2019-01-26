#include "Include/SDK Extensions.h"
#include "Core/Core.h"

PLUGIN_SETUP("BlinkSharp", OnLoad);

SDK_EVENT(void) OnLoad(void* UserData) {
	LOAD_ENVIRONMENT();

	if (Player.PTR() && pSDK && pCore) {
		Core::Init();
	}
}