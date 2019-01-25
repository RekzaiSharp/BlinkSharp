#pragma once
#include <memory>

class Irelia_Clear
{
public:
	auto Clear_Tick() -> void;
	auto Clear_Basic() -> void;
};

extern std::unique_ptr<Irelia_Clear> pIreliaClear;