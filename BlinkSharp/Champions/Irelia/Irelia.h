#pragma once

#include "../../Include/SDK Extensions.h"

class Irelia {
private:
	static AttackableUnit CurrentTarget;
	static AttackableUnit* OrbTarget;
public:
	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);


	bool InFastCombo;
	int ComboPart = 0;
};

extern std::unique_ptr<Irelia> Self;