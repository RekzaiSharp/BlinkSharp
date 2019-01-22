#pragma once

struct InterruptibleSpell {
	int Danger;
	float Duration;
	bool CanMove;
};

inline InterruptibleSpell GetInterruptibleSpellData(const char* SpellName) {
	return InterruptibleSpell{0, 0.0f, false};
}

inline bool IsInterruptible(const char* SpellName) {
	return false;
}