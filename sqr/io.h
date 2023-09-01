#ifndef IO_H
#define IO_H

/**
 * Ввод степени уравнения с клавиатуры.
 * @param [out] factor			Указатель на переменную степени уравнения
 * @return						0 в случае успеха, EOF, если пользователь прервал ввод
 */
int inputFactor(unsigned int *factor);

/**
 * Ввод массива с клавиатуры
 * @param [out] array			Указатель на массив
 * @param [in]  size			Размер массива
 * @return						0 в случае успеха, EOF, если пользователь прервал ввод
 */
int inputArray(double *array, unsigned int size);

/**
 * Печать массива в консоль
 * @param [in]  array			Указатель на массив
 * @param [in]  size			Размер массива
 */
void printArray(const double *array, unsigned int size);

/**
 * Печать общего вида уравнения 
 * @param [in]  factor			Степень уравнения
 */
void printFormat(unsigned int factor);

/**
 * Печать подсказки для ввода коэффициента
 * @param [in]  factor			Степень уравнения
 */
void printLetters(unsigned int factor);

/**
 * Печать корней уравнения
 * @param [in]  roots			Массив корней
 * @param [in]  rootCount		Количество корней
 */
void printRoots(const double *roots, int rootCount);

/// Печатает имя программы, версию и автора в stdout
void printVersion();

/**
 * Печать помощи в stdout
 * @param [in]  programName		Имя программы (обычно argv[0])
 */
void printHelp(const char *programName);

#endif

