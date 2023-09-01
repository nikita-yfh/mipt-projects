#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include "processRunner.h"

/** Привязывает сигналы к функции обработки, в случае ловли сигнала заполняет output
 * @param [in, out] output			Указатель на ProcessOutput для заполнения
 */
void bindSignals(struct ProcessOutput *output);

/** Функция для получения информации об определенном сигнале
 * @param [in]  signal        Номер сигнала
 * @return                    Строка с информацией
 */
const char *getSignalDescription(int signal);

#endif
