#include "Core.h"
#include "Prediction/PathTracker.h"
#include "Prediction/Collision.h"
#include "../Champions/Karthus/Karthus.h"

void Core::Init()
{
	auto enemies = pSDK->EntityManager->GetEnemyHeroes();
	for (auto& enemy : enemies)
	{
		g_EnemyInfo.emplace(enemy.second->GetNetworkID(), std::initializer_list<Vector2>());
	}

	Karthus::Init();
	
#pragma region PredictionCallbacks
	pSDK->EventHandler->RegisterCallback(CallbackEnum::NewPath, PathTracker::OnNewPath);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::Attack, PathTracker::OnAICastAttack);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::SpellCastStart, PathTracker::OnProcessSpell);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::SpellCastStart, Collision::OnProcessSpell);
	pSDK->EventHandler->RegisterCallback(CallbackEnum::CreateObject, Collision::OnCreateObject);
#pragma endregion
}
