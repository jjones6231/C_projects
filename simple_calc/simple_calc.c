#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>


//define globals
int status = 0;
char* command;
char* command_current_location;

//helper function to print errors and clean up mallocs
void print_error(char* debug_message){
    printf("[!] %s\n", debug_message);
    if(command) free(command);
    exit(-1);
}


void print_longlong_res(int64_t result){
    printf("[*] Result: %ld\n", result);
    return;
}

void print_float_res(double res){
    printf("[*] Result: %lf\n", res);
    return;
}

/*
 * I wanted to practice parsing user input using different methods
 * while a sprintf(&uint, "%d\n", command); could work, I wanted to practice with strstr and strtoll
 * it is for the same reason that while reading the stdin input, I do not do any parsing there. 
 * A much more effective way would be create 2 different strings to store the ints and loop until a space
 * this does come with it's own limitations. 
 * However, and you need to keep track of the count of bytes read to gaurd against a buffer overflow 
 *
 * TLDR: I wanted to learn more about strtoll and strstr
 */
int64_t parse_int64_from_command(char* start_ptr){
    char* end_ptr;
    char* temp;
    char* temp_end;
    uint len = 0;
    int64_t ret = 0;
    uint check = 1;

    if((end_ptr = strstr(start_ptr, " ")) == NULL){
        temp = start_ptr;
        check = 0;
        goto convert; //end of the command, so try to convert
    }

    len = end_ptr-start_ptr;
    temp = malloc(len);

    //clear out remaining ptr values if there are any 0x20 is used as it the minimum size of a heap chunk
    memset(temp, 0, 0x20); 
    
    memcpy(temp, start_ptr, len); 
    temp_end = (temp+len)+1;

convert:
    ret = strtoll(temp, &temp_end,10);
    if(temp_end[0] != '\x00'){
        print_error("failed to convert number to a long long");
    }

    if(check) free(temp);
    command_current_location = end_ptr+1; //set the parse pointer one past, this clears out the space

    return ret;
}

void program_loop(){
    //initilaize all values
    char c;
    uint count;
    uint len;
    int64_t num1;
    int64_t num2;
    char operation;

    puts("Simple calculator usage: <number1> <operation> <number2>\nenter !exit to quit");
    while(1){
        //set the staring values
        c = 0;
        operation = 0;
        count = 0;
        len = 0;
        num1 = 0;
        num2 = 0;
        memset(command, 0, 0x400); //clear out the allocated space
        command_current_location = command; 

        printf("> ");
        //read all of the information, excluding the newline at the end. count limited to 0x400 to prevent overflow
        for(c = getchar(); c != '\n' && count < 0x400; c = getchar()){
            command[count]=c;
            count++;
        }

        //break on exit
        if(strstr(command, "!exit")) return;

        //parse the information and convert it into an integer
        num1 = parse_int64_from_command(command_current_location);
        operation = command_current_location[0];
        command_current_location = command_current_location + 2; //increment the ptr
        num2 = parse_int64_from_command(command_current_location);

        //logic to decide which calculation to preform based on the input
        switch (operation) {
            case '+':
                print_longlong_res(num1 + num2);
                break;
            case '-':
                print_longlong_res(num1 - num2);
                break;
            case '*':
                print_longlong_res(num1 * num2);
                break;
            case '/':
                if(!num2) print_error("divide by 0");
                print_float_res((double)num1 / num2);
                break;
            case '^':
                print_float_res(pow((double)num1,(double)num2));
                break;
            default:
                print_error("invalid operation");
        }
    }
}

int main(int argc, char* argv[]){
    command = malloc(0x400);
    
    program_loop();
done:
    if(command) free(command);
    return status;
}
