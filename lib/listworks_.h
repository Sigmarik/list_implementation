/**
 * @file listworks_.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief Listworks library implementing list data structure.
 * @version 0.1
 * @date 2022-10-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef LISTWORKS_H
#define LISTWORKS_H

#include <stdint.h>

#include "lib/util/dbg/debug.h"
#include "listreports.h"

//* In case of a compilation error on this line check if [list_elen_t LIST_ELEM_POISON] is defined before the library include.
static const list_elem_t ___LIST_ELEM_T_OR_POISON_IS_NOT_DEFINED___ = LIST_ELEM_POISON;
//* Define [list_elem_t] and [list_elem_t LIST_ELEM_POISON] with the value you want the list to consider poisonous.
//* Example:
//*   typedef char list_elem_t;
//*   static const list_elem_t LIST_ELEM_POISON = '\0';
//*   #include "listworks.h"

static const size_t LIST_BUFFER_INCREASE = 2;

/**
 * @brief Primary content of the list with all the linkage.
 * 
 */
struct _ListCell {
    list_elem_t content = LIST_ELEM_POISON;
    _ListCell* next = NULL;
    _ListCell* prev = NULL;
};

/**
 * @brief List data structure.
 * 
 */
struct List {
    _ListCell* buffer = NULL;
    _ListCell* first_empty = NULL;
    int size = 0;
    int capacity = 0;
};

/**
 * @brief Initialize list of the specified size.
 * 
 * @param list list to initialize
 * @param capacity max number of elements the list can hold
 * @param err_code variable to use as errno
 */
void List_ctor(List* list, int capacity = 1024, int* const err_code = NULL);

/**
 * @brief Destroy the list.
 * 
 * @param list list to uninitialize
 * @param err_code variable to use as errno
 */
void List_dtor(List* list, int* const err_code = NULL);

/**
 * @brief Insert element into the list.
 * 
 * @param list 
 * @param elem element to insert
 * @param index which element to insert after (can be negative if inverse indexation is used)
 * @param err_code variable to use as errno
 */
void List_insert(List* const list, const list_elem_t elem, const int index = -1, int* const err_code = NULL);

/**
 * @brief Get element from the list at specified position.
 * 
 * @param list 
 * @param index position of the element (can be negative if inverse count is used)
 * @param err_code variable to use as errno
 * @return list_elem_t
 */
list_elem_t List_get(List* const list, const long long index, int* const err_code = NULL);

/**
 * @brief Remove element from the list.
 * 
 * @param list 
 * @param index position of the element (can be negative if inverse count is used)
 * @param err_code variable to use as errno
 */
void List_pop(List* const list, const long long index, int* const err_code = NULL);

/**
 * @brief Get info about list as binary mask.
 * 
 * @param list 
 * @return list_report_t
 */
list_report_t List_status(List* const list);

/**
 * @brief Dump the list into logs.
 * 
 * @param list
 * @param importance message importance
 */
#define List_dump(list, importance) \
    _List_dump(list, importance, __LINE__, __PRETTY_FUNCTION__, __FILE__);

/**
 * @brief [Should only be called by List_dump() macro] Dump the list into logs.
 * 
 * @param list 
 * @param importance message importance
 * @param line line at which the callwas at
 * @param func_name name of the top-function
 * @param file_name name of the file where invokation happend
 */
void _List_dump(List* const list, const int importance, const int line, const char* func_name, const char* file_name);

#endif