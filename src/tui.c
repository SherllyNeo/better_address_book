#include <ncurses.h>
#include <stdio.h>
#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

#define CHARWIDTH 100


int construct_field_display_string(char display_string[],size_t size,char* field_name, char* field, bool editing, int editing_cursor) {
    int len = strlen(field);
    memset(display_string,0,(size+1)*sizeof(char));

    /* Determine the size of the string */
    if (editing) {
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
        /* Construct the display string without "(Editing)" */
        sprintf(display_string, "%s: %s", field_name,field);
    }

    display_string[size + 1] = '\0';

    return 0;
}

int construct_finding_string(char finding_string[],size_t size, char* search_term, int editing_cursor) {
    int len = strlen(search_term);
    memset(finding_string,0,(size+1)*sizeof(char));

    sprintf(finding_string, "Searching: ");
    /* Insert cursor underscore at the correct position */
    if (editing_cursor <= 0) {
        strcat(finding_string, "_");
        strcat(finding_string, search_term);
    } else if (editing_cursor >= len) {
        strcat(finding_string, search_term);
        strcat(finding_string, "_");
    } else {
        strncat(finding_string, search_term, editing_cursor);
        strcat(finding_string, "_");
        strcat(finding_string, search_term + editing_cursor);
    }


    finding_string[size + 1] = '\0';

    return 0;
}


void display_contact(WINDOW *win, Contact contact, char* filepath,WINDOW *winOutput) {
    int highlight = 0;
    int num_properties = 6;
    int ch;
    char edited_value[LINESIZE] = { 0 };
    int editing_index = -1;
    int editing_cursor = -1;

    while (1) {
        wclear(win);
        wclear(winOutput);
        box(win, 0, 0);
        box(winOutput, 0, 0);
        wrefresh(winOutput);
        wrefresh(win);

        if (editing_index != -1) {
            mvwprintw(winOutput, 0, 1, "press enter to save change");
            mvwprintw(winOutput, 1, 1, "Move cursor with arrow keys");
            if ((int)strlen(edited_value) <= 0) {
                mvwprintw(winOutput, 2, 1, "Cannot be empty");
            }
            else {
                mvwprintw(winOutput, 2, 1, "changing to \"%s\"",edited_value);
            }
        }
        else {
            mvwprintw(winOutput, 0, 1, "press enter to edit a field");
        }

        wrefresh(winOutput);
        for (int i = 0; i < num_properties; i++) {
            if (i == highlight) {
                wattron(win, A_STANDOUT);
            }

            /* print lines */
            switch (i) {
                case 0:
                    ;
                    {
                    /* Displaying First Name */
                    size_t size_fn = snprintf(NULL, 0, "(Editing) %s: %s", "First name", contact.first_name);
                    char display_string_first_name[size_fn + 1]; 
                    int failed_to_generate_string_for_first_name = construct_field_display_string(display_string_first_name, size_fn, "First Name", contact.first_name, editing_index == 0, editing_cursor);
                    if (failed_to_generate_string_for_first_name) {
                        fprintf(stderr, "Failed to generate string for first name");
                    }
                    mvwprintw(win, 1, 1, "%s", display_string_first_name);
                    break;
                    }

                case 1:
                    ;
                    {
                    /* Displaying Last Name */
                    size_t size_ln = snprintf(NULL, 0, "(Editing) %s: %s", "Last Name", contact.last_name);
                    char display_string_last_name[size_ln + 1];
                    int failed_to_generate_string_for_last_name = construct_field_display_string(display_string_last_name, size_ln, "Last Name", contact.last_name, editing_index == 1, editing_cursor);
                    if (failed_to_generate_string_for_last_name) {
                        fprintf(stderr, "Failed to generate string for last name");
                    }
                    mvwprintw(win, 2, 1, "%s", display_string_last_name);
                    break;
                    }

                case 2:
                    ;
                    {
                    /* Displaying Email */
                    size_t size_e = snprintf(NULL, 0, "(Editing) %s: %s", "Email", contact.email);
                    char display_string_email[size_e + 1];
                    int failed_to_generate_string_for_email = construct_field_display_string(display_string_email, size_e, "Email", contact.email, editing_index == 2, editing_cursor);
                    if (failed_to_generate_string_for_email) {
                        fprintf(stderr, "Failed to generate string for email");
                    }
                    mvwprintw(win, 3, 1, "%s", display_string_email);
                    break;
                    }

                case 3:
                    ;
                    {
                    /* Displaying Phone */
                    size_t size_p = snprintf(NULL, 0, "(Editing) %s: %s", "Phone", contact.phone);
                    char display_string_phone[size_p + 1];
                    int failed_to_generate_string_for_phone = construct_field_display_string(display_string_phone, size_p, "Phone", contact.phone, editing_index == 3, editing_cursor);
                    if (failed_to_generate_string_for_phone) {
                        fprintf(stderr, "Failed to generate string for phone");
                    }
                    mvwprintw(win, 4, 1, "%s", display_string_phone);
                    break;
                    }
                case 4:
                    ;
                    {
                    /* Displaying Address */
                    size_t size_a = snprintf(NULL, 0, "(Editing) %s: %s", "Address", contact.address);
                    char display_string_address[size_a + 1];
                    int failed_to_generate_string_for_address = construct_field_display_string(display_string_address, size_a, "Address", contact.address, editing_index == 4, editing_cursor);
                    if (failed_to_generate_string_for_address) {
                        fprintf(stderr, "Failed to generate string for address");
                    }
                    mvwprintw(win, 5, 1, "%s", display_string_address);
                    break;
                    }

                case 5:
                    ;
                    {
                    /* Displaying Notes */
                    size_t size_n = snprintf(NULL, 0, "(Editing) %s: %s", "Notes", contact.notes);
                    char display_string_notes[size_n + 1];
                    int failed_to_generate_string_for_notes = construct_field_display_string(display_string_notes, size_n, "Notes", contact.notes, editing_index == 5, editing_cursor);
                    if (failed_to_generate_string_for_notes) {
                        fprintf(stderr, "Failed to generate string for notes");
                    }
                    mvwprintw(win, 6, 1, "%s", display_string_notes);
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
                    if ((int)strlen(edited_value) <= 0) {
                        break;
                    }

                    editing_index = -1;
                    editing_cursor = -1;



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
                    if ((int)strlen(edited_value) < CHARWIDTH && (isalnum(ch) || ch == ' ' || ch == '\t' || ch == '\n' || ch == '.'|| ch == '@' || ch == ',' || ch == '\'' ) ) {

                        if (editing_cursor <= 0 && edited_value[0] == '\0') {
                            edited_value[0] = '\0';
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
    WINDOW *win, *winContacts, *winOutput;
    int highlight = 0;
    int start_contact = 0; // Index of the first contact to display
    int choice;
    int offset_lines = 10;
    int offset_cols = 2;
    int finding = false;
    char finding_value[LINESIZE] = { 0 };
    int finding_cursor = -1;


    initscr();
    cbreak(); 
    noecho(); 
    keypad(stdscr, TRUE); 

    win = newwin(LINES - offset_lines, COLS - offset_cols, 0, 0); 
    winOutput = newwin(offset_lines, COLS - offset_cols, LINES - offset_lines, 0);
    winContacts = newwin(LINES - offset_lines, COLS - offset_cols, 0, 0);

    if (win == NULL || winContacts == NULL) {
        fprintf(stderr, "Error creating window.\n");
        endwin();
        exit(EXIT_FAILURE);
    }
    keypad(win, TRUE);

    while (1) {
        wclear(win);
        wclear(winOutput);
        box(win, 0, 0);
        box(winOutput, 0, 0);
        wrefresh(winOutput);
        wrefresh(win);


        if (!finding) {
            int cur = highlight + start_contact;
            mvwprintw(winOutput, 0, 1, "Press enter to select/edit");
            mvwprintw(winOutput, 1, 1, "Name: %s %s, Email: %s, Phone: %s",contacts[cur].first_name,contacts[cur].last_name,contacts[cur].email,contacts[cur].phone);
            mvwprintw(winOutput, 2, 1, "Address %s",contacts[cur].address);
            mvwprintw(winOutput, 4, 2, "Press a to add a new contact");
            mvwprintw(winOutput, 5, 2, "Press d to delete %s %s",contacts[cur].first_name,contacts[cur].last_name);
            mvwprintw(winOutput, 6, 2, "Press f to find a contact");
            mvwprintw(winOutput, 8, 1, "number of contacts: %d",num_contacts);
            mvwprintw(winOutput, 9, 1, "Babook");
        }
        else {
            mvwprintw(winOutput, 0, 1, "Searching (Found %d)",num_contacts);


            size_t size_fn = snprintf(NULL, 0, "(Searching): %s",finding_value);
            char display_string_search[size_fn + 1]; 
            int failed_to_generate_string_for_search = construct_finding_string(display_string_search, size_fn, finding_value, finding_cursor);
            if (failed_to_generate_string_for_search) {
                fprintf(stderr, "Failed to generate string for search term\n");
            }
            mvwprintw(winOutput, 1, 1, "%s", display_string_search);
            mvwprintw(winOutput, 8, 1, "searching all fields apart from notes, including index",num_contacts);
        }

        wrefresh(winOutput);

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
        if (finding) {
            wrefresh(win);

            switch(choice) {
                case KEY_RIGHT:
                    finding_cursor++;
                    if (finding_cursor > (int)strlen(finding_value)) {
                        finding_cursor = strlen(finding_value);
                    }
                    break;
                case KEY_LEFT:
                    finding_cursor--;
                    if (finding_cursor < 0) {
                        finding_cursor = 0;
                    }
                    break;
                case 'f':
                    /* stop finding and refresh */
                    finding = false;
                    finding_cursor = -1;

                    /* re-read contacts */
                    num_contacts = readContacts(contacts, filepath);

                    break;
                case 10:
                    /* stop finding */
                    finding = false;
                    finding_cursor = -1;

                    if (strlen(finding_value) <= 0) {
                        num_contacts = readContacts(contacts,filepath);
                    }

                    break;
                case KEY_BACKSPACE:
                    /* When backspacing, we have to decriment the position of each value after the cursor */
                    if ((int)strlen(finding_value) > 0 && finding_value[0] != '\0') {
                        for (int i = finding_cursor; i < (int)strlen(finding_value); i++) {
                            finding_value[i-1] = finding_value[i];
                        }
                        finding_value[strlen(finding_value) - 1] = '\0';
                        finding_cursor--;
                    }
                    break;
                default:
                    if ((int)strlen(finding_value) < CHARWIDTH && (isalnum(choice) || choice == ' ' || choice == '\t' || choice == '\n' || choice == '.'|| choice == '@' || choice == ',' || choice == '\'' ) ) {

                        char new_value[LINESIZE] = { 0 };
                        strncpy(new_value, finding_value, finding_cursor);
                        new_value[finding_cursor] = choice; 
                        new_value[finding_cursor + 1] = '\0';
                        strncat(new_value, finding_value + finding_cursor, LINESIZE - strlen(new_value) - 1); 
                        strcpy(finding_value, new_value);

                        finding_cursor++;
                        if (strlen(finding_value) > 0 ) {
                            /* have to reread when searching */
                            num_contacts = readContacts(contacts,filepath);
                            num_contacts = searchContacts(contacts,num_contacts, finding_value);
                        }
                        highlight = 0;
                        start_contact = 0;
                    }
                    break;
            }
        }
        else {
            switch(choice) {
                case KEY_UP:
                case 'k':
                    highlight--;
                    if (start_contact == 0 && highlight < 0) {
                        highlight = 0;
                        start_contact = 0;
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
                    finding = false;
                    keypad(win, false);
                    keypad(winContacts, true);
                    refresh();
                    display_contact(winContacts, contacts[start_contact + highlight], filepath,winOutput);

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
                    finding = true;
                    finding_cursor = strlen(finding_value);
                    break;
                case 'a':
                    ;
                    if (num_contacts < MAX_CONTACTS - 5) {
                        Contact default_contact = { "tmpFirstName", "tmpLastName", "example@email.com", "+44 38383","123 St Avenue, 11221","notes here", num_contacts };
                        appendLine(default_contact, filepath);

                        /* re-read contacts */
                        num_contacts = readContacts(contacts, filepath);
                    }
                    break;
                case 'd':
                    deleteContact(start_contact + highlight,filepath);

                    /* re-read contacts */
                    num_contacts = readContacts(contacts, filepath);
                    break;
            }
        }
        wrefresh(win);
    }
}

