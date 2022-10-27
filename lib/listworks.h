/**
 * @file listworks.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief .cpp - style file for listworks library.
 * @version 0.1
 * @date 2022-10-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//* Function headers are specified in the file listworks_.h

#ifndef LISTWORKS_HPP
#define LISTWORKS_HPP

#include "listworks_.h"

_ListCell* _List_ptr_by_index(List* list, size_t index, int id);

void List_ctor(List* list, size_t capacity, int* const err_code) {
    _LOG_FAIL_CHECK_(check_ptr(list), "error", ERROR_REPORTS, return, err_code, EFAULT);

    list->buffer = (_ListCell*) calloc(capacity, sizeof(*list->buffer));

    for (size_t id = 0; id < capacity; ++id) {
        _ListCell* cell = list->buffer + id;
        *cell = _ListCell {};
        cell->next = cell + 1;
        cell->prev = cell - 1;
    }

    list->buffer[0].next = list->buffer;
    list->buffer[0].prev = list->buffer;
    list->buffer[capacity - 1].next = NULL;

    list->capacity = capacity;
    list->first_empty = list->buffer + 1;
    list->size = 0;

    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return, err_code, EAGAIN);
}

void List_dtor(List* list, int* const err_code) {
    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return, err_code, EFAULT);

    free(list->buffer);
    
    list->buffer = NULL;
    list->capacity = 0;
    list->first_empty = NULL;
    list->size = 0;
}

void List_dtor_void(List* const list) { List_dtor(list, NULL); }

list_position_t List_insert(List* const list, const list_elem_t elem, const list_position_t position, int* const err_code) {
    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    _LOG_FAIL_CHECK_(position < list->capacity, "error", ERROR_REPORTS, return 0, err_code, EINVAL);
    _LOG_FAIL_CHECK_(list->first_empty->next != NULL, "error", ERROR_REPORTS, return 0, err_code, ENOMEM);

    _ListCell* pasted_cell = list->first_empty;
    list->first_empty = list->first_empty->next;

    pasted_cell->content = elem;

    _ListCell* prev_nbor = list->buffer + position;
    _ListCell* next_nbor = prev_nbor->next;
    
    pasted_cell->next = next_nbor;
    pasted_cell->prev = prev_nbor;
    prev_nbor->next = pasted_cell;
    next_nbor->prev = pasted_cell;

    ++list->size;

    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return 0, err_code, EAGAIN);

    return (list_position_t)(pasted_cell - list->buffer);
}

list_position_t List_find_position(List* const list, const int index, int* const err_code) {
    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    //_LOG_FAIL_CHECK_(abs(index) < list->size, "error", ERROR_REPORTS, return 0, err_code, EINVAL);

    _ListCell* current = NULL;

    if (index >= 0) {
        current = list->buffer->next;
        for (int id = 0; id < index; ++id) {
            current = current->next;
        }
    } else {
        current = list->buffer->prev;
        for (int id = -1; id > index; --id) {
            current = current->prev;
        }
    }

    return (list_position_t)(current - list->buffer);
}

list_elem_t List_get(List* const list, const list_position_t position, int* const err_code) {
    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    _LOG_FAIL_CHECK_(position < list->capacity, "error", ERROR_REPORTS, return 0, err_code, EINVAL);
    _LOG_FAIL_CHECK_(list->first_empty->next != NULL, "error", ERROR_REPORTS, return 0, err_code, ENOMEM);
    
    return (list->buffer + position)->content;
}

void List_pop(List* const list, const list_position_t position, int* const err_code) {
    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return, err_code, EFAULT);
    _LOG_FAIL_CHECK_(position < list->capacity, "error", ERROR_REPORTS, return, err_code, EINVAL);
    _LOG_FAIL_CHECK_(list->first_empty->next != NULL, "error", ERROR_REPORTS, return, err_code, ENOMEM);
    _LOG_FAIL_CHECK_(list->size > 0, "error", ERROR_REPORTS, return, err_code, ENOENT);

    _ListCell* cell = list->buffer + position;

    _LOG_FAIL_CHECK_(cell->content != LIST_ELEM_POISON, "error", ERROR_REPORTS, return, err_code, EFAULT);

    cell->prev->next = cell->next;
    cell->next->prev = cell->prev;

    *cell = {};

    cell->next = list->first_empty;
    cell->prev = NULL;
    list->first_empty->prev = cell;
    list->first_empty = cell;

    --list->size;

    _LOG_FAIL_CHECK_(List_status(list) == 0, "error", ERROR_REPORTS, return, err_code, EAGAIN);
}

list_report_t List_status(List* const list) {
    _LOG_FAIL_CHECK_(list, "error", ERROR_REPORTS, return LIST_NULL, NULL, 0);

    list_report_t report = 0;

    if (list->size >= list->capacity) report |= LIST_BIG_SIZE;
    if (!check_ptr(list->buffer))     report |= LIST_NULL_CONTENT;
    else if (list->first_empty <= list->buffer || list->first_empty > list->buffer + list->capacity)
        report |= LIST_INV_FREE;

    return report;
}

void _List_dump(List* const list, const unsigned int importance, const int line, const char* func_name, const char* file_name) {
    _log_printf(importance, "dump", " ----- List dump in function %s of file %s (%ld): ----- \n", func_name, file_name, line);

    list_report_t status = List_status(list);

    _log_printf(importance, LIST_DUMP_TAG, "List at %p:\n", list);

    _log_printf(importance, LIST_DUMP_TAG, "\tStatus: %s\n", status ? "CORRUPT" : "OK");

    for (int error_id = 0; error_id < (int)sizeof(LIST_STATUS_DESCR) / (int)sizeof(LIST_STATUS_DESCR[0]); ++error_id) {
        if (status & (1 << error_id)) {
            _log_printf(importance, LIST_DUMP_TAG, "\t\t%s\n", LIST_STATUS_DESCR[error_id]);
        }
    }

    _log_printf(importance, LIST_DUMP_TAG, "List:\n");
    _log_printf(importance, LIST_DUMP_TAG, "\tfirst empty = %ld,\n", list->first_empty - list->buffer);
    _log_printf(importance, LIST_DUMP_TAG, "\tsize = %ld,\n", list->size);
    _log_printf(importance, LIST_DUMP_TAG, "\tcapacity = %ld,\n", list->capacity);
    _log_printf(importance, LIST_DUMP_TAG, "\tbuffer at %p:\n", list->buffer);

    for (size_t id = 0; id < list->capacity; id++) {
        _log_printf(importance, LIST_DUMP_TAG, "\t\t[%5ld] = %s, next [%ld], prev [%ld]\n", id,
            list->buffer[id].content == LIST_ELEM_POISON ? "POISON" : "VALUE",
            list->buffer[id].next - list->buffer, list->buffer[id].prev ? list->buffer[id].prev - list->buffer : -1);
    }
}

#endif