# better_address_book
A better address book written in C


##

Currently unmained

TODO:
* Remove all memory bugs
* Add TUI searching
* Add field specific searching to CLI

### Command line Useage

```bash
  ____       _   _                 _       _     _
 | __ )  ___| |_| |_ ___ _ __     / \   __| | __| |_ __ ___  ___ ___
 |  _ \ / _ \ __| __/ _ \ '__|   / _ \ / _` |/ _` | '__/ _ \/ __/ __|
 | |_) |  __/ |_| ||  __/ |     / ___ \ (_| | (_| | | |  __/\__ \__ \
 |____/ \___|\__|\__\___|_|    /_/   \_\__,_|\__,_|_|  \___||___/___/
  ____              _
 | __ )  ___   ___ | | __
 |  _ \ / _ \ / _ \| |/ /
 | |_) | (_) | (_) |   <
 |____/ \___/ \___/|_|\_\

-a or --add to add a new contact
-r or --remove to remove a contact by index
-s or --show to show all contacts
-f or --find to find potential matches 
--field to specify fields to show when using -f
-i or --inspect to inspect a specific contact by index
```

### Graphic Useage

Vim keybindings for movement

#### Add new contact 
Press a when highlighting the contact you'd like to delete
#### Remove contact 
Press d when highlighting the contact you'd like to delete
#### Search contacts 
Press f when on the contacts screen and type in a search term
#### Edit contact fields
Press enter or l on a contact to view it, then enter or l to edit a specific field

You can use arrowkeys to move cursor within a string


### Technical notes
* All contacts are stored in a csv that is parsed using my [CSV parsed](https://github.com/SherllyNeo/dsvParser/tree/main)
* All of this is just a nice interface for interacting with the csv file, meaning your contacts are portable 






