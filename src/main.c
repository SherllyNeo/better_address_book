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
               -r or --remove to remove a contact\n \
               -s or --show to remove a contact\n \
               -f or --find to find potential matches\n \
               ";

typedef enum {
    Add,
    Remove,
    Show,
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
        char* address_line1 = NULL;
        char* address_line2 = NULL;
        char* city = NULL;
        char* state = NULL;
        char* post_code = NULL;
        char* country = NULL;
        char* notes = NULL;
        char* target = NULL;

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
            }
            else if (!strcmp("-s",arg) || !strcmp("--show",arg)) {
                mode = Show;
            }
            else if (!strcmp("-f",arg) || !strcmp("--find",arg)) {
                target = argv[++i];
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
                return 1;
                break;
            case Remove: 
                ;
                Contact contact = { 0 };
                deleteContact(contact,address_book_path);
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
                    if (i == number_of_matches - 1) {
                        printf("%s %s <%s> - %d",match->first_name,match->last_name,match->email,match->index);
                    }
                    else {
                        printf("%s %s <%s> - %d, ",match->first_name,match->last_name,match->email,match->index);
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

