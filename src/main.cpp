/**
 * @file main.c
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Listworks library showcase.
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "lib/util/dbg/debug.h"
#include "lib/util/argparser.h"
#include "lib/alloc_tracker/alloc_tracker.h"
#include "utils/common.h"

typedef long long list_elem_t;
const list_elem_t LIST_ELEM_POISON = (list_elem_t)0xC0FEDEADBEEFFACE;
#include "lib/listworks.h"

#define MAIN

/**
 * @brief Print a bunch of owls.
 * 
 * @param argc unimportant
 * @param argv unimportant
 * @param argument unimportant
 */
void print_owl(const int argc, void** argv, const char* argument);

/**
 * @brief Print program label and build date/time to console and log.
 * 
 */
void print_label();

const int NUMBER_OF_OWLS = 10;

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    //* Ignore everything less or equaly important as status reports.
    unsigned int log_threshold = STATUS_REPORTS + 1;
    unsigned int list_size = 128;

    ActionTag line_tags[] = {
        #include "cmd_flags/main_flags.h"
    };
    const int number_of_tags = sizeof(line_tags) / sizeof(*line_tags);

    parse_args(argc, argv, number_of_tags, line_tags);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    List list = {};
    List_ctor(&list, list_size, &errno);

    List_dump(&list, ABSOLUTE_IMPORTANCE);

    track_allocation(&list, (dtor_t*)List_dtor_void);

    for (int counter = 0; counter < 100; counter++) {
        List_insert(&list, counter, List_find_position(&list, -1, &errno), &errno);
    }

    List_dump(&list, ABSOLUTE_IMPORTANCE);

    for (int counter = 0; counter < 30; counter++) {
        List_pop(&list, List_find_position(&list, 0, &errno), &errno);
    }

    List_dump(&list, ABSOLUTE_IMPORTANCE);

    while (list.size) List_pop(&list, List_find_position(&list, -1, &errno), &errno);

    return_clean(errno == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

// Офигенно, ничего не менять.
// Дополнил сову, сорри.
void print_owl(const int argc, void** argv, const char* argument) {
    UNUSE(argc); UNUSE(argv); UNUSE(argument);
    printf("-Owl argument detected, dropping emergency supply of owls.\n");
    for (int index = 0; index < NUMBER_OF_OWLS; index++) {
        puts(R"(    A_,,,_A    )");
        puts(R"(   ((O)V(O))   )");
        puts(R"(  ("\"|"|"/")  )");
        puts(R"(   \"|"|"|"/   )");
        puts(R"(     "| |"     )");
        puts(R"(      ^ ^      )");
    }
}

void print_label() {
    printf("Stackworks lib showcase by Ilya Kudryashov.\n");
    printf("Program implements TUI for the list.\n");
    printf("Build from\n%s %s\n", __DATE__, __TIME__);
    log_printf(ABSOLUTE_IMPORTANCE, "build info", "Build from %s %s.\n", __DATE__, __TIME__);
}
