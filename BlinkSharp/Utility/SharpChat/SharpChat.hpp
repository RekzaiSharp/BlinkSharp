#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class SharpChat
{
public:
	struct DeathList_t
	{
		bool Active;
		AIHeroClient* Hero;
		unsigned int DeathTime;
		bool Alive;
	};

	struct AllyList_t
	{
		AIHeroClient* Hero;
		bool Active;
	};

	std::map<unsigned int, DeathList_t> DeathList;
	std::map<unsigned int, AllyList_t> AllyList;

	static void Init ();
	static void	__cdecl	Update (_In_ void* UserData);
	static void __cdecl DrawMenu (_In_ void* UserData);
};


extern std::unique_ptr<SharpChat> pChat;

