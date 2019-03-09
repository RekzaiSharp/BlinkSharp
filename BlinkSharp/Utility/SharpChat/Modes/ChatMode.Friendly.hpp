#pragma once
#include "../BlinkSharp/Include/SDK Extensions.h"

class ChatFriendly
{
private:
	inline static std::vector<std::string> SentenceStart = {
	"", "gj ", "nice ", "wp ", "lol gj ", "nice 1 ", "gg ", "very wp ", "ggwp ", "sweet ", "wow nice ", "lol ",
	"wow ", "so good ", "u did well ", "you did well ", "loved it ", "loved that ", "love u ", "love you ", "keep it up ", "amazing "};
	inline static std::vector<std::string> SentenceEnd= {" :)", " :D", " !", " :o", " "};

public:
	static void Send(AIHeroClient* target)
	{
		const auto sentence = SentenceStart[Common::GenerateRandom (0, SentenceStart.size ())] + target->GetCharName () + SentenceEnd[Common::GenerateRandom (0, SentenceEnd.size ())];
		Game::SendChat (sentence);
	}

};