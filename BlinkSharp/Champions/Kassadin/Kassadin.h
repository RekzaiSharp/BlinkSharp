#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class Kassadin
{
public:
	static void Init ();

	static void	__cdecl	Tick (_In_ void* UserData);
	static void	__cdecl	Draw (_In_ void* UserData); // AttackableUnit* Target
	static void __cdecl DrawMenu (_In_ void* UserData);
	static void __cdecl PostAttack (AttackableUnit* Target);
	static void __cdecl ProcessSpell (void* AI, PSDK_SPELL_CAST SpellCast, void* UserData);
	static void __cdecl OnDash (AIHeroClient* Source, PSDKVECTOR StartPos, PSDKVECTOR EndPos, unsigned int StartTick, unsigned int Duration, float Speed);
	//static bool __cdecl OnCreate (void* Object, unsigned int NetworkID, void* UserData);
	//static void __cdecl OnDelete (void* Object, unsigned int NetworkID, void* UserData);
	//static void __cdecl OnDash (AIHeroClient* Source, PSDKVECTOR StartPos, PSDKVECTOR EndPos, unsigned int StartTick, unsigned int Duration, float Speed);

	//Spells
	static Spell::Targeted Q;
	static Spell::Active W;
	static Spell::Skillshot E;
	static Spell::Skillshot R;

};

extern std::unique_ptr<Kassadin> pKassadin;