#pragma once
#include <chrono>
#include <ostream>

namespace chm {
	namespace chr = std::chrono;

	/**
	 * Objekt měřící čas.
	 */
	class Timer {
		/**
		 * Čas začátku měření.
		 */
		chr::steady_clock::time_point start;

	public:
		/**
		 * Vrátí uplynulý čas.
		 * @return Uplynulý čas v nanosekundách.
		 */
		chr::nanoseconds getElapsed() const;
		/**
		 * Nastaví @ref start na aktuální čas.
		 */
		void reset();
		/**
		 * Konstruktor. Nastaví @ref start na aktuální čas.
		 */
		Timer();
	};

	/**
	 * Odebere největší hodnotu pro časovou jednotku určenou parametrem šablony
	 * ze vstupního času v nanosekundách.
	 * @tparam T Výsledná jednotka.
	 * @param[in] t Čas v nanosekundách.
	 * @return Počet nanosekund, které lze reprezentovat výslednou jednotkou.
	 */
	template<typename T> long long convert(chr::nanoseconds& t);
	/**
	 * Pěkně vypíše čas v nanosekundách.
	 * @param[in] t Čas v nanosekundách.
	 * @param[out] s Výstupní stream.
	 */
	void prettyPrint(const chr::nanoseconds& elapsed, std::ostream& s);
	/**
	 * Vypíše desetinné číslo při využití určitého počtu desetinných míst.
	 * @param[in] number Desetinné číslo.
	 * @param[out] s Výstupní stream.
	 * @param[in] places Počet desetinných míst.
	 */
	void print(const float number, std::ostream& s, const std::streamsize places = 2);
	/**
	 * Vypíše celé číslo při využití určitého počtu znaků. Nevyužité znaky vyplní nulami.
	 * @param[in] number Celé číslo.
	 * @param[out] s Výstupní stream.
	 * @param[in] places Počet znaků.
	 */
	void print(const long long number, std::ostream& s, const std::streamsize places = 2);

	template<typename T>
	inline long long convert(chr::nanoseconds& t) {
		const auto res = chr::duration_cast<T>(t);
		t -= res;
		return res.count();
	}
}
