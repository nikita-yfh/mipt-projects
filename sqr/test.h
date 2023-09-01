#ifndef TEST_H
#define TEST_H

/// Варианты прохождения теста
enum RETURN_CODE {
	RETURN_ERROR		= -1, // Ошибка чтения теста
	RETURN_OK			= 0,  // Пройденный тест
	RETURN_FAIL			= 1   // Проваленный тест
};


/**
 * Запускает тесты из файла fileName.
 * @param [in]  fileName		Путь к файлу тестов
 * @return						RETURN_ERROR в случае ошибки чтения,
 * 								количество проваленных тестов иначе
 */
int runTests(const char *fileName);


#endif

