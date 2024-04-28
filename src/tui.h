#ifndef TUI
#define TUI

#include "tui.c"
#include "shared.h"


void display_contacts(Contact contacts[], int num_contacts, void (*update_callback)(Contact));

#endif

