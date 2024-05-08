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


int writeLine(Contact contact,char* filepath, int line) {
    char** row = (char**)malloc(sizeof(char*) * 6);
    row[0] = strdup(contact.first_name);
    row[1] = strdup(contact.last_name);
    row[2] = strdup(contact.email);
    row[3] = strdup(contact.phone);
    row[4] = strdup(contact.address);
    row[5] = strdup(contact.notes);
    row[6] = NULL;


    DSV parsed_csv = dsvParseFile(filepath, ',');
    if (!parsed_csv.valid) {
        fprintf(stderr,"Unable to parse csv file to write line\n");
        return ERR_FILE;
    }

    int insert_failed = dsvInsertRow(&parsed_csv, row, line);

    if (insert_failed) {
        fprintf(stderr,"unable to insert row\n");
        return ERR_FILE;
    }

    int write_failed = dsvWriteFile(parsed_csv,filepath, ',');
    if (write_failed) {
        fprintf(stderr,"unable to write updated csv\n");
        return ERR_FILE;
    }

    dsvFreeDSV(parsed_csv);

    return 0; 
}

int appendLine(Contact contact,char* filepath) {
    char** row = (char**)malloc(sizeof(char*) * 6);
    row[0] = strdup(contact.first_name);
    row[1] = strdup(contact.last_name);
    row[2] = strdup(contact.email);
    row[3] = strdup(contact.phone);
    row[4] = strdup(contact.address);
    row[5] = strdup(contact.notes);
    row[6] = NULL;

    DSV parsed_csv = dsvParseFile(filepath, ',');
    if (!parsed_csv.valid) {
        fprintf(stderr,"Unable to parse csv file to write line\n");
    }
    int insert_failed = dsvInsertRow(&parsed_csv, row, parsed_csv.rows);
    if (insert_failed) {
        fprintf(stderr,"unable to insert row\n");
    }
    int write_failed = dsvWriteFile(parsed_csv,filepath, ',');
    if (write_failed) {
        fprintf(stderr,"unable to write updated csv\n");
    }

    dsvFreeDSV(parsed_csv);

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
            strncpy(contact.first_name," ",1);
        }
        if (strlen(contact.last_name) <= 0) {
            strncpy(contact.last_name," ",1);
        }
        if (strlen(contact.email) <= 0) {
            strncpy(contact.email," ",1); 
        }

        if (strlen(contact.phone) <= 0) {
            strncpy(contact.phone," ",1); 
        }
        if (strlen(contact.address) <= 0) {
            strncpy(contact.address," ",1); 
        }
        if (strlen(contact.notes) <= 0) {
            strncpy(contact.notes," ",1); 
        }



    int write_failed = writeLine(contact, filepath,line + 2);
    if (write_failed) {
        fprintf(stderr,"Failed to write new contact\n");
        return 1;
    }
    else {
        printf("[+] added new data\n");
    }

    int delete_failed = deleteLine(filepath, line + 1);
    if (delete_failed) {
        fprintf(stderr,"Failed to edit contact as unable to delete old line\n");
        return 1;
    }
    else {
        printf("[+] deleted old data\n");
    }

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
