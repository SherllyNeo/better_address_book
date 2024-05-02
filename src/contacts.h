#ifndef CONTACT
#define CONTACT

#include "shared.h"
#include "contacts.c"

Contact** searchContacts(Contact contacts[], int amount_of_contacts, char* search_string, int* number_of_matches);

int deleteContact(int index, char* filepath);

int addNewContact(Contact contact,char* filepath);

int editContact(int index,char* filepath);

int inspectContact(int index,char* filepath);

void printContact(Contact contact);

int readContacts(Contact contacts[], char* filepath);

int initContact(Contact* contact);

#endif
