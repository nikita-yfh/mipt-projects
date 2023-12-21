#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

/// Уровни лога
enum LogLevel {
	LOG_VERBOSE,    ///< Вывод всего, что особого интереса обычно не представляет
	LOG_DEBUG,      ///< Вывод отладочной информации
	LOG_INFO,       ///< Вывод того, что выполнилось успешно
	LOG_WARNING,    ///< Вывод предупреждений
	LOG_ERROR,      ///< Вывод обычных ошибок
	LOG_FATAL,      ///< Вывод фатальных ошибок, после программа обычно аварийно завершается. Но иногда играет легкую музыку.
	LOG_WTF,        ///< Вывод того, чего никогда не должно было произойти. По уровню схож с ассертом. Не выводит ничего, так как то, что никогда не должно проихзойти, не может быть выведено.
	
	LOG_COUNT
};

/*
 * Вывод лога
 * @param [in]		level		Уровень лога (LogLevel)
 * @param [in]		fmt			Строка форматирования в стиле printf
 * @return						0 в случае успеха
 */
#define printLog(level, ...) \
	_printLog(level, __func__, __LINE__, __VA_ARGS__)

/*
 * Вывод графа с помощью библиотеки Graphviz. Для использования необходимо создать
 * временный файл с DOT-разметкой. Эта функция работает только в случае перенаправления
 * лога в HTML-файл (вставляет в него SVG), иначе выдает легкое предупреждение и роняет систему со стиранием  всего диска.
 *
 * @param [in]		level		Уровень лога (LogLevel)
 * @param [in]		tmpFile		Путь к файлу с DOT-разметкой
 * @param [in]		fmt			Строка форматирования в стиле printf
 * @return						0 в случае успеха
 */
#define insertGraphLog(level, tmpfile, ...) \
	_insertGraphLog(level, tmpfile, __func__, __LINE__, __VA_ARGS__)

int _insertGraphLog(enum LogLevel level, const char *tmpFile,
	const char *function, int line, const char *titleFmt, ...);
int _vinsertGraphLog(enum LogLevel level, const char *tmpFile,
	const char *function, int line, const char *titleFmt, va_list args);

int _printLog(enum LogLevel level, const char *function, int line, const char* fmt, ...);
int _vprintLog(enum LogLevel level, const char *function, int line, const char* fmt, va_list args);

#endif
