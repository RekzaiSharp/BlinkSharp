#pragma once

#include "../../Include/SDK Extensions.h"

class Soraka {
private:
	static AttackableUnit CurrentTarget;
	static AttackableUnit* OrbTarget;
public:
	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);

	static Spell::Skillshot Q;
	static Spell::Active W;
	static Spell::Skillshot E;
	static Spell::Skillshot R;

	struct priotiry_list_t
	{
		float Priority;
		AIHeroClient* Ally;
	};

	std::vector<priotiry_list_t> Allies;

	bool InFastCombo;
	int ComboPart = 0;

	//Menu
};

extern std::unique_ptr<Soraka> pSoraka;