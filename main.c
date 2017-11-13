#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 64

void main_loop();
char* read_line();
char** parse_line(char*);
int execute_command(char**);
void launch_shell(char**);
int number_of_builtins();
//builtin shell commands
int cd (char**);
int Exit (char**);


char* builtin_str [] = {        // Have to be builtins to manipulate the current (parent) process.
        "cd",
        "exit"
};

int(*builtin_function[]) (char**) = {   // An array of function pointers that take an array of strings,
        &cd,                            // and return an int
        &Exit
};


int main() {

    main_loop(); //Read , Parse , Execute.

    return 0;
}

void main_loop (){
    char* line;
    char** arguments;
    int status = 1;

    while (status){

        printf("shell> ");

        //Read
        line = read_line();

        //Parse
        arguments = parse_line(line);

        //Execute
        status = execute_command(arguments);
    }
}

char* read_line(){
    int buffer_size = BUFFER_SIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer) {
        fprintf(stderr, "Buffer allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        //If command line exceeded 512 characters print error message.
        if(position >= 512){
            fprintf(stderr , "A very long command line (over 512 characters\n");
            buffer[0] = '\0';
            while ( getchar() != '\n' ); // Get to the end of the stream to skip the long command.
            return buffer;
        }

        // If we have exceeded the buffer, reallocate.
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "Buffer allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}

char** parse_line(char* line){
    int buffer_size = BUFFER_SIZE;
    int position = 0;
    char** tokens = malloc(sizeof(char*) * buffer_size);
    char* token;

    if(!tokens){
        fprintf(stderr , "Tokens allocation error\n");
        exit(EXIT_FAILURE);
    }

    //TODO edit the delimiters.
    token = strtok(line , " ");
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= buffer_size){
            buffer_size += BUFFER_SIZE;
            tokens = realloc(tokens, sizeof(char*) * buffer_size);
            if(!tokens){
                fprintf(stderr , "Tokens allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL , " ");
    }
    tokens[position] = NULL;
    return tokens;
}

int execute_command(char** arguments){
    if(arguments[0] == NULL){   // Empty command line.
        return 1;
    }

    for( int i=0 ; i<number_of_builtins() ; i++){
        if(strcmp(arguments[0] , builtin_str[i]) == 0){
            return (*builtin_function[i])(arguments);
        }
    }
    launch_shell(arguments);
    return 1; // To keep the main loop from breaking.
}

void launch_shell(char** arguments){
    pid_t pid ;
    int status;

    pid = fork();

    if(pid == 0){       // Child process
        if( execvp( arguments[0] , arguments) == -1){
            perror("execvp FAILED, A command doesn't exist or can't be executed");
        }
        exit(EXIT_FAILURE);
    }
    else if( pid < 0){
        perror("Fork FAILED");
    }
    else{               // Parent process
        do{
            waitpid( pid , &status , WUNTRACED);
        }
        while( !WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

int cd (char** arguments){
    if(arguments [1] == NULL){
        fprintf(stderr , "Expexted argument to \"cd\"\n");
    }
    else{
        if(chdir(arguments[1]) != 0){
            perror("chdir FAILED");
        }
    }
    return 1;
}

int Exit(char** arguments){
    return 0;
}

int number_of_builtins(){
    return sizeof(builtin_str) / sizeof(char*);
}