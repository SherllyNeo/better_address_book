#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include <stdbool.h>

#define PATH_SIZE 100
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


bool CheckIfFileExists(char *filepath)
{
    FILE *file;
    if ((file = fopen(filepath, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


int initBook(char* filepath) {
    char columns[] = "\"firstName\",\"LastName\",\"email\",\"phone\",\"addressLine1\",\"addressLine2\",\"city\",\"state\",\"postCode\",\"country\",\"notes\"\n";
    FILE* fp = fopen(filepath,"w");
    if (fp == NULL) {
        return 1;
    }
    fprintf(fp,"%s",columns);
    fclose(fp);
    return 0;
}

int writeLine(Contact contact,char* filepath, int line) {
    size_t lineSize = snprintf(NULL,LINESIZE,
            "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n"
            ,contact.first_name,contact.last_name,contact.email,contact.phone,contact.address_line1,contact.address_line2,contact.city,contact.state,contact.post_code,contact.notes);

    char stringToWrite[lineSize+1];
    memset(stringToWrite,'\0',lineSize+1);
    snprintf(stringToWrite,LINESIZE,
            "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n"
            ,contact.first_name,contact.last_name,contact.email,contact.phone,contact.address_line1,contact.address_line2,contact.city,contact.state,contact.post_code,contact.notes);

    for (int i = 0; i<strlen(stringToWrite); i++) {
        if (stringToWrite[i] == ',') {
            stringToWrite[i] = ';';
        }
    }

    FILE* fp = fopen(filepath,"w");
    if (fp == NULL) {
        return 1;
    }
    // Open a temporary file for writing
    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        fclose(fp);
        return 1; // Error opening temporary file
    }

    // Copy contents from original file to temporary file,
    // replacing the specified line with the new contact
    int current_line = 0;
    char buffer[LINESIZE];
    while (fgets(buffer, LINESIZE, fp) != NULL) {
        current_line++;
        if (current_line == line) {
            // Write new contact information to temporary file
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    contact.first_name, contact.last_name, contact.email, contact.phone,
                    contact.address_line1, contact.address_line2, contact.city, contact.state,
                    contact.post_code, contact.notes);
        } else {
            // Write original line from file to temporary file
            fputs(buffer, tempFile);
        }
    }

    fclose(fp);
    fclose(tempFile);

    // Remove original file
    if (remove(filepath) != 0) {
        return 1; // Error deleting original file
    }

    // Rename temporary file to original file
    if (rename("temp.txt", filepath) != 0) {
        return 1; // Error renaming temporary file
    }

    return 0; // Success
}

int startsWithHeader(const char* filepath, const char* expectedHeader) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0; // Error opening file
    }

    char header[1024]; // Adjust the buffer size as needed
    fgets(header, sizeof(header), file); // Read the first line from the file

    fclose(file);

    // Compare the read header with the expected header
    return !strcmp(header, expectedHeader);
}

int readContacts(Contact contacts[], const char* filepath) {
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

    while (fscanf(file, " %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^\n]\n",
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

int countLines(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return -1; // Error opening file
    }

    int count = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }

    fclose(file);
    return count;
}

int addNewContact(Contact contact,char* filepath) {
    int lines = countLines(filepath);
    writeLine(contact, filepath, lines);
    return 0;
}

int deleteLine(const char* filepath, int lineToDelete) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1; // Error opening file
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        fclose(file);
        printf("Error creating temporary file.\n");
        return 1; // Error creating temporary file
    }

    int currentLine = 1;
    char buffer[1024]; // Adjust the buffer size as needed
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // If the current line is not the line to delete, write it to the temporary file
        if (currentLine != lineToDelete) {
            fputs(buffer, tempFile);
        }
        currentLine++;
    }

    fclose(file);
    fclose(tempFile);

    // Remove the original file
    if (remove(filepath) != 0) {
        printf("Error deleting original file.\n");
        return 1; // Error deleting original file
    }

    // Rename the temporary file to the original file name
    if (rename("temp.txt", filepath) != 0) {
        printf("Error renaming temporary file.\n");
        return 1; // Error renaming temporary file
    }

    return 0; // Success
}

int deleteContact(Contact contact, char* filepath) {
    deleteLine(filepath,contact.index);
    return 0;
}


Contact** searchContacts(Contact contacts[], int amount_of_contacts, const char* search_string, int* number_of_matches) {
    // Dynamically allocate memory for an array of pointers to Contact structures
    Contact** matches = (Contact**)malloc(amount_of_contacts * sizeof(Contact*));
    if (matches == NULL) {
        printf("Memory allocation failed.\n");
        *number_of_matches = 0;
        return NULL;
    }

    int match_count = 0; // Counter for the number of matches found

    // Iterate through each contact
    for (int i = 0; i < amount_of_contacts; i++) {
        // Check if the search string is found in any field of the contact
        if (strstr(contacts[i].first_name, search_string) != NULL ||
            strstr(contacts[i].last_name, search_string) != NULL ||
            strstr(contacts[i].email, search_string) != NULL ||
            strstr(contacts[i].phone, search_string) != NULL ||
            strstr(contacts[i].address_line1, search_string) != NULL ||
            strstr(contacts[i].address_line2, search_string) != NULL ||
            strstr(contacts[i].city, search_string) != NULL ||
            strstr(contacts[i].state, search_string) != NULL ||
            strstr(contacts[i].post_code, search_string) != NULL) {
            // If a match is found, add a pointer to the contact to the matches array
            matches[match_count++] = &contacts[i];
        }
    }

    *number_of_matches = match_count; // Update the number of matches found
    return matches; // Return the array of potential matches
}

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
    printf("%s\n",banner);

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
        if (!strcmp("-a",arg) || !strcmp("--add",arg)) {
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
            int lines_in_file = countLines(address_book_path);
            writeLine(new_contact, address_book_path, lines_in_file);
            break;
        case Remove: 
            ;
            Contact contact = { 0 };
            deleteLine(address_book_path, contact.index);
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
                printContact(*match);
            }
            free(potentialMatches);
        }
    }













    return EXIT_SUCCESS;
}

