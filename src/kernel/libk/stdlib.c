#include <libk/stdlib.h>
#include <libk/ctype.h>

int itoa(char *dest, int num) {
    if (!dest) return -STATUS_INVALID_ARG_ERROR;

    if (num == 0) {
        dest[0] = '0';
        dest[1] = 0;
        return STATUS_OK;
    }

    if (num < 0) {
        dest[0] = '-';
        ++dest;
        num *= -1;
    }

    int tmp = num;
    int digits = 0;
    while (tmp) {
        ++digits;
        tmp /= 10;
    }

    for (int i = digits - 1; i >= 0; --i) {
        dest[i] = DIGIT_NUM_TO_CHAR(num % 10);
        num /= 10;
    }

    dest[digits] = 0;
    return STATUS_OK;
}