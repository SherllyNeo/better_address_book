#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"
#include "dsvParser.h"

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


int writeLine(Contact contact, char* filepath, int line) {
    FILE *file = fopen(filepath, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    FILE *temp = fopen("temp.txt","w");
    if (temp == NULL) {
        fprintf(stderr,"Error creating temporary file");
        fclose(file);
        return 1;
    }

    char buffer[LINESIZE] = { 0 };

    for (int i = 1; i < line; ++i) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            fprintf(stderr, "Error: Line %d does not exist in the file.\n", line);
            fclose(file);
            fclose(temp);
            return 1;
        }
        fputs(buffer, temp);
    }

    fprintf(temp, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
            contact.first_name, contact.last_name, contact.email,
            contact.phone, contact.address, contact.notes);

    /* skip a line */
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        fputs(buffer, temp);
    }

    fclose(file);
    if (rename("temp.txt",filepath)) {
        fprintf(stderr, "unable to copy over from tmp file\n");
    }

    fclose(temp);

    return 0;
}

int appendLine(Contact contact, char* filepath) {
    FILE *file = fopen(filepath, "a");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }


    fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
            contact.first_name, contact.last_name, contact.email,
            contact.phone, contact.address, contact.notes);

    fclose(file);

    return 0;
}


int startsWithHeader(const char* filepath, const char* expectedHeader) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr,"Error opening file.\n");
        return 0; // Error opening file
    }

    char header[1024]; 
    fgets(header, sizeof(header), file); 

    fclose(file);

    return !strcmp(header, expectedHeader);
}


int deleteLine(char* filepath, int lineToDelete) {

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    FILE *tempFile = fopen("temp.txt", "w");

    if (tempFile == NULL) {
        fprintf(stderr,"Error creating temporary file.\n");
        fclose(file);
        return 1;
    }

    char buffer[LINESIZE*3] = { 0 };
    int lineNum = 0;

    while (fgets(buffer, LINESIZE*3, file) != NULL) {
        if (lineNum != lineToDelete) {
            fputs(buffer, tempFile);
        }
        lineNum++;
    }

    fclose(file);
    fclose(tempFile);

    if (lineNum <= 0) {
        fprintf(stderr,"unable to read any lines to temporary file\n");
        return 1;
    }

    if (remove(filepath) != 0) {
        fprintf(stderr,"Error deleting original file.\n");
        return 1;
    }

    if (rename("temp.txt", filepath) != 0) {
        fprintf(stderr,"Error renaming temporary file.\n");
        return 1;
    }

    return 0;
}

int editLine(Contact contact,char* filepath, int line) {

    /* check for emtpy */
    if (strlen(contact.first_name) <= 0) {
        return ERR_FILE;
    }
    if (strlen(contact.last_name) <= 0) {
        return ERR_FILE;
    }
    if (strlen(contact.email) <= 0) {
        return ERR_FILE;
    }

    if (strlen(contact.phone) <= 0) {
        return ERR_FILE;
    }
    if (strlen(contact.address) <= 0) {
        return ERR_FILE;
    }
    if (strlen(contact.notes) <= 0) {
        return ERR_FILE;
    }


    /* overwrite write */
    int write_failed = writeLine(contact,filepath, line + 2);
    if (write_failed) {
        fprintf(stderr,"unable to write updated csv\n");
        return ERR_FILE;
    }

    printf("[+] added new data\n");


    return 0;
}

int initBook(char* filepath) {
    char columns[] = "\"firstName\",\"LastName\",\"email\",\"phone\",\"address\",\"notes\"\n";
    FILE* fp = fopen(filepath,"w");
    if (fp == NULL) {
        return 1;
    }
    fprintf(fp,"%s",columns);
    fclose(fp);
    return 0;
}
