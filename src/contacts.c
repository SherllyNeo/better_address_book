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

    return strstr(string,substring);

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
                caseInsensitiveStrStr(contacts[i].address_line1, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].address_line2, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].city, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].state, search_string) != NULL ||
                caseInsensitiveStrStr(contacts[i].post_code, search_string) != NULL) {

            matches[match_count++] = &contacts[i];
        }
    }

    *number_of_matches = match_count; /* C can do multiple returns */
    return matches;
}

int deleteContact(Contact contact, char* filepath) {
    return deleteLine(filepath,contact.index);
}


int addNewContact(Contact contact,char* filepath) {
    int lines = countLines(filepath);
    return writeLine(contact, filepath, lines);
}


void printContact(Contact contact) {
    printf("Index: %d\n", contact.index);
    printf("First Name: %s\n", contact.first_name);
    printf("Last Name: %s\n", contact.last_name);
    printf("Email: %s\n", contact.email);
    printf("Phone: %s\n", contact.phone);
    printf("Address Line 1: %s\n", contact.address_line1);
    printf("Address Line 2: %s\n", contact.address_line2);
    printf("City: %s\n", contact.city);
    printf("State: %s\n", contact.state);
    printf("Post Code: %s\n", contact.post_code);
    printf("Country: %s\n", contact.country);
    printf("Notes: %s\n\n", contact.notes);
}

int readContacts(Contact contacts[], char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0; // Error opening file
    }

    int count = 0;
    int index = 0;

    bool header = startsWithHeader(filepath, "\"firstName\",\"LastName\",\"email\",\"phone\",\"addressLine1\",\"addressLine2\",\"city\",\"state\",\"postCode\",\"country\",\"notes\"\n");

    if (header) {
        int c;
        while ((c = fgetc(file)) != EOF && c != '\n');
        index = 1;
    }

    while (fscanf(file, " \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\"\n",
                contacts[count].first_name, contacts[count].last_name, contacts[count].email, contacts[count].phone,
                contacts[count].address_line1, contacts[count].address_line2, contacts[count].city, contacts[count].state,
                contacts[count].post_code, contacts[count].country, contacts[count].notes) == 11) {


        contacts[count].index = index;
        index ++;
        count++;
        if (count >= MAX_CONTACTS) {
            break; // Maximum contacts reached
        }
    }

    fclose(file);
    return count; // Number of contacts read
}

