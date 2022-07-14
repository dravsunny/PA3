#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_err(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

char *convert(unsigned  long n, int b)
{
    char *bits = malloc(b+1);    // +1 for '\0'
    if (bits == NULL)
        exit_err("Cannot allocate mem");
    memset(bits, '0', b);    // zero them
    bits[b]='\0';    // end of string

    for (int i = 0; i < b && n >= 0; i++) {
        bits[b-1-i] = n % 2 + '0';
        n /= 2;
    }

    return bits;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./first <file name>\n");
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        exit_err("Cannot open the file!\n");
    }

    char buf[200];
    char *binary;
    while (fgets(buf, sizeof buf, f) != NULL) {
        unsigned long n;
        int b;
        sscanf(buf, "%lu %d\n", &n, &b);

        binary = convert(n, b);
        printf("%s\n", binary);
        free(binary);
    }
    fclose(f);

    return 0;
}
