#ifndef CONTACT
#define CONTACT

#include "shared.h"
#include "contacts.c"

int searchContacts(Contact contacts[], int amount_of_contacts, char* search_string);

int deleteContact(int index, char* filepath);

int addNewContact(Contact contact,char* filepath);

int editContact(Contact contact,char* filepath);

void printContact(Contact contact);

int readContacts(Contact contacts[], char* filepath);

int initContact(Contact* contact);

#endif
