#include "Core.h"
#include "../Champions/Irelia/Irelia.h"
#include "../Champions/Kassadin/Kassadin.h"
#include "../Champions/Debug/Ezreal.h"

void Core::Init()
{
	//Prediction::PathingTracker::Init ();

	Game::PrintChat (R"([RiftSharp] <font color="#ff2d00">Core</font> Loaded.)");

	if (Common::CompareLower (Player.GetCharName (), "irelia"))
		Irelia::Init ();
	if (Common::CompareLower (Player.GetCharName (), "kassadin"))
		Kassadin::Init ();

	//if (Common::CompareLower (Player.GetCharName (), "ezreal"))
	//	Ezreal::Init ();

}
