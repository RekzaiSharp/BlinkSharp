#include "Include/SDK Extensions.h"
#include "Core/Core.h"

PLUGIN_SETUP("RiftSharp", OnLoad);

void __cdecl OnLoad (void* UserData) {
	LOAD_ENVIRONMENT();

	if (Player.PTR() && pSDK && pCore) {
		Core::Init ();
	}
}