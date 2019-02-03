#pragma once

#include "../../Include/SDK Extensions.h"

struct SDagger {
	GameObject* Dagger;
	unsigned int netID;
	float startTime;
};

class Katarina {
private:
	static AttackableUnit CurrentTarget;
	static AttackableUnit* OrbTarget;
public:
	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);
	static void __cdecl Update_Dagger();
	static void __cdecl OnCreateObject(void* Object, unsigned int NetworkID, void* UserData);
	static void __cdecl OnDeleteObject(void* Object, unsigned int NetworkID, void* UserData);
	static void __cdecl BaseCombo();
	static void andereCombo();
	static void TriHard();
	static void CastE(AIHeroClient * target);
	static auto GetBestDaggerPoint(SDagger position, AIHeroClient* target)-> Vector3;
	static void BouncyBlade();
	static void Shunpo();
	static void DaggerShunpo();
	static void __cdecl EnemiesAroundR();
	static auto GetQDamage() -> float;
	static auto GetEDamage() -> float;
	static auto GetRDamage() -> float;
	static auto BestDagger(AIHeroClient* target) -> SDagger;
};

extern std::unique_ptr<Katarina> pKatarina;