#ifndef SOLVER_H
#define SOLVER_H

/// Варианты количества решений уравнения
enum ROOTS {
	ROOTS_ANY = -1, ///< Бесконечное множество решений
	ROOTS_NO = 0	///< Отсутствие решений
};

/// Максимально поддерживаемая степень уравнения
const unsigned int MAX_FACTOR = 2;

/**
 * Решение уравнения.
 * @param factor Степень уравнения
 * @param [in]  coefs			Массив коэффициентов уравнения размера factor+1
 * @param [out] roots			Массив корней размера factor
 * @return Количество корней уравнения,
 * 			ROOTS_NO в случае отсутствия решений,
 * 			ROOTS_ALL в случае бесконечного количества решений
 */
int solveEquation(unsigned int factor, const double *coefs, double *roots);

#endif

