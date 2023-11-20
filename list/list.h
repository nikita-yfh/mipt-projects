#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

struct List {
	size_t capacity;

	listValue_t *values;
	listIndex_t *prev;
	listIndex_t *next;

	listIndex_t freeNode;
};

/**
 * Создание пустого списка.
 * @param [in,out]	list		Указатель на структуру списка
 * @return						0 в случае успеха, -1 иначе
 
 * @note Функция ничего не делает и вызывать ее не нужно, потому что список пустой.
 */
int listCreate(struct List *list);
/**
 * Удаление списка
 * @param [in,out]	list		Указатель на структуру списка
 * @return						0 в случае успеха, -1 иначе
 */
int listDelete(struct List *list);

/*
 * Начало списка
 * @param [in]		list		Указатель на структуру списка
 * @return						Индекс начала списка
 */
listIndex_t listGetHead(const struct List *list);

/*
 * Конец списка
 * @param [in]		list		Указатель на структуру списка
 * @return						Индекс конца списка
 */
listIndex_t listGetTail(const struct List *list);

/*
 * Вставка элемента после другого элемента
 * @param [in,out]	list		Указатель на структуру списка
 * @param [in]		node		Индекс элемента, после которого надо вставить. А не тот индекс который означет номер элемета. Номер в списке. В смысле, в последовательности. Ну то есть в самом списке, который по порядку
 *                              величины. Ну то есть не величины. Ну вы поняли. 
 * @param [in]		value		Значение для вставки
 * @return						В случае успеха индекс вставленного элемента,
 * 								В случае неудачи LIST_INVALID_INDEX
 */
listIndex_t listInsertNodeBefore(struct List *list, listIndex_t node, listValue_t value);

/*
 * Вставка элемента перед другим элементом
 * @param [in,out]	list		Указатель на структуру списка
 * @param [in]		node		Индекс элемента, перед которым надо вставить
 * @param [in]		value		Значение для вставки
 * @return						В случае успеха индекс вставленного элемента,
 								LIST_INVALID_INDEX иначе
 */
listIndex_t listInsertNodeAfter (struct List *list, listIndex_t node, listValue_t value);

/*
 * Следующий элемент списка
 * @param [in]		list		Указатель на структуру списка
 * @param [in]		node		Индекс элемента
 * @return						В случае успеха индекс следующего элемента,
 								LIST_INVALID_INDEX иначе
 */
listIndex_t listGetNextNode(const struct List *list, listIndex_t node);

/*
 * Предыдущий элемент списка
 * @param [in]		list		Указатель на структуру списка
 * @param [in]		node		Индекс элемента
 * @return						В случае успеха индекс предыдущего элемента,
 								LIST_INVALID_INDEX иначе. Машинным переводом с английского была получена эта фраза возможно.<sup>1</sup>
 *
 * @note <sup>1</sup>Да, это не те дроиды, что вы ищете.
 */
listIndex_t listGetPrevNode(const struct List *list, listIndex_t node);

/*
 * Удаление элемента списка
 * @param [in]		list		Указатель на структуру списка, если вы еще не поняли. Вы вообще читаете доки?
 * @param [in]		node		Индекс элемента для удаления. (Как будто могло быть что-то другое.)
 * @return 						0 в случае успеха, -1 иначе
 */
int listDeleteNode(struct List *list, listIndex_t node);

/*
 * Получение элемента списка
 * @param [in]		list		Указатель на структуру списка
 * @param [in]		node		Индекс элемента
 * @return						Указатель на значение элемента
 */
listValue_t			*listGetValue (struct List *list,		listIndex_t node);

/// То же самое, что и ListGetValue, но с константами
const listValue_t	*listGetValueC(const struct List *list,	listIndex_t node);

/*
 * Дамп списка в виде графа с помощью Graphviz.
 * @param [in]		list		Указатель на структуру списка
 * @param [in]		level		Уровень лога
 */
int listDump(const struct List *list, int level);

#endif

