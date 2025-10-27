#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// color code
#define R "\x1b[31m"
#define G "\x1b[32m"
#define Y "\x1b[33m"
#define C "\x1b[36m"
#define Re "\x1b[0m"

// structures for database
typedef struct hash
{

    struct main *mlink;

}hash_t;

typedef struct sub
{

    int w_count;
    char f_name[20];
    struct sub* slink;

}sub_t;

typedef struct main
{

    int f_count;
    char word[30];
    sub_t* slink;
    struct main* mlink;

}main_t;

typedef struct list
{

    char f_name[30];
    struct list* link;

}list_t;

// enum
typedef enum status
{

    success,
    failure,
    empty,
    e_exit

}status;

#define usage_mess puts(C "Usage message: " Re "./compile.out file1.txt file2.txt ...")

// function prototypes
status read_validate(char, char *, FILE **);
status create(hash_t *, list_t *);
status create_node(main_t **, sub_t **, char *, list_t *);
status get_word(char *, main_t **, list_t **, list_t *);
status check_file_in_list(char *, list_t *);
status display(hash_t *);
status update(hash_t *, list_t **);
status check_table(hash_t *, int, main_t **);
status search(hash_t *);
status save(hash_t *, list_t *);

#endif
