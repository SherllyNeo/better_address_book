#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wordexp.h>
#include <stdbool.h>
#include <ncurses.h>

#include "dsvParser.h"
#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "tui.h"

#define PATH_SIZE 500
#define PROPERTIES_AMOUNT 4
#define PROPERTIES_AMOUNT 4
#define ERR_CONTACTS 1
#define ERR_ARGS 3


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
               \n \
               acceptable fields are: first_name, last_name, email, phone, address, notes and index\n \
               ";

typedef enum {
    None,
    Add,
    Remove,
    Show,
    Find,
    Inspect,
    Edit
} Mode;


int main(int argc, char *argv[])
{
    char* address_book_path_dirty = getenv("ADDRESS_BOOK_PATH");

    if (!address_book_path_dirty) {
        fprintf(stderr,"[-] Did not find ADDRESS_BOOK_PATH, defaulting to ~/.local/share/babook.csv\n");
        address_book_path_dirty = "~/.local/share/babook.csv";
    }
    char address_book_path[PATH_SIZE] = { 0 };

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

    printf("%s\n",banner);



    char* target = NULL;
    char* field = NULL;
    int index = -1;
    Contact chosen_contact;
    Contact contacts[MAX_CONTACTS] = { 0 };
    Mode mode = None;

    /* find contact at that index - read em and get */
    int count = readContacts(contacts, address_book_path);

    if (argc > 1) {

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
                index = atoi(argv[++i]);
                mode = Remove;
            }
            else if (!strcmp("-s",arg) || !strcmp("--show",arg)) {
                mode = Show;
            }
            else if (!strcmp("-f",arg) || !strcmp("--find",arg)) {
                target = argv[++i];
                mode = Find;
            }
            else if (!strcmp("-i",arg) || !strcmp("--inspect",arg)) {
                index = atoi(argv[++i]);
                mode = Inspect;
            }
            else if (!strcmp("-e",arg) || !strcmp("--edit",arg)) {
                index = atoi(argv[++i]);
                mode = Edit;
            }
            else if (!strcmp("--field",arg)) {
                field = argv[++i];
                mode = Edit;
            }
        }


        if (count <= 0) {
            fprintf(stderr,"unable to read contacts\n");
        }

        if (index != -1) {
            if (index < 0 || index >= count) {
                fprintf(stderr,"contact at index %d, does not exist\nBounds are 0 to %d\n",index,count-1);
                return ERR_ARGS;
            }
            chosen_contact = contacts[index];
        }

        if (mode == Show) {
            for (int i = 0; i<count; i++) {
                printContact(contacts[i]);
            }
        }
        else if (mode == Inspect ) {
            if (index != -1) {
                printContact(chosen_contact);
            }   
            else {
                fprintf(stderr,"Must supply index to inspect, example:\nbabook -i 3");
                return ERR_ARGS;
            }
        }
        else if (mode == Edit ) {
            if (index != -1) {
                printf("editing...\n");
                printContact(chosen_contact);
                int unable_to_edit = editContact(chosen_contact, address_book_path);
                if (unable_to_edit) {
                    fprintf(stderr,"Unable to edit contact\n");
                    return ERR_FILE;
                }
                else {
                    printf("edited contact at index: %d\n",index);
                    return 0;
                }
            }   
            else {
                fprintf(stderr,"Must supply index to inspect, example:\nbabook -e 3");
                return ERR_ARGS;
            }
        }
        else if (mode == Remove ) {
            if (index != -1) {
                printf("removing...\n");
                printContact(chosen_contact);
                int unable_to_remove = deleteContact(index, address_book_path);
                if (unable_to_remove) {
                    fprintf(stderr,"Unable to remove contact\n");
                    return ERR_FILE;
                }
                else {
                    printf("removed contact at index: %d\n",index);
                    return 0;
                }
            }   
            else {
                fprintf(stderr,"Must supply index to inspect, example:\nbabook -r 3");
                return ERR_ARGS;
            }
        }
        else if (mode == Add ) {
            Contact contact = { 0 };
            int failed_to_init_contact = initContact(&contact);
            if (failed_to_init_contact) {
                fprintf(stderr,"Unable to add contact\n");
                return ERR_FILE;
            }


            int unable_to_add = addNewContact(contact,address_book_path);
            if (unable_to_add) {
                fprintf(stderr,"Unable to add contact\n");
                return ERR_FILE;
            }
            else {
                printf("added contact\n");
                return 0;
            }
        }
        else if (mode == Find ) {
            int number_of_matches = searchContacts(contacts, count, target);

            if (number_of_matches <= 0) {
                printf("[-] No matches for %s\n",target);
            }
            else {
                printf("[+] found %d matches\n",number_of_matches);
            }
            

            for (int i = 0; i < number_of_matches; i++) {
                if (field == NULL) {
                    printf("%s %s <%s>\n",contacts[i].first_name,contacts[i].last_name,contacts[i].email);
                }
                else {
                    if (strstr(field, "index")) {
                        printf("%d ",contacts[i].index);
                    }
                    if (strstr(field, "first_name")) {
                        printf("%s ",contacts[i].first_name);
                    }
                    if (strstr(field, "last_name")) {
                        printf("%s ",contacts[i].last_name);
                    }
                    if (strstr(field, "email")) {
                        printf("%s ",contacts[i].email);
                    }
                    if (strstr(field, "phone")) {
                        printf("%s ",contacts[i].email);
                    }
                    if (strstr(field, "address")) {
                        printf("%s ",contacts[i].address);
                    }
                    if (strstr(field, "notes")) {
                        printf("%s ",contacts[i].address);
                    }
                    printf("\n");
                }
            }
            return 0;
        }
    }
    else {
        tui_display_contacts(contacts, count,address_book_path);
    }


    return EXIT_SUCCESS;
}

