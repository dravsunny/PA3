#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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

    char *float_number = alloc_bits(n_i + 1 + n_f);
    char *old_number = alloc_bits(n_i + n_f);
    strcpy(old_number,int_part);
    strcat(old_number,fract_part);

    if (int_part[0] == '1') {
        *exp = n_i-1;
        strcpy(float_number, "1.");
        strncat(float_number, old_number+1, n_i-1+n_f);
        free(old_number);
        return float_number;
    }

    char *first_one = strchr(old_number, '1');

    if (first_one == NULL) {
        *exp = 0;
        strcpy(float_number, "0.");
        strncat(float_number, old_number+1, n_f);
        free(old_number);
        return float_number;
    }

    *exp = -(first_one - old_number);
    strcpy(float_number, "1.");
    strncat(float_number, first_one+1, n_f);
    int l = strlen(float_number);
    memset(float_number+l, '0', 2+n_f-l);
    float_number[2+n_f] = '\0';
    free(old_number);

    return float_number;
}



char xor(char a, char b)
{
    return (!a && b) || (a && !b);
}


char *adding_one(char *bits, int f, int *carry)
{
    int b = strlen(bits);
    char *inc_bits = alloc_bits(b);

    inc_bits[0] = bits[0], inc_bits[1] = '.';
    *carry = 1;
    for (int i = f+1; bits[i]!='.'; i--) {
        char bit = bits[i]-'0';
        inc_bits[i] = xor(bit, *carry)+'0';
        *carry = bit && *carry;
    }
    return inc_bits;
}


char *get_lower(char *float_number, int f)
{
    int b = strlen(float_number);
    char *lower_number = alloc_bits(b);
    strcpy(lower_number, float_number);
    for (int i = f+2; lower_number[i]; i++)
        lower_number[i] = '0';
    return lower_number;
}

char *get_upper(char *float_number, int f, int *carry)
{
    char *lower_number = get_lower(float_number, f);
    char *upper_number = adding_one(lower_number, f, carry);
    free(lower_number);
    return upper_number;
}

char *round_convert(char *float_number, int f, int *exp_carry)
{
    char *lower_number = get_lower(float_number, f);

    char *upper_number = get_upper(float_number, f, exp_carry);

    if (float_number[2+f]!='1') {
        free(upper_number);
        *exp_carry = 0;
        return lower_number;
    }

    for (int i = 3+f; float_number[i]; i++) {
        if (float_number[i]=='1') {
            free(lower_number);
            return upper_number;
        }
    }

    if (lower_number[1+f]=='0') {
        *exp_carry = 0;
        free(upper_number);
        return lower_number;
    } else {
        free(lower_number);
        return upper_number;
    }
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

char *ieee_convert(char *round_binary, int exp, char sign_bit, int exp_width, int f)
{
    char *ieee_number = alloc_bits(1 + exp_width + f);

    ieee_number[0] = sign_bit;

    if (strchr(round_binary, '1') == NULL)
        exp = 0;
    else
        exp += pow2(exp_width-1) - 1;

    char *exp_bits = int_convert(exp, exp_width);
    strcpy(ieee_number+1, exp_bits);
    free(exp_bits);

    strncat(ieee_number, round_binary+2, f);

    return ieee_number;
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_err("Usage: ./fifth <file>");
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        exit_err("Error opening file");
    }

    double n;
    char sign_bit;
    int int_decimal;
    double fract_decimal;
    int i, f;
    int exp, exp_width;
    int dummy;

    for (char buf[200]; fgets(buf, sizeof buf, fp) != NULL; ) {
        sscanf(buf, "%lf %d %d %d\n", &n, &dummy, &exp_width, &f);
        if (n < 0) {
            sign_bit = '1';
            n = -n;
        } else {
            sign_bit = '0';
        }

        int_decimal = (int)n;
        i = need_bits(int_decimal);
        char *int_binary = int_convert(int_decimal, i);

        fract_decimal = n - int_decimal;
        char *fract_binary = fract_convert(fract_decimal, f*10);

        char *float_binary = float_convert(int_binary, fract_binary, &exp);

        int exp_carry;
        char *round_binary = round_convert(float_binary, f, &exp_carry);
        exp += exp_carry;
        char *ieee_binary = ieee_convert(round_binary, exp, sign_bit, exp_width, f);

        printf("%s\n", ieee_binary);

        free(int_binary);
        free(fract_binary);
        free(float_binary);
        free(round_binary);
        free(ieee_binary);
    }
    fclose(fp);
    return 0;
}
