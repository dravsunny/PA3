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

char *signed_to_bits(long n, int b)
{
    if (n >= 0)
        return positive_convert(n, b);
    else
        return negative_convert(n, b);
}

char *unsigned_to_bits(long n, int b)
{
    return positive_convert(n, b);
}


long bits_to_unsigned(char *bits)
{
    long n = 0;
    for (int i = 0; bits[i]; i++)
        n = n*2 + bits[i]-'0';
    return n;
}

long power2(int n)
{
    long p;
    for (p = 1; n > 0; n--)
        p *= 2;
    return p;
}

long bits_to_signed(char *bits)
{
    if (bits[0]=='0')
        return bits_to_unsigned(bits);

    char special = 1;
    for (int i = 1; bits[i]; i++) {
        if (bits[i] == '1') {
            special = 0;
            break;
        }
    }
    if (special)
        return (-1)*power2(strlen(bits)-1);

    for (int i = 0; bits[i]; i++)
        bits[i] = (1 - (bits[i]-'0')) + '0';

    char carry = 1;
    for (int i = strlen(bits)-1; i>=0; i--) {
        char bit = bits[i]-'0';

        bits[i] = xor(bit, carry)+'0';

        carry = bit && carry;
    }
    return (-1) * bits_to_unsigned(bits);
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./third <file name>\n");
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        exit_err("Cannot open the file!\n");
    }

    char buf[200];
    char *bits;

    while (fgets(buf, sizeof buf, f) != NULL) {
        long n;
        int b;
        char src, dst;
        sscanf(buf, "%ld%d %c %c\n", &n, &b, &src, &dst);

        if (src=='s' && dst=='u') {
            bits = signed_to_bits(n, b);
            printf("%ld\n", bits_to_unsigned(bits));
            free(bits);
        } else if (src=='u' && dst=='s') {
            bits = unsigned_to_bits(n, b);
            printf("%ld\n", bits_to_signed(bits));
            free(bits);
        }
    }
    fclose(f);

    return 0;
}
