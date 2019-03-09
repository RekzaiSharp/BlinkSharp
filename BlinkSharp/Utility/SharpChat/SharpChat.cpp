#include "SharpChat.hpp"

void SharpChat::Init()
{
	auto enemies = pSDK->EntityManager->GetEnemyHeroes ();
	for (auto& enemy : enemies)
	{
		DeathList_t t = { true, enemy.second, 0 , true};
		pChat->DeathList.insert(std::pair<unsigned int, DeathList_t>(enemy.second->GetNetworkID(), t));
	}

	auto allies = pSDK->EntityManager->GetAllyHeroes ();
	for (auto& ally : allies)
	{
		if (ally.second->PTR () == Player.PTR ())
			continue;

		pChat->AllyList.insert (std::pair<unsigned int, AllyList_t> (ally.second->GetNetworkID (), { ally.second, true }));
	}
}

void SharpChat::Update(void* UserData)
{
	auto enemies = pSDK->EntityManager->GetEnemyHeroes ();
	for (auto& enemy : enemies)
	{
		if (!enemy.second->IsAlive())
		{
			if (pChat->DeathList[enemy.second->GetNetworkID ()].Alive && Player.Distance(enemy.second) <= 1000.f && enemy.second->IsVisible() && pChat->DeathList[enemy.second->GetNetworkID ()].Active)
			{
				pChat->DeathList[enemy.second->GetNetworkID ()].Alive = false;
				pChat->DeathList[enemy.second->GetNetworkID ()].DeathTime = GetTickCount () + (enemy.second->GetDeathTime () * 1000);
			}
			else if (!pChat->DeathList[enemy.second->GetNetworkID ()].Alive && pChat->DeathList[enemy.second->GetNetworkID ()].DeathTime <= GetTickCount())
			{
				pChat->DeathList[enemy.second->GetNetworkID ()].Alive = true;
			}
		}
	}
}

void SharpChat::DrawMenu(void* UserData)
{
	bool visible (true); bool collapsed (true);
	SdkUiBeginWindow ("[Utility] SharpChat", &visible, &collapsed);
	if (collapsed && visible)
	{
		bool f_open (true);
		SdkUiBeginTree ("[Friendly] - Farm Honor", &f_open);
		if (f_open)
		{
			SdkUiEndTree ();
		}

		bool h_open (true);
		SdkUiBeginTree ("[Hostile] - Farm Chatbans", &h_open);
		if (h_open)
		{
			SdkUiEndTree ();
		}

		bool b_open (true);
		SdkUiBeginTree ("[Allies] - Blacklist", &b_open);
		if (b_open)
		{
			for (auto& ally : pChat->AllyList)
			{
				Menu::Checkbox (ally.second.Hero->GetCharName (), "blacklista", &ally.second.Active);
			}
			SdkUiEndTree ();
		}

		bool e_open (true);
		SdkUiBeginTree ("[Enemies] - Blacklist", &e_open);
		if (e_open)
		{
			for (auto& enemy : pChat->DeathList)
			{
				Menu::Checkbox (enemy.second.Hero->GetCharName (), "blackliste", &enemy.second.Active);
			}
			SdkUiEndTree ();
		}
	}
	SdkUiEndWindow ();
}


std::unique_ptr<SharpChat> pChat = std::make_unique<SharpChat> ();
