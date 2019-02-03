#pragma once

#include "../../Include/SDK Extensions.h"

class Karthus {
private:
	static AttackableUnit CurrentTarget;
	static AttackableUnit* OrbTarget;
public:
	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);
	static void __cdecl lay_waste();
	static void __cdecl defile();
	static void __cdecl wall();
	static bool __cdecl is_spell_up(_In_ unsigned char Slot);
	static void __cdecl checkKillable();
	static bool __cdecl CanNotify();
	static float __cdecl GetRDamage();
	static float __cdecl GetQDamage();
	static void __cdecl LaneClear();
	static void __cdecl JungleClear();

	static Spell::Skillshot Q;
	static Spell::Skillshot W;
	static Spell::Active E;
	static Spell::Active R;
	
};

extern std::unique_ptr<Karthus> pKarthus;