#include <ncurses.h>
#include "shared.h"
#include "file.h"
#include "contacts.h"
#include "string.h"
#include "stdlib.h"




void display_contact(WINDOW *win, Contact contact, char* filepath) {
    int highlight = 0;
    int num_properties = 6;
    char ch;
    bool editing = false;
    char edited_value[LINESIZE] = { 0 };
    char edited_field[LINESIZE] = { 0 };

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
                        mvwprintw(win, 1, 1, "First Name: %s", contact.first_name);
                        break;
                    case 1:
                        mvwprintw(win, 2, 1, "Last Name: %s", contact.last_name);
                        break;
                    case 2:
                        mvwprintw(win, 3, 1, "Email: %s", contact.email);
                        break;
                    case 3:
                        mvwprintw(win, 4, 1, "Phone: %s", contact.phone);
                        break;
                    case 4:
                        mvwprintw(win, 5, 1, "Address: %s", contact.address);
                        break;
                    case 5:
                        mvwprintw(win, 6, 1, "Notes: %s", contact.notes);
                        break;
                }
            }

            wattroff(win, A_STANDOUT);
        }

        if (editing) {
            switch (highlight) {
                case 0:
                    strcpy(edited_value, contact.first_name);
                    strcpy(edited_field,"first_name");
                    break;
                case 1:
                    strcpy(edited_value, contact.last_name);
                    strcpy(edited_field,"last_name");
                    break;
                case 2:
                    strcpy(edited_value, contact.email);
                    strcpy(edited_field,"email");
                    break;
                case 3:
                    strcpy(edited_value, contact.phone);
                    strcpy(edited_field,"phone");
                    break;
                case 4:
                    strcpy(edited_value, contact.address);
                    strcpy(edited_field,"address");
                    break;
                case 5:
                    strcpy(edited_value, contact.notes);
                    strcpy(edited_field,"notes");
                    break;
            }
        }

        wrefresh(win);
        


        wrefresh(win);

        // Get user input
        ch = wgetch(win);

        if (editing) {
            mvwprintw(win, num_properties + 3, 1, "Editing %s: %s_", edited_field, edited_value);
            mvwprintw(win, num_properties + 2, 1, "Press enter to save");
        }

        // Process user input
        if (editing) {
            switch(ch) {
                case 10:
                    /* save */
                    editing = false;

                    switch (highlight) {
                        case 0:
                            strcpy(contact.first_name, edited_value);
                            break;
                        case 1:
                            strcpy(contact.email, edited_value);
                            break;
                        case 2:
                            strcpy(contact.phone, edited_value);
                            break;
                        case 3:
                            strcpy(contact.address, edited_value);
                            break;
                        case 4:
                            strcpy(contact.notes, edited_value);
                            break;
                    }
                    editLine(contact, filepath, contact.index);
                    break;
                case KEY_BACKSPACE:
                    if (editing && strlen(edited_value) > 0) {
                        edited_value[strlen(edited_value) - 1] = '\0';
                    }
                    break;
                default:
                    if (strlen(edited_value) < LINESIZE) {
                        strncat(edited_value, &ch, 1);
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
                case 10: /* Enter key */
                    editing = true;
                case 'l':
                    editing = true;
                    break;
                case 'q':
                case 'h':
                    wclear(win);
                    return;
            }
        }

        wrefresh(win);
    }
}


void tui_display_contacts(Contact contacts[], int num_contacts, char* filepath) {
    WINDOW *win, *winContacts;
    int highlight = 0;
    int start_contact = 0; // Index of the first contact to display
    int choice;

    // Initialize ncurses
    initscr();
    cbreak(); // Line buffering disabled
    noecho(); // Don't echo user input
    keypad(stdscr, TRUE); // Enable keypad for function keys

    // Create main window
    win = newwin(LINES - 3, COLS - 2, 0, 0); // Adjust window size
    winContacts = newwin(LINES - 3, COLS - 2, 0, 0); // Adjust window size

    if (win == NULL || winContacts == NULL) {
        fprintf(stderr, "Error creating window.\n");
        endwin();
        exit(EXIT_FAILURE);
    }
    keypad(win, TRUE);

    while (1) {
        wclear(win);
        box(win, 0, 0);

        // Display contacts within the visible area
        for (int i = start_contact; i < num_contacts && i < start_contact + LINES - 3; i++) {
            if (i == start_contact + highlight) {
                wattron(win, A_STANDOUT);
            }
            mvwprintw(win, i - start_contact + 1, 1, "%d. %s %s", contacts[i].index, contacts[i].first_name, contacts[i].last_name);
            wattroff(win, A_STANDOUT);
        }

        // Get user input
        choice = wgetch(win);

        // Process user input
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
                if (highlight >= LINES - 3 || start_contact + highlight >= num_contacts) {
                    highlight = LINES - 4;
                    if (start_contact + LINES - 3 < num_contacts) {
                        start_contact++; // Scroll down
                    }
                }
                break;
            case 10: /* Enter key */
            case 'l':
                // Display the selected contact
                keypad(win, false);
                keypad(winContacts, true);
                refresh();
                display_contact(winContacts, contacts[start_contact + highlight], filepath);
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
                Contact default_contact = { 0 };
                appendLine(default_contact, filepath);

                /* re-read contacts */
                //readContacts(contacts, filepath);

                break;
        }
        wrefresh(win);
    }
}

