#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The command pdf filename.pdf will open the pdf
// perror("message") prints the error message on terminal where it is called. It is in stdio.h
// system() is used to execute shell commands, it is in stdlib.h
// unistd.h provides access to POSIX OS API (whatever it is idk), its just a good practice when working with processes

int main(int argc, char *argv[]) {

  // argc (argument count) stores the number of arguments passed to program
  // argv (argument vector) an array of strings (hence * and []) where argv[0] is the program name (name.out which comes after compliling this code) and argv[1] is the PDF path

    if (argc == 2) {

    // expecting two arguements, first filename.out and seocnd the PDF file path
        char command[256];
        snprintf(command, sizeof(command), "zathura '%s' & disown", argv[1]);

    // prints "zathura filename & disown" with filename = argv[1] in variable command after allocating the meory sizeof(command) (256) 

        int status = system(command);
        if (status == -1) {
            perror("Error executing command");
            return 1;
        }
    } else {
        printf("USAGE : pdf [PDF PATH]\n");

    // prints this if the pdf successfully opens
    }
    return 0;
}

//Run gcc -o pdf pdf.c. A pdf (pdf.out) will be created.
//Copy this to ~/.local/bin using cp ~/.local/bin
//Open zshrc using nvim ~/.zshrc
//Write the code export PATH=$PATH:/home/abhimanyukoushik07/.local/bin Here you are basically add the path to local binary folder to PATH to tell zsh (the shell) to go and search in that folder in case you find something unknown
//To source something you run the command source ~/.zshrc (The configuration file, but zsh is the shell) or you can restart the device
