/**
 * Brainfuck interpreter that conforms to the language specification
 * seen here: https://github.com/brain-lang/brainfuck/blob/master/brainfuck.md
 * 
 * Reads code from a file or stdin.
 * 
 * USAGE: ./bf <infile>
 * BY: Oliver Larsson 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Input buffer size.
#define BUF_SIZE 128
// Memory strip starting size.
#define MEM_SIZE 64

void parse(const char *code, size_t codelen);
const char *findClosing(const char *opening);
const char *findOpening(const char *code, const char *closing);

/**
 * Main program entrypoint.
 */
int main(int argc, char **argv) {

    // Open file if specified.
    if (argc > 1 && !freopen(argv[1], "r", stdin)) {
        perror(argv[1]);
        exit(EXIT_FAILURE);        
    }

    // Read all the code into a string for parsing.
    char *code = NULL;
    size_t codelen = 0;

    // Input buffer.
    char buffer[BUF_SIZE];
    size_t read;

    // Put all input into the input string.
    while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
        code = realloc(code, codelen += read);
        strncpy(code + codelen - read, buffer, read);        
    }

    // Parse the code.
    parse(code, codelen);
    free(code);

    return 0;
}

/**
 * Parse a brainfuck program as a string.
 * 
 * @arg code - the code string.
 * @arg codelen - length of code.
 */
void parse(const char *code, size_t codelen) {
    // Current memory buffer.
    char *mem = calloc(1, MEM_SIZE);
    size_t memsize = MEM_SIZE;
    
    // Current head.
    unsigned long offset = memsize / 2;

    // Parse the code one char at a time.
    for (const char *c = code; c < code + codelen; c++) {
        if (*c == '+') {
            mem[offset]++;
        } else if (*c == '-') {
            mem[offset]--;
        } else if (*c == '>' && ++offset == memsize) {
            // We need to allocate more memory.
            mem = realloc(mem, memsize += MEM_SIZE);
            memset(mem + offset, 0, MEM_SIZE);            
        } else if (*c == '<' && offset-- == 0) {
            // We need to allocate more memory.
            mem = realloc(mem, memsize + MEM_SIZE);
            memcpy(mem + MEM_SIZE, mem, memsize);
            memset(mem, 0, MEM_SIZE);
            memsize += MEM_SIZE;
            offset += MEM_SIZE;
        } else if (*c == '.') {
            printf("%c", mem[offset]);
        } else if (*c == ',') {
            mem[offset] = getchar();
        } else if (*c == '[' && mem[offset] == 0) {
            c = findClosing(c);
        } else if (*c == ']' && mem[offset] != 0) {
            c = findOpening(code, c);
        }
    }

    free(mem);
    fflush(stdout);
}

/**
 * Find a corresponding closing ] to an opening [
 */
const char *findClosing(const char* opening) {
    const char *ptr = opening;
    unsigned int opened = 1;

    while (*(++ptr) != '\0') {
        if (*ptr == '[') {
            opened++;
        } else if (*ptr == ']') {
            if (opened > 1) 
                opened--;
            else 
                return ptr;
        }
    }

    fprintf(stderr, "syntax error: %s\n", "no closing ] found");
    exit(EXIT_FAILURE);
}

/**
 * Find a corresponding closing [ to an opening ]
 */
const char *findOpening(const char *code, const char* closing) {
    const char *ptr = closing;
    unsigned int closed = 1;

    while (--ptr >= code) {
        if (*ptr == ']') {
            closed++;
        } else if (*ptr == '[') {
            if (closed > 1) 
                closed--;
            else 
                return ptr;
        }
    }

    fprintf(stderr, "syntax error: %s\n", "no opening [ found");
    exit(EXIT_FAILURE);
}
