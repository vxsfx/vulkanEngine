#include <iostream>

template <typename T>
void print(T arg){
    std::cout << arg << std::endl;
}

void a(){
    
}



struct polymorph {
    float c;
    float d;

    void (*method1_call)();

    inline void method1(){
        method1_call(this);
    }
};



void b(polymorph* self){
    print(self->c);
}


int main(){
    polymorph cool = {0.5f, 1.5f};
    cool.method1_call = b;

    cool.method1();


    return 0;
}