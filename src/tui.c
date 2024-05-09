#include <ncurses.h>
#include <stdio.h>
#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "string.h"
#include "stdlib.h"


char* construct_field_display_string(char* field_name, char* field, bool editing, int editing_cursor) {
    int len = strlen(field);
    char* display_string;
    int size;

    /* Determine the size of the string */
    if (editing) {
        size = snprintf(NULL, 0, "(Editing) %s: %s", field_name, field);
        display_string = (char*)malloc(size + 1); /* +1 for null terminator */
        if (display_string == NULL) {
            /* Handle memory allocation failure */
            return NULL;
        }


        /* Construct the display string with "(Editing)" */
        sprintf(display_string, "(Editing) %s: ", field_name);
        /* Insert cursor underscore at the correct position */
        if (editing_cursor <= 0) {
            strcat(display_string, "_");
            strcat(display_string, field);
        } else if (editing_cursor >= len) {
            strcat(display_string, field);
            strcat(display_string, "_");
        } else {
            strncat(display_string, field, editing_cursor);
            strcat(display_string, "_");
            strcat(display_string, field + editing_cursor);
        }

    } else {
        size = snprintf(NULL, 0, "%s: %s", field_name, field);
        display_string = (char*)malloc(size + 1); /* +1 for null terminator */
        if (display_string == NULL) {
            /* Handle memory allocation failure */
            return NULL;
        }

        /* Construct the display string without "(Editing)" */
        sprintf(display_string, "%s: %s", field_name,field);
    }

    display_string[size + 1] = '\0';

    return display_string;
}


void display_contact(WINDOW *win, Contact contact, char* filepath) {
    int highlight = 0;
    int num_properties = 6;
    int ch;
    char edited_value[LINESIZE] = { 0 };
    int editing_index = -1;
    int editing_cursor = -1;

    while (1) {
        wclear(win);
        box(win, 0, 0);

        for (int i = 0; i < num_properties; i++) {
            if (i == highlight) {
                wattron(win, A_STANDOUT);
            }

            if (i < num_properties) {
                switch (i) {
                    case 0:
                        ;
                        char* display_string = construct_field_display_string("First Name", contact.first_name, editing_index == 0, editing_cursor);
                        mvwprintw(win, 1, 1, "%s", display_string);
                        if (display_string) {
                            free(display_string);
                        }
                        break;
                    case 1:
                        if (editing_index == 1) {
                            mvwprintw(win, 2, 1, "(Editing) Last Name: %s_", contact.last_name);
                        }
                        else {
                            mvwprintw(win, 2, 1, "Last Name: %s", contact.last_name);
                        }
                        break;
                    case 2:
                        if (editing_index == 2) {
                            mvwprintw(win, 3, 1, "(Editing) Email: %s_", contact.email);
                        }
                        else {
                            mvwprintw(win, 3, 1, "Email: %s", contact.email);
                        }
                        break;
                    case 3:
                        if (editing_index == 3) {
                            mvwprintw(win, 4, 1, "(Editing) Phone: %s_", contact.phone);
                        }
                        else {
                            mvwprintw(win, 4, 1, "Phone: %s", contact.phone);
                        }
                        break;
                    case 4:
                        if (editing_index == 4) {
                            mvwprintw(win, 5, 1, "(Editing) Address: %s_", contact.address);
                        }
                        else {
                            mvwprintw(win, 5, 1, "Address: %s", contact.address);
                        }
                        break;
                    case 5:
                        if (editing_index == 5) {
                            mvwprintw(win, 6, 1, "(Editing) Notes: %s_", contact.notes);
                        }
                        else {
                            mvwprintw(win, 6, 1, "Notes: %s", contact.notes);
                        }
                        break;
                }
            }

            wattroff(win, A_STANDOUT);
        }

        wrefresh(win);

        ch = wgetch(win);

        if (editing_index != -1) {
            wrefresh(win);

            switch(ch) {
                case KEY_RIGHT:
                    editing_cursor++;
                    if (editing_cursor > (int)strlen(edited_value)) {
                        editing_cursor = strlen(edited_value);
                    }
                    break;
                case KEY_LEFT:
                    editing_cursor--;
                    if (editing_cursor < 0) {
                        editing_cursor = 0;
                    }
                    break;
                case 10:
                    /* save */
                    editing_index = -1;
                    editing_cursor = -1;

                    if ((int)strlen(edited_value) <= 0) {
                        edited_value[0] = ' ';
                        edited_value[1] = '\0';
                    }


                    editLine(contact, filepath, contact.index);
                    break;
                case KEY_BACKSPACE:
                    /* When backspacing, we have to decriment the position of each value after the cursor */
                    if ((int)strlen(edited_value) > 0 && edited_value[0] != '\0') {
                        for (int i = editing_cursor; i < (int)strlen(edited_value); i++) {
                            edited_value[i-1] = edited_value[i];
                        }
                        edited_value[strlen(edited_value) - 1] = '\0';
                        editing_cursor--;
                    }
                    break;
                default:
                    if ((int)strlen(edited_value) < LINESIZE) {

                        if (editing_cursor <= 0 && edited_value[0] == '\0') {
                            edited_value[0] = ' ';
                            edited_value[1] = '\0';
                        }

                        char new_value[LINESIZE] = { 0 };
                        strncpy(new_value, edited_value, editing_cursor);
                        new_value[editing_cursor] = ch; 
                        new_value[editing_cursor + 1] = '\0';
                        strncat(new_value, edited_value + editing_cursor, LINESIZE - strlen(new_value) - 1); 
                        strcpy(edited_value, new_value);

                        editing_cursor++;
                    }
                    break;
            }
        }
        else {
            switch(ch) {
                case KEY_UP:
                case 'k':
                    highlight--;
                    if (highlight < 0) {
                        highlight = num_properties - 1;
                    }
                    break;
                case KEY_DOWN:
                case 'j':
                    highlight++;
                    if (highlight >= num_properties) {
                        highlight = 0;
                    }
                    break;
                case 'l':
                case 10: /* Enter key */
                    editing_index = highlight;
                    /* init values for editing */
                    switch (highlight) {
                        case 0:
                            strcpy(edited_value, contact.first_name);
                            break;
                        case 1:
                            strcpy(edited_value, contact.last_name);
                            break;
                        case 2:
                            strcpy(edited_value, contact.email);
                            break;
                        case 3:
                            strcpy(edited_value, contact.phone);
                            break;
                        case 4:
                            strcpy(edited_value, contact.address);
                            break;
                        case 5:
                            strcpy(edited_value, contact.notes);
                            break;
                    }
                    editing_cursor = strlen(edited_value);
                    break;
                case 'q':
                case 'h':
                    wclear(win);
                    return;
            }
        }

        wrefresh(win);
        switch (editing_index) {
            case 0:
                strcpy(contact.first_name, edited_value);
                break;
            case 1:
                strcpy(contact.last_name, edited_value);
                break;
            case 2:
                strcpy(contact.email, edited_value);
                break;
            case 3:
                strcpy(contact.phone, edited_value);
                break;
            case 4:
                strcpy(contact.address, edited_value);
                break;
            case 5:
                strcpy(contact.notes, edited_value);
                break;
        }
        wrefresh(win);
    }
}


void tui_display_contacts(Contact contacts[], int num_contacts, char* filepath) {
    WINDOW *win, *winContacts;
    int highlight = 0;
    int start_contact = 0; // Index of the first contact to display
    int choice;
    int offset_lines = 5;
    int offset_cols = 2;

    initscr();
    cbreak(); 
    noecho(); 
    keypad(stdscr, TRUE); 

    win = newwin(LINES - offset_lines, COLS - offset_cols, 0, 0); 
    winContacts = newwin(LINES - offset_lines, COLS - offset_cols, 0, 0);

    if (win == NULL || winContacts == NULL) {
        fprintf(stderr, "Error creating window.\n");
        endwin();
        exit(EXIT_FAILURE);
    }
    keypad(win, TRUE);

    while (1) {
        wclear(win);
        box(win, 0, 0);

        /* render contacts */
        for (int i = start_contact; i < num_contacts && i < start_contact + LINES - 3; i++) {
            if (i == start_contact + highlight) {
                wattron(win, A_STANDOUT);
            }
            mvwprintw(win, i - start_contact + 1, 1, "%d. %s %s", contacts[i].index, contacts[i].first_name, contacts[i].last_name);
            wattroff(win, A_STANDOUT);
        }

        choice = wgetch(win);


        /* process user input */
        switch(choice) {
            case KEY_UP:
            case 'k':
                highlight--;
                if (start_contact == 0 && highlight < 0) {
                    /* if we scroll up at the top, set to the end */
                    highlight = LINES - offset_lines - 2;
                    start_contact = num_contacts - highlight - 1;
                }
                if (highlight < 0) {
                    highlight = 0;
                    if (start_contact > 0) {
                        start_contact--; 
                    }
                }
                break;
            case KEY_DOWN:
            case 'j':
                ;
                int cursor = start_contact + highlight;
                if (cursor < num_contacts - 1) {
                    highlight++;
                    if (cursor >= LINES - offset_lines - 2) {
                        start_contact++;
                        highlight--;
                    }
                }
                else {
                    start_contact = 0;
                    highlight = 0;
                }

                

                break;
            case 10: /* Enter key */
            case 'l':
                keypad(win, false);
                keypad(winContacts, true);
                refresh();
                display_contact(winContacts, contacts[start_contact + highlight], filepath);

                num_contacts = readContacts(contacts, filepath);

                wclear(win);
                wclear(winContacts);
                keypad(win, true);
                wrefresh(win);
                break;
            case 'q':
                endwin();
                return;
            case 'f':
                ;
                char* search_string = "edit4";
                num_contacts = searchContacts(contacts,num_contacts, search_string);
                break;
            case 'a':
                ;
                Contact default_contact = { "tmpFirstName", "tmpLastName", "example@email.com", "+44 38383","123 St Avenue, 11221","notes here", num_contacts };
                appendLine(default_contact, filepath);

                /* re-read contacts */
                num_contacts = readContacts(contacts, filepath);


                break;
            case 'd':
                deleteContact(start_contact + highlight,filepath);

                /* re-read contacts */
                num_contacts = readContacts(contacts, filepath);

                break;
        }
        wrefresh(win);
    }
}

