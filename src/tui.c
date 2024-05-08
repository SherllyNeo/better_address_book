#include <ncurses.h>
#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "string.h"
#include "stdlib.h"




void display_contact(WINDOW *win, Contact contact, char* filepath) {
    int highlight = 0;
    int num_properties = 6;
    int ch;
    char edited_value[LINESIZE] = { 0 };
    int editing_index = -1;

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
                        if (editing_index == 0) {
                            mvwprintw(win, 1, 1, "(Editing) First Name: %s_", contact.first_name);
                        }
                        else {
                           mvwprintw(win, 1, 1, "First Name: %s", contact.first_name);
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
                case 10:
                    /* save */
                    editing_index = -1;

                    editLine(contact, filepath, contact.index);
                    break;
                case KEY_BACKSPACE:
                    if (strlen(edited_value) > 0) {
                        edited_value[strlen(edited_value) - 1] = '\0';
                    }
                    break;
                default:
                    if (strlen(edited_value) < LINESIZE) {
                        edited_value[strlen(edited_value)] = ch;
                        edited_value[strlen(edited_value) + 1] = '\0';
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
        mvwprintw(win, start_contact + num_contacts + 2, 1, "number of contacts: %d", num_contacts);

        choice = wgetch(win);


        /* process user input */
        switch(choice) {
            case KEY_UP:
            case 'k':
                highlight--;
                if (highlight < 0) {
                    highlight = 0;
                    if (start_contact > 0) {
                        start_contact--; // Scroll up
                    }
                }
                break;
            case KEY_DOWN:
            case 'j':
                highlight++;
                if (highlight >= LINES - offset_lines || start_contact + highlight >= num_contacts ) {
                    highlight = num_contacts - 1;
                    if (start_contact + LINES - offset_lines < num_contacts) {
                        start_contact++; 
                    }
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

