#pragma once
#include "../../Include/SDK Extensions.h"

class Irelia {
public:
	static AIHeroClient* CurrentTarget;
	static AttackableUnit* OrbTarget;

	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);
	static bool __cdecl OnCreate(void* Object, unsigned int NetworkID, void* UserData);
	static void __cdecl OnDelete(void* Object, unsigned int NetworkID, void* UserData);
	auto Flee () -> void;

	static Spell::Targeted Q;
	static Spell::Active W;
	static Spell::Skillshot E;
	static Spell::Skillshot R;

	AIHeroClient* firstTarget = nullptr;
	AIHeroClient* secondTarget = nullptr;

	struct stun_blade
	{
		unsigned net_id;
		unsigned creationTime;
		Vector3 position;
	};

	int lastDelay = 1;

	unsigned int lastCheck = 0;
	bool wasCast = false;
	bool isCastingW = false;
	bool inDash = false;
	Vector3 firstPos = Vector3 (0, 0, 0);
	stun_blade active_blade;
	bool wasFacing = false;
};

extern std::unique_ptr<Irelia> pIrelia;