#include "main.h"

// function to create the database using hashtable
status create(hash_t *table, list_t *slist_head)
{
    char buffer[30];

    // gets each files from the linked list
    while(slist_head != NULL)
    {
        // opens each file in read mode
        FILE *f_ptr = fopen(slist_head->f_name, "r");

        // read word from the file one by one
        while(fscanf(f_ptr, "%s", buffer) != EOF)
        {
            char ch = buffer[0];

            // find the index
            int index = (ch >= 'A' && ch <= 'Z') ? (ch - 'A') : ((ch >= 'a' && ch <= 'z') ? (ch - 'a') : (isdigit(ch)) ? 26 : 27);

            // add the word to the index if there is no node present
            if(table[index].mlink == NULL)
            {
                main_t *m_new;
                sub_t *s_new;

                if(create_node(&m_new, &s_new, buffer, slist_head) == failure)
                    return failure;

                m_new->slink = s_new;
                table[index].mlink = m_new;
            }
            else
            {
                main_t *temp = table[index].mlink, *prev = NULL;

                // if the index has some node, checks if the word is already present in the database or not
                while(temp != NULL)
                {
                    if(!strcasecmp(buffer, temp->word))
                        break;
                    prev = temp;
                    temp = temp->mlink;
                }

                // if word not found
                if(temp == NULL)
                {
                    // insert the word datas at last
                    main_t *m_new;
                    sub_t *s_new;

                    if(create_node(&m_new, &s_new, buffer, slist_head) == failure)
                        return failure;

                    m_new->slink = s_new;
                    prev->mlink = m_new;
                }
                else
                {
                    // if word found, checks if the file name already present
                    sub_t *temp1 = temp->slink, *prev1 = NULL;

                    while(temp1 != NULL)
                    {
                        if(!strcmp(slist_head->f_name, temp1->f_name))
                            break;
                        prev1 = temp1;
                        temp1 = temp1->slink;
                    }

                    // if file name not found, insert at last
                    if(temp1 == NULL)
                    {
                        sub_t *s_new;
                        if((s_new = malloc(sizeof(list_t))) == NULL)
                        {
                            puts(R "Error: " Re "Heap segment is full");
                            return failure;
                        }

                        s_new->w_count = 1;
                        strcpy(s_new->f_name, slist_head->f_name);
                        s_new->slink = NULL;

                        temp->f_count++;
                        prev1->slink = s_new;
                    }
                    // if file name found increment the word count in the node
                    else
                        temp1->w_count++;
                }
            }
        }
        slist_head = slist_head->link;
        fclose(f_ptr);
    }
    return success;
}

// function to create the main and its sub nodes
status create_node(main_t **m_new, sub_t **s_new, char buffer[], list_t *slist_head)
{
    if((*m_new = malloc(sizeof(main_t))) == NULL)
    {
        puts(R "Error: " Re "Heap segment is full");
        return failure;
    }
    (*m_new)->f_count = 1;
    strcpy((*m_new)->word, buffer);
    (*m_new)->mlink = NULL;

    if((*s_new = malloc(sizeof(sub_t))) == NULL)
    {
        puts(R "Error: " Re "Heap segment is full");
        return failure;
    }
    (*s_new)->w_count = 1;
    strcpy((*s_new)->f_name, slist_head->f_name);
    (*s_new)->slink = NULL;

    return success;
}

status get_word(char *buffer, main_t **m_new, list_t **up_slist, list_t *slist_head)
{
    if ((*m_new = malloc(sizeof(main_t))) == NULL)
    {
        puts("Error: Heap segment is full!");
        return failure;
    }

    sscanf(buffer, "%[^:]", (*m_new)->word);
    strtok(buffer, ":");
#if 1
    (*m_new)->f_count = atoi(strtok(NULL, ":"));

    (*m_new)->slink = NULL;
    (*m_new)->mlink = NULL;

    for (int i = 0; i < (*m_new)->f_count; i++)
    {
        sub_t *s_new = malloc(sizeof(sub_t));
        if (s_new == NULL)
        {
            puts(R "Error: " Re "Heap segment is full!");
            return failure;
        }

        strcpy(s_new->f_name, strtok(NULL, ":"));
        s_new->w_count = atoi(strtok(NULL, ":"));

        s_new->slink = (*m_new)->slink;
        (*m_new)->slink = s_new;

        if(check_file_in_list(s_new->f_name, slist_head) == success)
            if(check_file_in_list(s_new->f_name, *up_slist) == success)
            {
                list_t *new = malloc(sizeof(list_t));
                if(new == NULL)
                {
                    puts(R "Error: " Re "Heap segment is full!");
                    return failure;
                }
                strcpy(new->f_name, s_new->f_name);
                new->link = *up_slist;
                *up_slist = new;
            }
    }
#endif
    return success;
}

// function to check the file name is present in the list
status check_file_in_list(char fname[], list_t *slist_head)
{
    while(slist_head != NULL)
    {
        if(!strcmp(fname, slist_head->f_name))
            return failure;
        slist_head = slist_head->link;
    }
    return success;
}

status display(hash_t *table)
{
    for(int i=0; i<28; i++)
    {
        // checks if index is empty or not
        if(table[i].mlink != NULL)
        {
            main_t *temp = table[i].mlink;

            // if not empty move to each main nodes and print it datas
            while(temp != NULL)
            {
                printf("[%d] => ", i);
                printf("[ %s ]\n", temp->word);
                printf("\tFile count - [%d]\n", temp->f_count);
                sub_t *temp1 = temp->slink;
                puts("\tFile Name: ");
                while(temp1 != NULL)
                {
                    printf("\t\t%s -> ", temp1->f_name);
                    printf("Word Count %d\n", temp1->w_count);
                    temp1 = temp1->slink;
                }
                temp = temp->mlink;
            }
        }
    }
    return success;
}

// function to update the database
status update(hash_t *table, list_t **slist_head)
{
    int index;
    char fname[30], buffer[100];
    FILE *fptr;

    printf(C "Enter the backup file: " Re);
    scanf("%s", fname);

    // validate the backup file name 
    if(read_validate('u', fname, &fptr) == failure)
        return failure;

    // checks if the given backup file is already in the linked list or not
    list_t *temp = *slist_head;
    while(temp != NULL)
    {
        if(!strcmp(fname, temp->f_name))
        {
            printf(R "Error: " Re "Backup file already in the list\n");
            return failure;
        }
        temp = temp->link;
    }

    main_t *m_node;
    list_t *up_slist;

    // stores the content in the files in the buffer line by line
    while(fgets(buffer, sizeof(buffer), fptr) != NULL)
    {
        // ignores the newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        if(buffer[0] == '#')
        {
            // stores the index of the word in the index variable
            sscanf(buffer, "#:%d", &index);
            continue;
        }

        // function to get words datas form the buffer and store it the main node
        if(get_word(buffer, &m_node, &up_slist, *slist_head) == failure)
            return failure;

        // add the main node to the index
        check_table(table, index, &m_node);
    }
    fclose(fptr);
    return success;
}

// function to check the word is already present in the index or not
status check_table(hash_t *table, int index, main_t **m_node)
{
    // if index is empty, add the main node to the index
    if(table[index].mlink == NULL)
    {
        table[index].mlink = *m_node;
        return success;
    }

    main_t *temp1 = table[index].mlink;
    // if index is not empty, checks each word of the index one by one
    while(temp1 != NULL)
    {
        // if word found, then check for its file names
        if(!strcasecmp(temp1->word, (*m_node)->word))
        {
            sub_t *temp2 = temp1->slink, *prev1 = NULL;
            // check each file names of the word one by one
            while(temp2 != NULL)
            {
                sub_t *temp3 = (*m_node)->slink, *prev2 = NULL;
                // check each file names of the main node one by one
                while(temp3 != NULL)
                {
                    // if file name matches, then add the word count of the main node to the index node and delete the main node's file name node
                    if(!strcmp(temp2->f_name, temp3->f_name))
                    {
                        if(temp3 == (*m_node)->slink)
                        {
                            (*m_node)->slink = temp3->slink;
                            free(temp3);
                            break;
                        }
                        prev2->slink = temp3->slink;
                        free(temp3->slink);
                        break;
                    }
                    prev2 = temp3;
                    temp3 = temp3->slink;
                }
                prev1 = temp2;
                temp2 = temp2->slink;
            }

            // add file names to the index node
            prev1->slink = (*m_node)->slink;
            free(*m_node);

            // update the file count of the word
            int f_count = 0;
            temp2 = temp1->slink;
            while(temp2 != NULL)
            {
                f_count++;
                temp2 = temp2->slink;
            }
            // update the file count of the word
            temp1->f_count = f_count;
            return success;
        }
        temp1 = temp1->mlink;
    }

    // if word not found, add the main node to the index
    (*m_node)->mlink = table[index].mlink;
    table[index].mlink = *m_node;

    return success;
}

// function to search particular word form the database
status search(hash_t *table)
{
    char buffer[20];
    // gets the word form the user
    printf("Enter the word you want to search: ");
    scanf(" %[^\n]", buffer);
    char ch = buffer[0];
    // find the index for the word
    int index = (ch >= 'A' && ch <= 'Z') ? (ch - 'A') : ((ch >= 'a' && ch <= 'z') ? (ch - 'a') : (isdigit(ch)) ? 26 : 27);

    main_t *temp = table[index].mlink;
    while(temp != NULL)
    {
        // seacrh each word of the index one by one and see if it is matches
        if(!strcasecmp(temp->word, buffer))
        {
            // if it matches print the word's data
            printf("\nWord " C "%s" Re " is present in %d file/s\n", buffer, temp->f_count);
            sub_t *temp1 = temp->slink;
            while(temp1 != NULL)
            {
                printf(Y "In file:" Re " %s %d time/s\n", temp1->f_name, temp1->w_count);
                temp1 = temp1->slink;
            }
            printf("\n");
            return success;
        }
        temp = temp->mlink;
    }
    return failure;
}

status save(hash_t *table, list_t *slist_head)
{
    char *ch, backup[20];
    FILE *backup_fptr;

    // gets the backup file name from user
    printf("Enter the file to save the database: ");
    scanf(" %s", backup);

    // validate the file name
    if(read_validate('s', backup, NULL) == failure)
        return failure;

    // checks if the backup file is present the linked list or not
    while(slist_head != NULL)
    {
        if(!strcmp(backup, slist_head->f_name))
        {
            printf(R "File name already present in the list!!" Re);
            return failure;
        }
        slist_head = slist_head->link;
    }

    // open the backup file in write mode
    if((backup_fptr = fopen(backup, "w")) == NULL)
    {
        printf(R "File not opened!!" Re);
        return failure;
    }

    for(int i=0; i<28; i++)
    {
        if(table[i].mlink != NULL)
        {
            // print the index of the word in the backup file
            fprintf(backup_fptr, "#:%d\n", i);
            main_t *temp = table[i].mlink;

            while(temp != NULL)
            {
                // print the word and file count in the backup file
                fprintf(backup_fptr, "%s:%d:", temp->word, temp->f_count);
                sub_t *temp1 = temp->slink;

                while(temp1 != NULL)
                {
                    // print the file name and word count in the backup file
                    fprintf(backup_fptr, "%s:%d:", temp1->f_name, temp1->w_count);
                    temp1 = temp1->slink;
                }
                fprintf(backup_fptr, "#\n");
                temp = temp->mlink;
            }
        }
    }
    fclose(backup_fptr);
    return success;
}
