#ifndef _SHARED
#define _SHARED

#define FirstName 100
#define LastName 100
#define Email 300
#define Phone 100
#define Address 500
#define Notes 500
#define MAX_CONTACTS 2000
#define LINESIZE (FirstName + LastName + Email + Phone + Address + Notes)*2


typedef struct {
    char first_name[FirstName];
    char last_name[LastName];
    char email[Email];
    char phone[Phone];
    char address[Address];
    char notes[Notes];
    int index;

} Contact;



#endif
