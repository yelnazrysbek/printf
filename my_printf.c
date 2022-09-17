#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define INT_MAX 4294967296

int my_printf(char* format, ...);
int parse_args(va_list ap, char* format);

int main() {
    int a = 5;
    int *p = &a;
    int size = my_printf("%%%%%%%%\n");
    my_printf("%d\n", size);
    return 0;
}

int my_printf(char* format, ...) {
    int size = 0;
    
    va_list ap;
    va_start(ap, format);

    if (format != NULL) {
        size+= parse_args(ap, format);
    }

    va_end(ap);

    return size;
}

int put_char(char c) {
    write(1, &c, 1); // first 1 means that stream is terminal
    return 1;
}

int print_str(char* str) {

    if (str == NULL) {
        print_str("(null)");
        return 6;
    }
    int size = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        size += put_char(str[i]);
    }
    return size;
}

int parse_str(va_list ap) {
    char* str = va_arg(ap, char*);
    int size = print_str(str);
    return size;
}

int parse_char(va_list ap) {
    char c = va_arg(ap, int);
    put_char(c);
    return 1;
}

int get_len(long num, int base) {
    int len = 0;
    if (num == 0) {
        return 1;
    }
    while (num > 0) {
        len += 1;
        num = num/base;
    }
    return len;
}

char* convert(long num, int base) {
    int len = get_len(num, base);
    char* str_num = (char*)malloc(len+1);
    str_num[len] = '\0';
    int i = len-1;

    if (num == 0) {
        str_num[i] = '0';
    }

    while (num > 0) {
        int rem = num % base;

        if (rem > 9) {
            rem = (rem-10) + 'a';
        } else {
            rem = rem + '0';
        }
        num = num/base;
        str_num[i] = rem;
        i--;
    }
    return str_num;
}

int print_number(long num, int base) {
    int size = 0;

    if (num < 0) {
        size += put_char('-');
        num = num*(-1);
    }

    char* str_num = convert(num, base);
    size += print_str(str_num);
    free(str_num);
    return size;
}

int parse_number(va_list ap, int base) {
    int num = va_arg(ap, int);
    int size = print_number(num, base);
    return size;
}

int parse_unsigned(va_list ap) {
    int arg = va_arg(ap, int);
    int size = 0;
    if (arg >= 0) {
        size += print_number(arg, 10);
    } else {
        long unsign_num = INT_MAX + arg;
        size += print_number(unsign_num, 10);
    }
    return size;
}

int parse_ptr(va_list ap) {
    int size = 0;
    size += print_str("0x");
    void* arg = va_arg(ap, void*);
    long ptr_number = (long)arg;
    size += print_number(ptr_number, 16);
    return size;
}
int parse_options(va_list ap, char option) {

    int size = 0; 

    switch(option) {
        case 's': size += parse_str(ap); break;
        case 'c': size += parse_char(ap); break;
        case 'd': size += parse_number(ap, 10); break;
        case 'o': size += parse_number(ap, 8); break;
        case 'x': size += parse_number(ap, 16); break;
        case 'i': size += parse_number(ap, 10); break;
        case 'u': size += parse_unsigned(ap); break;
        case 'p': size += parse_ptr(ap); break;
        case '%': size += put_char('%'); break;
    }
}

int parse_args(va_list ap, char* format) {

    int size = 0;

    while(*format != 0) {
        if (*format == '%') {
            format++;
            size += parse_options(ap, *format);
        } else {
            size += put_char(*format);
        }
        format++;
    }


    return size;
        
}