#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void exit_err(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

char *alloc_bits(int b)
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

int min(int a, int b)
{
    return (a < b) ? a : b;
}

char *hex_to_binary(char *hex_number, int b)
{
    if (strncmp(hex_number, "0x", 2)==0)
        hex_number += 2;

    char *bin_number = alloc_bits(b);

    int rem_bits = b;
    for(int hex_pos=strlen(hex_number)-1; hex_pos>=0 && rem_bits>0; hex_pos--){
        char hex_digit = tolower(hex_number[hex_pos]);
        if (hex_digit >= 'a' && hex_digit <= 'f')
            hex_digit = (hex_digit - 'a' + 10);
        else
            hex_digit -= '0';

        int m = min(4, rem_bits);
        char *hex_to_bin = int_convert(hex_digit, m);

        memcpy(bin_number+rem_bits-m, hex_to_bin, m);
        free(hex_to_bin);

        rem_bits -= m;
    }

    return bin_number;
}

int bits_to_int(char *bits)
{
    int n = 0;
    for (int i = 0; bits[i]; i++)
        n = 2*n + bits[i]-'0';
    return n;
}

double pow2(double n)
{
    if (n >= 0) {
        double p = 1.0;
        for (int i = 0; i < n; i++)
            p *= 2;
        return p;
    } else {
       return 1. / pow2(-n);
    }
}


double ieee_to_decimal(char *ieee_number, int exp_width, int mantis_width)
{
    int sign = (ieee_number[0]=='0') ? 1 : -1;

    char *exp_bits = alloc_bits(exp_width);
    strncpy(exp_bits, ieee_number+1, exp_width);
    int exp = bits_to_int(exp_bits);
    free(exp_bits);

    int not_normalized = (exp==0);
    if (not_normalized) exp=1;
    exp -= (pow2(exp_width-1)-1);

    char *mantis_bits = alloc_bits(mantis_width);
    strncpy(mantis_bits, ieee_number+1+exp_width, mantis_width);
    int shifted = bits_to_int(mantis_bits);
    free(mantis_bits);

    double fract = (not_normalized==0) + (double)shifted  / pow2(mantis_width);

    return sign * pow2(exp) * fract;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./sixth <file>");
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        exit_err("Error opening file");
    }

    int bit_width, exp_width, mantis_width;
    int decimal_width;
    char hex_n[100];

    for (char buf[200]; fgets(buf, sizeof buf, fp) != NULL; ) {
        sscanf(buf, "%d %d %d %s %d\n", &bit_width, &exp_width, &mantis_width, hex_n,
            &decimal_width);

        char *bit_n = hex_to_binary(hex_n, bit_width);

        double decimal_n = ieee_to_decimal(bit_n, exp_width, mantis_width);
        printf("%.*lf\n", decimal_width, decimal_n);

        free(bit_n);
    }
    fclose(fp);
    return 0;
}
