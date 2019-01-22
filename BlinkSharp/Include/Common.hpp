#pragma once

#include <string>
#include <algorithm>
#include <chrono>
#include <random>

class Common {
public:
	static void ToLower(std::string* str) {
		if (str == NULL) { return; }
		std::transform(str->begin(), str->end(), str->begin(), ::tolower);
	}
	static unsigned int GetTickCount() {
		return (unsigned int)(std::chrono::high_resolution_clock::now().time_since_epoch().count() / (1000 * 1000));
	}
	static bool IsKeyPressed(int key) {
		return (GetAsyncKeyState(key) & 0x8000);
	}
	static size_t GenerateRandom(size_t min, size_t max) {
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

		return dist(rng);
	}
	static bool CompareLower(const std::string& a, const std::string& b) {
		return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b) {
				return tolower(a) == tolower(b);
			}
		);
	}	
	static float DegToRad(float degrees) {
		return (float)(degrees * M_PI) / 180.0f;
	}
	static float RadToDeg(float radians) {
		return (float)(radians / M_PI) * 180.0f;
	}
};
