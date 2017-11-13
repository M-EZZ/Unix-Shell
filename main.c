#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 64

void main_loop();
char* read_line();
char** parse_line(char* line);
void execute_command(char**);

int main() {

    main_loop(); //Read , Parse , Execute.

    //TODO  Add exit commands

    return 0;
}

void main_loop (){
    char* line;
    char** arguments;

    while (1){

        printf("shell> ");

        //Read
        line = read_line();

        //Parse
        arguments = parse_line(line);

        //Execute
        //execute_command(arguments);
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