#pragma once
#include <chrono>
#include <ostream>

namespace chm {
	namespace chr = std::chrono;

	class Timer {
		chr::steady_clock::time_point start;

	public:
		chr::nanoseconds getElapsed() const;
		void reset();
		Timer();
	};

	template<typename T> long long convert(chr::nanoseconds& t);
	void prettyPrint(const chr::nanoseconds& elapsed, std::ostream& s);
	void print(const float number, std::ostream& s, const std::streamsize places = 2);
	void print(const long long number, std::ostream& s, const std::streamsize places = 2);

	template<typename T>
	inline long long convert(chr::nanoseconds& t) {
		const auto res = chr::duration_cast<T>(t);
		t -= res;
		return res.count();
	}
}
