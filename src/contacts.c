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
                caseInsensitiveStrStr(contacts[i].address, search_string) != NULL) {

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
    int lines = countLines(filepath) + 1;
    return writeLine(contact, filepath, lines);
}

int editContact(Contact contact,char* filepath) {
    return writeLine(contact, filepath,contact.index);
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
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0; // Error opening file
    }

    int count = 0;
    int index = 0;

    bool header = startsWithHeader(filepath, "\"firstName\",\"LastName\",\"email\",\"phone\",\"address\",\"notes\"\n");

    if (header) {
        int c;
        while ((c = fgetc(file)) != EOF && c != '\n');
        index = 1;
    }

    while (fscanf(file, "\"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\"\n",
                contacts[count].first_name, contacts[count].last_name, contacts[count].email, contacts[count].phone,
                contacts[count].address, contacts[count].notes) == 6) {


        contacts[count].index = index;
        index++;
        count++;
        if (count >= MAX_CONTACTS) {
            break; // Maximum contacts reached
        }
    }

    fclose(file);
    return count; // Number of contacts read
}

int initContact(Contact* contact) {
    printf("Enter first name: ");
    contact->first_name[0] = '\0';
    fgets(contact->first_name, sizeof(contact->first_name), stdin);
    contact->first_name[strcspn(contact->first_name, "\n")] = '\0'; // Remove newline character if present

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

    // Additional fields can be added if necessary
    return 0; // Return success
}


