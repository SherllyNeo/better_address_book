#include <ncurses.h>
#include "shared.h"
#include "string.h"
#include "stdlib.h"

// Function to display a contact
void display_contact(WINDOW *win, Contact contact, void (*update_callback)(Contact)) {
    int highlight = 0;
    int num_properties = 4;
    int ch;
    bool editing = false;
    bool choosing_property = false; // Flag to indicate if the user is choosing a property to edit
    char edited_value[200]; // Buffer for edited value
    int chosen_property = -1; // Index of the chosen property to edit

    while (1) {
        wclear(win);
        box(win, 0, 0);

        // Display contact properties within the visible area
        for (int i = 0; i <= num_properties; i++) {
            if (i == highlight) {
                wattron(win, A_STANDOUT);
            }

            if (i < num_properties) {
                switch (i) {
                    case 0:
                        mvwprintw(win, 1, 1, "Name: %s %s", contact.first_name, contact.last_name);
                        break;
                    case 1:
                        mvwprintw(win, 2, 1, "Email: %s", contact.email);
                        break;
                    case 2:
                        mvwprintw(win, 3, 1, "Phone: %s", contact.phone);
                        break;
                    case 3:
                        mvwprintw(win, 4, 1, "Address: %s, %s, %s, %s, %s, %s", contact.address_line1, contact.address_line2, contact.city, contact.state, contact.post_code, contact.country);
                        break;
                    case 4:
                        mvwprintw(win, 5, 1, "Notes: %s", contact.notes);
                        break;
                }
            }

            wattroff(win, A_STANDOUT);
        }

         if (editing) {
            mvwprintw(win, num_properties + 3, 1, "Editing %s: %s_", editing == 1 ? "value" : "field", edited_value);
            mvwprintw(win, num_properties + 2, 1, "Press F1 to save");
        }

        if (choosing_property) {
            switch (highlight) {
                case 0:
                    mvwprintw(win, num_properties + 5, 1, "Choose a property to edit, 0 - FirstName, 1 - LastName: ");
                    break;
                case 3:
                    mvwprintw(win, num_properties + 5, 1, "Choose a property to edit, 0 - Address Line 1, 1 - Address Line 2, 2 - City, 3 - Sate, 4 - Post code, 5 - Country: ");
                    break;
            }
            wrefresh(win);
            int property_choice = wgetch(win) - '0'; // Convert character input to integer
            if (property_choice >= 1 && property_choice <= num_properties) {
                chosen_property = property_choice - 1; // Adjust index to match array indexing
                choosing_property = false;
                editing = true;
                // Initialize edited value buffer with the current value of the chosen property
                switch (highlight) {
                    case 0:
                        if (property_choice == 0) {
                            strcpy(edited_value, contact.first_name);
                        }
                        else if (property_choice == 1) {
                            strcpy(edited_value, contact.last_name);
                        }
                        mvwprintw(win, num_properties + 5, 1, "Choose a property to edit, 0 - FirstName, 1 - LastName: %d",property_choice);
                        break;
                    case 1:
                        strcpy(edited_value, contact.email);
                        break;
                    case 2:
                        strcpy(edited_value, contact.phone);
                        break;
                    case 3:
                        if (property_choice == 0) {
                            strcpy(edited_value, contact.address_line1);
                        }
                        else if (property_choice == 1) {
                            strcpy(edited_value, contact.address_line2);
                        }
                        else if (property_choice == 2) {
                            strcpy(edited_value, contact.city);
                        }
                        else if (property_choice == 3) {
                            strcpy(edited_value, contact.state);
                        }
                        else if (property_choice == 4) {
                            strcpy(edited_value, contact.post_code);
                        }
                        else if (property_choice == 5) {
                            strcpy(edited_value, contact.country);
                        }
                        mvwprintw(win, num_properties + 5, 1, "Choose a property to edit, 0 - Address Line 1, 1 - Address Line 2, 2 - City, 3 - Sate, 4 - Post code, 5 - Country: %d",property_choice);
                        break;
                    case 4:
                        strcpy(edited_value, contact.notes);
                        break;
                }
            } else {
                // Invalid property choice, reset choosing_property flag
                choosing_property = false;
            }
        }

        wrefresh(win);

        // Get user input
        ch = wgetch(win);

        // Process user input
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
            case 'l':
                if (!editing && !choosing_property) {
                    // Enter choosing property mode
                    choosing_property = true;
                } else if (choosing_property) {
                    // Cancel choosing property mode
                    choosing_property = false;
                }
                break;
            case KEY_F(1): 
                if (editing) {
                    // Exit editing mode
                    editing = false;
                    // Save the edited value
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
                            strcpy(contact.address_line1, edited_value);
                            break;
                        case 4:
                            strcpy(contact.notes, edited_value);
                            break;
                    }
                    // Call the update callback
                    update_callback(contact);
                }
                chosen_property = -1;
                break;
            case 'q':
            case 'h':
                wclear(win);
                return;
            case KEY_BACKSPACE:
                // If in editing mode and edited value is not empty
                if (editing && strlen(edited_value) > 0) {
                    // Erase the last character from the edited value
                    edited_value[strlen(edited_value) - 1] = '\0';
                }
                break;
            default:
            // If in editing mode, update edited value
                if (editing) {
                    if (strlen(edited_value) < 199) {
                        // Append typed character to edited value
                        strncat(edited_value, &ch, 1);
                    }
                }
                break;
        }

        wrefresh(win);
    }
}


void display_contacts(Contact contacts[], int num_contacts, void (*update_callback)(Contact)) {
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
                display_contact(winContacts, contacts[start_contact + highlight], update_callback);
                wclear(win);
                wclear(winContacts);
                keypad(win, true);
                wrefresh(win);
                break;
            case 'q':
                endwin();
                return;
        }
        wrefresh(win);
    }
}

