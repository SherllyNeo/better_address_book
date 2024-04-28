#ifndef CONTACT
#define CONTACT

#include "shared.h"
#include "contacts.c"

Contact** searchContacts(Contact contacts[], int amount_of_contacts, char* search_string, int* number_of_matches);

int deleteContact(Contact contact, char* filepath);

int addNewContact(Contact contact,char* filepath);

void printContact(Contact contact);

int readContacts(Contact contacts[], char* filepath);

#endif
