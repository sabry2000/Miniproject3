#include <stdio.h>
#include <stdlib.h>

unsigned int n; 
unsigned int bitMask;
char *endptr;
char str[10];
unsigned int number;

// Create a mask by shifting 1 over a bunch and subtracting 1 to get a number of 1s equal to the offset
void create_mask() {bitMask = (1<<(n))-1;}
int bit_mask(int memAddr) {return memAddr & bitMask;}
int get_page(int memAddr){return memAddr>>n;}
// Calls get_page and bit_mask then prints it all
void print_address(unsigned int virtAddr) {
    printf("virtual address %d is in page number %d and offset %d\n", virtAddr, get_page(virtAddr), bit_mask(virtAddr));
}

int main() {
    FILE *input;
    input = fopen("input.txt","r");
    if(input) {
        // Get n
        if(fgets(str, 10, input) != NULL) {
            n = strtoul(str, &endptr, 10);
            // Create the mask for out particular file
            create_mask();
        }
        // Ignore m as it is easier to get it out of n
        fgets(str, 10, input);
        // Read all addresses up to the end of the file
        while(fgets(str, 10, input) != NULL) {
            number = strtoul(str, &endptr, 10);
            // Does the shifting, masking, and printing
            print_address(number);
        }
        fclose(input);
    }
    return 0;
}
