#include <stdio.h>
struct polymorph {
    int a;
    int b;

    void* (*method)();
} poly;

int* c(struct polymorph* self){
    printf("aaa %d",(*self).a);
    return &(*self).b;
}


int main(){
    struct polymorph p;
    p.method = c;
    p.b = 30;

    p.a = 50;

    void* d = p.method(&p);

    printf("%d", *((int*)d));
    return p.a;
}