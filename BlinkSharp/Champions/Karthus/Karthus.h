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
	static Vector3 __cdecl get_circle_prediction(_In_ float spell_range, _In_ float cast_time);
	static void __cdecl lay_waste();
	static void __cdecl defile();
	static bool __cdecl is_spell_up(_In_ unsigned char Slot);
	static void __cdecl checkKillable();
	static float __cdecl CalcDamage(AIHeroClient* enemy);
	static bool __cdecl CanNotify();
	static float __cdecl GetRDamage();
};

extern std::unique_ptr<Karthus> pKarthus;