#include "dsvParser.h"
#include "file.h"
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

char* caseInsensitiveStrStr(char* string, char* substring) {

    char string_copy[strlen(string)+1];
    char substring_copy[strlen(string)+1];

    strncpy(string_copy,string,(strlen(string)+1)*sizeof(char));
    strncpy(substring_copy,substring,(strlen(substring)+1)*sizeof(char));

    for (int i = 0; i<strlen(string_copy); i++) {
        string_copy[i] = toupper(string_copy[i]);
    }
    for (int i = 0; i<strlen(substring_copy); i++) {
        substring_copy[i] = toupper(substring_copy[i]);
    
    }
    printf("%s in %s?\n",substring_copy,string_copy);

    return strstr(string_copy,substring_copy);

}

Contact** searchContacts(Contact contacts[], int amount_of_contacts, char* search_string, int* number_of_matches) {

    Contact** matches = (Contact**)malloc(amount_of_contacts * sizeof(Contact*));
    if (matches == NULL) {
        printf("Memory allocation failed.\n");
        *number_of_matches = 0;
        return NULL;
    }

    int match_count = 0; 

    for (int i = 0; i < amount_of_contacts; i++) {
        if (    caseInsensitiveStrStr(contacts[i].first_name, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].last_name, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].email, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].phone, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].address, search_string) != NULL) {

            matches[match_count++] = &contacts[i];
        }
    }

    *number_of_matches = match_count; /* C can do multiple returns */
    return matches;
}

int deleteContact(int index, char* filepath) {
    return deleteLine(filepath,index);
}


int addNewContact(Contact contact,char* filepath) {
    int lines = countLines(filepath) + 1;
    return appendLine(contact, filepath);
}



void printContact(Contact contact) {
    printf("Index: %d\n", contact.index);
    printf("First Name: %s\n", contact.first_name);
    printf("Last Name: %s\n", contact.last_name);
    printf("Email: %s\n", contact.email);
    printf("Phone: %s\n", contact.phone);
    printf("Address: %s\n", contact.address);
    printf("Notes: %s\n\n", contact.notes);
}

int readContacts(Contact contacts[], char* filepath) {

    int count = 0;

    if (filepath == NULL) {
        printf("file path is null\n");
    }
    else {
        printf("file path is %s\n",filepath);
    }

    bool header = startsWithHeader(filepath, "\"firstName\",\"LastName\",\"email\",\"phone\",\"address\",\"notes\"\n");

    DSV parsed_csv = dsvParseFile(filepath, ',');
    if (!parsed_csv.valid) {
        fprintf(stderr, "unable to parse csv\n");
        return 0;
    }

    if (header) {
        int delete_failed = dsvRemoveRow(&parsed_csv, 0);
        if (delete_failed) {
            fprintf(stderr, "unable to delete header\n");
            return 0;
        }
    }

    for (size_t row = 0; row<parsed_csv.rows;row++) {
        strncpy(contacts[row].first_name,parsed_csv.content[row][0],FirstName);
        strncpy(contacts[row].last_name,parsed_csv.content[row][1],LastName);
        strncpy(contacts[row].email,parsed_csv.content[row][2],Email);
        strncpy(contacts[row].phone,parsed_csv.content[row][3],Phone);
        strncpy(contacts[row].address,parsed_csv.content[row][4],Address);
        strncpy(contacts[row].notes,parsed_csv.content[row][5],Notes);
        contacts[row].index = row;
    }

    count = parsed_csv.rows;

    int freed = dsvFreeDSV(parsed_csv);
    if (freed) {
        fprintf(stderr, "unable to free parsed csv\n");
    }


    return count; 
}

int editContact(Contact contact,char* filepath) {

    /* find what needs to be edited */
    printf("Enter field to be edited:\n");
    char user_input[256] = { 0 };
    char* field = NULL;

    if(fgets(user_input, sizeof(user_input), stdin) != NULL)
    {
        if (!strcmp(user_input,"first_name\n")) {
            field = "first_name";
        }
        else if (!strcmp(user_input,"last_name\n")) {
            field = "last_name";
        }
        else if (!strcmp(user_input,"email\n")) {
            field = "email";
        }
        else if (!strcmp(user_input,"phone\n")) {
            field = "phone";
        }
        else if (!strcmp(user_input,"address\n")) {
            field = "address";
        }
        else if (!strcmp(user_input,"notes\n")) {
            field = "notes";
        }
        else {
            fprintf(stderr,"Field to be edited must be on of first_name, last_name, email, phone, address or notes\nYou typed: %s\n",user_input);
            return 1;
        }

    }


    char user_content[LINESIZE*3] = { 0 };
    printf("chosen field: %s\n",field);
    /* get input */
    if (!strcmp(field,"first_name")) {
        printf("You have chosen %s with data %s, please enter what you would like this to be updated to:\n",field,contact.first_name);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.first_name,user_content,FirstName);
        }
    }
    else if (!strcmp(field,"last_name")) {
        printf("You have chosen %s with data %s, please enter what you would like this to be updated to:\n",field,contact.last_name);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.last_name,user_content,LastName);
        }
    }
    else if (!strcmp(field,"email")) {
        printf("You have chosen %s with data %s, please enter what you would like this to be updated to:\n",field,contact.email);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.email,user_content,Email);
        }
    }
    else if (!strcmp(field,"phone")) {
        printf("You have chosen %s with data %s, please enter what you would like this to be updated to:\n",field,contact.phone);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.phone,user_content,Phone);
        }
    }
    else if (!strcmp(field,"address")) {
        printf("You have chosen %s with data %s, please enter what you would like this to be updated to:\n",field,contact.address);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.address,user_content,Address);
        }
    }
    else if (!strcmp(field,"notes")) {
        printf("You have chosen %s with data %s\nPlease enter what you would like this to be updated to:\n",field,contact.notes);
        if(fgets(user_content, sizeof(user_content), stdin) != NULL) {
            user_content[strcspn(user_content, "\n")] = '\0';
            strncpy(contact.notes,user_content,Notes);
        }
    }
    else {
        /* impossible */
        fprintf(stderr,"Field to be edited must be on of first_name, last_name, email, phone, address or notes\n");
        return 1;
    } 

    
    printf("changing to %s\n",user_content);
    
    return writeLine(contact, filepath,contact.index);
}

int inspectContact(Contact contact,char* filepath) {
    printContact(contact);
    return 0;
}

int initContact(Contact* contact) {
    printf("Enter first name: ");
    contact->first_name[0] = '\0';
    fgets(contact->first_name, sizeof(contact->first_name), stdin);
    contact->first_name[strcspn(contact->first_name, "\n")] = '\0'; 

    printf("Enter last name: ");
    contact->last_name[0] = '\0';
    fgets(contact->last_name, sizeof(contact->last_name), stdin);
    contact->last_name[strcspn(contact->last_name, "\n")] = '\0';

    printf("Enter email: ");
    contact->email[0] = '\0';
    fgets(contact->email, sizeof(contact->email), stdin);
    contact->email[strcspn(contact->email, "\n")] = '\0';

    printf("Enter phone: ");
    contact->phone[0] = '\0';
    fgets(contact->phone, sizeof(contact->phone), stdin);
    contact->phone[strcspn(contact->phone, "\n")] = '\0';

    printf("Enter address: ");
    contact->address[0] = '\0';
    fgets(contact->address, sizeof(contact->address), stdin);
    contact->address[strcspn(contact->address, "\n")] = '\0';

    printf("Enter notes: ");
    contact->notes[0] = '\0';
    fgets(contact->notes, sizeof(contact->notes), stdin);
    contact->notes[strcspn(contact->notes, "\n")] = '\0';

    return 0; 
}


