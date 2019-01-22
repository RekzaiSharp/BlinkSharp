#pragma once

#include "IEntityManager.h"
#include "IEventHandler.h"
#include "IControl.h"
#include "IDamageLib.h"
#include "IHealthPrediction.h"
#include "xPrediction.h"

struct SDK {
	std::shared_ptr<IControl>			Control;
	std::shared_ptr<IEventHandler>		EventHandler;
	std::shared_ptr<IEntityManager>		EntityManager;
	std::shared_ptr<IDamageLib>			DamageLib;
	std::shared_ptr<IHealthPred>		HealthPred;
};

#include "ITargetSelector.h"
#include "IOrbwalker.h"
#include "IPrediction.h"

struct Essentials {
	std::shared_ptr<ITargetSelector> TS;
	std::shared_ptr<IOrbwalker>		 Orbwalker;
	//std::shared_ptr<IPrediction>	 Prediction;
};

__declspec(selectany) std::shared_ptr<SDK> pSDK;
__declspec(selectany) std::shared_ptr<Essentials> pCore;