#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_err(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

char *positive_convert(long n, int b)
{
    char *bits = malloc(b+1);
    if (bits == NULL)
        exit_err("Cannot allocate mem");
    memset(bits, '0', b);
    bits[b]='\0';

    for (int i = 0; i < b && n >= 0; i++) {
        bits[b-1-i] = n % 2 + '0';
        n /= 2;
    }

    return bits;
}

char xor(char a, char b)
{
    return (!a && b) || (a && !b);
}

char *negative_convert(long n, int b)
{
    char *bits = positive_convert(-n, b);

    for (int i = 0; bits[i]; i++)
        bits[i] = (1 - (bits[i]-'0')) + '0';

    char carry = 1;
    for (int i = strlen(bits)-1; i>=0; i--) {
        char bit = bits[i]-'0';

        bits[i] = xor(bit, carry)+'0';

        carry = bit && carry;
    }

    return bits;
}

int bit_width(long num)
{
    int bits = 0;
    long abs_num = (num < 0) ? -num : num;
    do {
        bits++;
        abs_num /= 2;
    } while (abs_num);
    return bits+1;
}

char *min_value(int b)
{
    char *bits = malloc(b+1);
    if (bits == NULL)
        exit_err("Cannot allocate mem\n");
    memset(bits, '0', b);
    bits[b] = '\0';
    bits[0] = '1';
    return bits;
}

char *max_value(int b)
{
    char *bits = malloc(b+1);
    if (bits == NULL)
        exit_err("Cannot allocate mem\n");
    memset(bits, '1', b);
    bits[b] = '\0';
    bits[0] = '0';
    return bits;
}



int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./second <file name>\n");
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        exit_err("Cannot open the file!\n");
    }

    char buf[200];
    char *binary;

    while (fgets(buf, sizeof buf, f) != NULL) {
        long n;
        int b;
        sscanf(buf, "%ld%d\n", &n, &b);

        if (bit_width(n) > b) {
            if (n < 0)
                binary = min_value(b);
            else
                binary = max_value(b);
        } else {
            if (n >= 0)
                binary = positive_convert(n, b);
            else
                binary = negative_convert(n, b);
        }

        printf("%s\n", binary);
        free(binary);
    }
    fclose(f);

    return 0;
}
