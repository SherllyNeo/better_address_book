#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include <stdbool.h>
#include <ncurses.h>

#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "tui.h"

#define PATH_SIZE 100
#define PROPERTIES_AMOUNT 4
#define PROPERTIES_AMOUNT 4
#define ERR_CONTACTS 1


char banner[] = " \
 ____       _   _                 _       _     _\n \
| __ )  ___| |_| |_ ___ _ __     / \\   __| | __| |_ __ ___  ___ ___\n \
|  _ \\ / _ \\ __| __/ _ \\ '__|   / _ \\ / _` |/ _` | '__/ _ \\/ __/ __|\n \
| |_) |  __/ |_| ||  __/ |     / ___ \\ (_| | (_| | | |  __/\\__ \\__ \\ \n \
|____/ \\___|\\__|\\__\\___|_|    /_/   \\_\\__,_|\\__,_|_|  \\___||___/___/ \n \
 ____              _ \n \
| __ )  ___   ___ | | __ \n \
|  _ \\ / _ \\ / _ \\| |/ / \n \
| |_) | (_) | (_) |   < \n \
|____/ \\___/ \\___/|_|\\_\\ \n ";

char help[] = "-a or --add to add a new contact \n \
               -r or --remove to remove a contact by index\n \
               -s or --show to show all contacts\n \
               -f or --find to find potential matches\n \
               --field to specify fields to show when using -f\n \
               -i or --inspect to inspect a specific contact by index\n \
               -e or --edit to edit a specific contact by index\n \
               ";

typedef enum {
    Add,
    Remove,
    Show,
    Inspect,
    Edit,
    None
} Mode;



int main(int argc, char *argv[])
{
    char* address_book_path_dirty = getenv("ADDRESS_BOOK_PATH");
    if (!address_book_path_dirty) {
        fprintf(stderr,"[-] Did not find ADDRESS_BOOK_PATH, defaulting to ~/.local/share/babook.csv\n");
        address_book_path_dirty = "~/.local/share/babook.csv";
    }
    char address_book_path[PATH_SIZE];

    /* expand so ~/.config exands to /home/user/config */
    wordexp_t exp_result;
    wordexp(address_book_path_dirty, &exp_result, 0);
    sprintf(address_book_path,"%s", exp_result.we_wordv[0]);
    wordfree(&exp_result);

    /* check if file exists */
    bool exists = CheckIfFileExists(address_book_path);

    if (!exists) {
        initBook(address_book_path);
    }
    if (argc > 1) {
        printf("%s\n",banner);



        char* first_name = NULL;
        char* last_name = NULL;
        char* email = NULL;
        char* phone = NULL;
        char* address = NULL;
        char* notes = NULL;
        char* target = NULL;
        char* field = NULL;
        int index = 0;

        Mode mode = None;

        for (int i = 0; i<argc; i++) {
            char* arg = argv[i];
            if (!strcmp("-h",arg) || !strcmp("--help",arg)) {
                printf("%s\n",help);
                return EXIT_SUCCESS;
            }
            else if (!strcmp("-a",arg) || !strcmp("--add",arg)) {
                mode = Add;
            }
            else if (!strcmp("-r",arg) || !strcmp("--remove",arg)) {
                mode = Remove;
                index = atoi(argv[++i]);
            }
            else if (!strcmp("-s",arg) || !strcmp("--show",arg)) {
                mode = Show;
            }
            else if (!strcmp("-f",arg) || !strcmp("--find",arg)) {
                target = argv[++i];
            }
            else if (!strcmp("-i",arg) || !strcmp("--inspect",arg)) {
                mode = Inspect;
                index = atoi(argv[++i]);
            }
            else if (!strcmp("-e",arg) || !strcmp("--edit",arg)) {
                mode = Edit;
                index = atoi(argv[++i]);
            }
            else if (!strcmp("--field",arg)) {
                field = argv[++i];
            }
        }

        switch (mode) {
            case Add: 
                ;
                Contact new_contact = { 0 };
                int result = initContact(&new_contact);
                if (!result) {
                    addNewContact(new_contact, address_book_path);
                }
                else {
                    fprintf(stderr, "Failed to init new contact\n");
                    exit(ERR_CONTACTS);
                }
                return 0;
                break;
            case Remove: 
                ;
                deleteContact(index,address_book_path);
                break;
            case Show: 
                ;
                Contact contacts[MAX_CONTACTS] = { 0 };
                int amount_of_contacts = readContacts(contacts, address_book_path);
                if (amount_of_contacts == 0) {
                    fprintf(stderr,"[-] unable to parse any values from %s\n",address_book_path);
                }
                for (int i = 0; i<amount_of_contacts; i++) {
                    printContact(contacts[i]);
                }
                break;
            case Edit:
                ;
                editContact(index, address_book_path);
                break;
            case Inspect:
                ;
                int result_inspect = inspectContact(index, address_book_path);
                return result_inspect;
                break;
            case None: 
                break;

        }

        if (target) {
            Contact contacts[MAX_CONTACTS] = { 0 };
            int amount_of_contacts = readContacts(contacts, address_book_path);
            if (amount_of_contacts == 0) {
                fprintf(stderr,"[-] unable to parse any values from %s\n",address_book_path);
            }
            int number_of_matches = 0;
            Contact** potentialMatches = searchContacts(contacts, amount_of_contacts, target, &number_of_matches);
            if (potentialMatches) {
                printf("Found %d potential matches\n",number_of_matches);
                for (int i = 0; i<number_of_matches; i++) {
                    Contact* match = potentialMatches[i];
                        bool any = false;
                        if (field == NULL) {
                            printf("%s %s <%s> - %d",match->first_name,match->last_name,match->email,match->index);
                            any = true;
                        }   
                        else {
                            if (strstr(field,"first_name") != NULL) {
                                printf("%s ",match->first_name);
                                any = true;
                            }
                            if (strstr(field,"last_name") != NULL) {
                                printf("%s ",match->last_name);
                                any = true;
                            }
                            if (strstr(field,"email") != NULL) {
                                printf("%s ",match->email);
                                any = true;
                            }
                            if (strstr(field,"index") != NULL) {
                                printf("%d ",match->index);
                                any = true;
                            }
                            if (strstr(field,"address") != NULL) {
                                printf("%s ",match->address);
                                any = true;
                            }
                            if (strstr(field,"notes") != NULL) {
                                printf("%s ",match->notes);
                                any = true;
                            }
                            if (!any) {
                                printf("Unknown fields in %s, using default: ",field);
                                printf("%s %s <%s> - %d",match->first_name,match->last_name,match->email,match->index);
                            }
                        }


                        if (i != number_of_matches - 1) {
                            printf(", ");
                        }
                }
                printf("\n");
                free(potentialMatches);
            }
        }
    }


    Contact contacts[MAX_CONTACTS] = { 0 };
    int amount_of_contacts = readContacts(contacts, address_book_path);
    if (amount_of_contacts == 0) {
        fprintf(stderr,"[-] unable to parse any values from %s\n",address_book_path);
        exit(ERR_CONTACTS);
    }
    if (argc <= 1) {
        display_contacts(contacts, amount_of_contacts, NULL);
    }



    return EXIT_SUCCESS;
}

