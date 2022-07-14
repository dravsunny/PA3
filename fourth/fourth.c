#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void exit_err(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

static char *alloc_bits(int b)
{
    char *bits = malloc(b+1);
    if (bits == NULL)
        exit_err("convert: cannot allocate memory");
    memset(bits, '0', b);
    bits[b] = '\0';

    return bits;
 }



char *int_convert(int n, int b)
{
    char *bits = alloc_bits(b);

    for (int i = 0; i < b && n >= 0; i++) {
        bits[b-1-i] = n%2 + '0';
        n /= 2;
    }

    return bits;
}


char *fract_convert(double n, int b)
{
    char *bits = alloc_bits(b);

    for (int i = 0; i < b; i++) {
        n *= 2;
        bits[i] = (int)n + '0';
        n = n - (int)n;
    }
    return bits;
}

int need_bits(int n)
{
    int b = 0;
    do {
        b++;
        n/=2;
    } while (n >0);
    return b;
}

char *float_convert(char *int_part, char *fract_part, int *exp)
{
    int n_i = strlen(int_part);
    int n_f = strlen(fract_part);

    char *float_number = alloc_bits(1 + 1 + n_f);
    char *old_number = alloc_bits(n_i + n_f);
    strcpy(old_number,int_part);
    strcat(old_number,fract_part);

    if (int_part[0] == '1') {
        *exp = n_i-1;
        strcpy(float_number, n_f > 0 ? "1." : "1");
        strncat(float_number, old_number+1, n_f);
        free(old_number);
        return float_number;
    }

    char *first_one = strchr(old_number, '1');

    if (first_one == NULL) {
        *exp = 0;
        strcpy(float_number, n_f > 0 ? "0." : "0");
        strncat(float_number, old_number+1, n_f);
        free(old_number);
        return float_number;
    }

    *exp = -(first_one - old_number);
    strcpy(float_number, n_f > 0 ? "1." : "1");
    strncat(float_number, first_one+1, n_f);
    int l = strlen(float_number);
    if (l != 2+n_f) {
        memset(float_number+l, '0', 2+n_f-l);
        float_number[2+n_f] = '\0';
    }
    free(old_number);
    return float_number;
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./fourth <file>");
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        exit_err("Error opening file");
    }

    double n;
    int int_decimal;
    double fract_decimal;
    int i, f;
    int exp;

    for (char buf[200]; fgets(buf, sizeof buf, fp) != NULL; ) {
        sscanf(buf, "%lf %d\n", &n, &f);

        int_decimal = (int)n;
        i = need_bits(int_decimal);
        char *int_binary = int_convert(int_decimal, i);

        fract_decimal = n - int_decimal;
        char *fract_binary = fract_convert(fract_decimal, f);

        char *float_binary = float_convert(int_binary, fract_binary, &exp);
        printf("%s %d\n", float_binary, exp);

        free(int_binary);
        free(fract_binary);
        free(float_binary);
    }
    fclose(fp);
    return 0;
}
