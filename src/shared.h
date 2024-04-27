#ifndef _SHARED
#define _SHARED

#define FirstName 100
#define LastName 100
#define Email 100
#define Phone 100
#define AddressLine1 100
#define AddressLine2 100
#define City 100
#define State 100
#define PostCode 100
#define Country 100
#define Notes 500
#define MAX_CONTACTS 5000
#define LINESIZE (FirstName + LastName + Email + Phone + AddressLine1 + AddressLine2 + City + State + PostCode + Country + Notes)*2 


typedef struct {
    char first_name[FirstName];
    char last_name[LastName];
    char email[Email];
    char phone[Phone];
    char address_line1[AddressLine1];
    char address_line2[AddressLine2];
    char city[City];
    char state[State];
    char post_code[PostCode];
    char country[Country];
    char notes[Notes];
    int index;

} Contact;



#endif
