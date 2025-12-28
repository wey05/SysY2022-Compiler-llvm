#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

int _sysy_last_char = 10; 

void _sysy_update_last(int c) {
    _sysy_last_char = c;
}

int __sysy_main();

int main() {
    int ret = __sysy_main();
    unsigned char exit_code = (unsigned char)ret;
    if (_sysy_last_char != 10) {
        printf("\n");
    }
    printf("%d\n", exit_code);
    return exit_code;
}

int getint() { int t; scanf("%d", &t); return t; }
int getch() { char c; scanf("%c", &c); return (int)c; }
float getfloat() { float t; scanf("%a", &t); return t; }  

void putint(int a) { 
    printf("%d", a); 
    _sysy_update_last('0'); 
}

void putch(int a) { 
    printf("%c", a); 
    _sysy_update_last(a); 
}

void putfloat(float a) { 
    printf("%a", a);  
    _sysy_update_last('0'); 
}

int getarray(int a[]) {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);
    return n;
}

int getfarray(float a[]) {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%a", &a[i]);  
    return n;
}

void putarray(int n, int a[]) {
    printf("%d:", n);
    for (int i = 0; i < n; i++) printf(" %d", a[i]);
    printf("\n");
    _sysy_update_last(10);
}

void putfarray(int n, float a[]) {
    printf("%d:", n);
    for (int i = 0; i < n; i++) printf(" %a", a[i]);  
    printf("\n");
    _sysy_update_last(10);
}

struct timeval _sysy_start, _sysy_end;
void starttime() { gettimeofday(&_sysy_start, NULL); }
void stoptime() {
    gettimeofday(&_sysy_end, NULL);
    double duration = (_sysy_end.tv_sec - _sysy_start.tv_sec) + 
                      (_sysy_end.tv_usec - _sysy_start.tv_usec) / 1000000.0;
    printf("Timer: %fs\n", duration);
    _sysy_update_last(10);
}
