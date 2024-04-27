#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"

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


int writeLine(Contact contact,char* filepath, int line) {
    size_t lineSize = snprintf(NULL,0,
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
