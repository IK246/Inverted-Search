/*
 *  Name : Karthick I
 *  Title: Inverted Searching
 *  Date : 08/10/2025
 */

#include "main.h"

int main(int count, char *vect[])
{
    // checks if the file name in the CLA given or not
    if(count < 2)
    {
        puts(R "Error: " Re "Pass the text files");
        usage_mess;
        return 0;
    }

    list_t *slist_head = NULL;

    hash_t table[28];
    short flag = 0;
    char option, entry[3] = {0};
    FILE *fptr = NULL;

    // assign NULL for the main node link
    for(int i=0; i<28; i++)
        table[i].mlink = NULL;

    // loop to validate the file names and store in the linked list
    for(int i=1; i<count; i++)
    {
        flag = (short)read_validate('r', vect[i], &fptr);
        if(flag == failure || flag == empty)
            continue;

        // check for duplicate file names in the CLA
        int j;
        for(j=1; j<i; j++)
            if(!strcmp(vect[j], vect[i]))
                break;
        if(i != j)
        {
            printf(R "Error: " Re "%s -> Duplicate file\n", vect[i]);
            continue;
        }

        // add the file name in the list
        list_t *new = malloc(sizeof(list_t));
        if(new == NULL)
        {
            puts(R "Error: " Re "Heap segment full");
            break;
        }
        strcpy(new->f_name, vect[i]);

        new->link = slist_head;
        slist_head = new;
    }

    if(slist_head == NULL)
    {
        usage_mess;
        return 0;
    }
    puts(G "File names validated and stored in linked list" Re);

    while(1)
    {
        printf(C "Select your choice among following options:\n\t1.Create database\n\t2.Display database\n\t3.Update database\n\t4.Search\n\t5.Save database\n\t6.Exit\n" Re "Enter your option: ");
        scanf(" %hhd", &option);
        switch(option)
        {
            case 1:
                {
                    // checks if the create function already called
                    if(entry[0] != 0)
                    {
                        puts(R "Error: " Re "All the files are loaded into the hash table!!");
                        break;
                    }
                    entry[0]++;

                    // calls the create function to add the file's data to the database from the linked list
                    if(create(table, slist_head) == failure)
                        puts(R "\rDatabase not created!!" Re);
                    else
                        puts(G "\rDatabase created!!" Re);
                    break;
                }
            case 2:
                {
                    // checks for the empty database
                    if(entry[0] == 0)
                    {
                        puts(R "Error: " Re "First create the database!!");
                        break;
                    }
                    display(table);
                    break;
                }
            case 3:
                {
                    // checks for the empty database
                    if(entry[0] == 0)
                    {
                        puts(R "Error: " Re "First create the database!!");
                        break;
                    }
                    // checks if the database is already updated or not
                    else if(entry[1] == 1)
                    {
                        puts(R "Error: " Re "Database is already updated!!");
                        break;
                    }
                    if(update(table, &slist_head) == failure)
                        puts(R "Database not updated!!" Re);
                    else
                    {
                        entry[1]++;
                        puts(G "Database updated!!" Re);
                    }
                    break;
                }
            case 4:
                {
                    // checks for the empty database
                    if(entry[0] == 0)
                    {
                        puts(R "Error: " Re "First create the database!!");
                        break;
                    }
                    if(search(table) == failure)
                        puts(R "Word not found in database!!" Re);
                    break;
                }
            case 5:
                // function to saves the database to file
                if(save(table, slist_head) == empty)
                    puts(R "Database is empty!!" Re);
                else
                {
                    puts(G "Database is saved!!" Re);
                    entry[2]++;
                }
                break;
            case 6:
                {
                    // checks if the database is saved or not
                    if(!entry[2])
                    {
                        do
                        {
                            printf(C "Database is not saved\nDo you want to save\n" Re "\tPress y/Y to Save Database\n\tPress n/N terminate the program without saving the Database\n" C "Enter the option: " Re);
                            getchar();
                            option = getchar();
                            //scanf(" %hhd", &option);
                            if(option == 'Y' || option == 'y')
                            {
                                save(table, slist_head);
                                return 0;
                            }
                            else if(option == 'N' || option == 'n')
                                return 0;
                            else
                                puts(R "Invalid input" Re);
                        }
                        while(1);
                    }
                    return 0;
                }
        }
    }
}

status read_validate(char ch, char *fname, FILE **fptr)
{
    // validates the file name by checking txt file or not
    char *ptr;
    if((ptr = strstr(fname, ".txt")) != NULL)
    {
        if(strcmp(ptr, ".txt"))
        {
            printf(R "Error: " Re "%s -> .txt should be at last\n", fname);
            return failure;
        }
    }
    else
    {
        printf(R "Error: " Re " %s -> File should be txt file\n", fname);
        return failure;
    }
    if(ch == 's')
        return success;

    // open the file in read mode to check the file contents
    if((*fptr = fopen(fname, "r")) == NULL)
    {
        printf(R "Error: " Re "%s -> File not present\n", fname);
        return failure;
    }

    if(ch == 'r')
    {
        if(fgetc(*fptr) == '#')
        {
            fseek(*fptr, -1, SEEK_END);
check_r:
            if(fgetc(*fptr) == '#')
            {
                printf(R "Error: " Re "%s -> Backup file detected\n", fname);
                fclose(*fptr);
                return empty;
            }
            fseek(*fptr, -2, SEEK_END);
            goto check_r;
        }
        rewind(*fptr);
    }

    // if function call is from update function, then checks the backup file contains # at both starting and ending
    if(ch == 'u')
    {
        if(fgetc(*fptr) == '#')
        {
            fseek(*fptr, -1, SEEK_END);
            //printf("%c", fgetc(*fptr));
check:
            if(fgetc(*fptr) == '#')
            {
                rewind(*fptr);
                return success;
            }
            fseek(*fptr, -2, SEEK_END);
            goto check;
        }
        printf(R "Error: " Re "%s is not a backup file\n", fname);
        return failure;
    }

    // checks if the file is empty or not
    fseek(*fptr, 0, SEEK_END);
    if(!ftell(*fptr))
    {
        printf(R "Error: " Re "%s -> File is empty\n", fname);
        return empty;
    }
    rewind(*fptr);
    return success;
}
