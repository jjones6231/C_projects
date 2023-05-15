#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//define some global variables to make reading the code easier
#define COMMENT_START "/*"
#define COMMENT_END "*/"
#define SINGLE_LINE_COMMENT "//"
#define NOT_COMMENTED 0
#define COMMENTED 1
#define COMMENT_STARTED 2

FILE* to_strip;
FILE* output_file;
int status;

//handle the logic for when to strip and when not to strip comments from the file
int check(char* to_parse, int status, size_t len){
    char* substring;
    if(status == NOT_COMMENTED){
        substring = strstr(to_parse, COMMENT_START); 
        if(substring != NULL) {
            //special status, when a comment is started, an end should also be checked for in the same line
            status = COMMENT_STARTED; 
            len = (size_t)(substring-to_parse); //calculate the offset for before the comment starts 
            goto write;
        }
        substring = strstr(to_parse, SINGLE_LINE_COMMENT);
        if(substring != NULL) {
            len = (size_t)(substring-to_parse)+1;
            memset(substring, '\n', 1); //replace the comment with a new line
            goto write;
        }
    } else if(status == COMMENTED){
        substring = strstr(to_parse, COMMENT_END);
        if(substring != NULL){
            len = len-((size_t)(substring-to_parse)+2);
            status = NOT_COMMENTED;
            to_parse = substring+2;
            goto write;
        } else {
            return status;
        }
    }
write:
    fwrite(to_parse, 1, len, output_file);  //write the uncommented section to the output file 
    return status;
}

/*
 * this function parses the entire file and loads it into the checker function
 * this is also a multiline comment
 */
void parse_loop(){
    char* line_read = NULL;
    size_t len;
    ssize_t read_bytes;
    uint filter_status = NOT_COMMENTED;

    while((read_bytes = getline(&line_read, &len, to_strip)) != -1){
        filter_status = check(line_read, filter_status, read_bytes);

        //if a comment is started, check again for a finishing phrase
        if(filter_status == COMMENT_STARTED) { 
            filter_status = COMMENTED;
            filter_status = check(line_read, filter_status, read_bytes);
        }
        memset(line_read, 0, len);
    }

    if(line_read)
        free(line_read);
    return;
}

/*
 * main function that handles file initiaztion and 
 * starts the loop which parses the program
 */
int main(int argc, char* argv[]){
    char output_filename[0x20];

    if(argc == 1){
        puts("usage ./comment_stripper <FILENAME>");
        status = 1;
        goto exit;
    }

    to_strip = fopen(argv[1], "r");
    if(to_strip == NULL) {
        printf("[!] Error opening file %s\n",argv[1]);
        status = 1;
        goto exit;
    }
    printf("[*] Opened input file %s\n", argv[1]);

    memset(output_filename,0,0x20); 
    strncpy(output_filename,argv[1],0x1a);
    strcat(output_filename, ".str"); /*comment on a single line*/
    output_file = fopen(output_filename, "w");
    printf("[*] created output file %s\n", output_filename);

    parse_loop();

    status = 0;
exit:
    if(output_file)
        fclose(output_file);
    if(to_strip)
        fclose(to_strip);
    return status;
}

