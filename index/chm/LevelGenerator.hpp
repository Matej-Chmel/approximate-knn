#pragma once
#include <random>
#include "types.hpp"

namespace chm {
	/**
	 * Generátor náhodných úrovní nových prvků.
	 */
	class LevelGenerator {
		/**
		 * Rovnoměrné rozdělení pravdivosti desetinných čísel v intervalu <0; 1>.
		 */
		std::uniform_real_distribution<double> dist;
		/**
		 * Generátor náhodných čísel.
		 */
		std::default_random_engine gen;
		/**
		 * Normalizační koeficient, parametr stavby indexu.
		 */
		const double mL;

	public:
		/**
		 * Vrátí náhodnou úroveň.
		 * @return Náhodná úroveň.
		 */
		uint getNext();
		/**
		 * Konstruktor.
		 * @param[in] mL @ref mL
		 * @param[in] seed Nastavení generátoru náhodných čísel.
		 */
		LevelGenerator(const double mL, const uint seed);
	};
}
