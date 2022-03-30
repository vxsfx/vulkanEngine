//#include "../Engine/Engine.h"

//#include "../Engine/Window.hpp"
#include "Game.h"
#include <iostream>

int main(){
    int result = 0;

    if (!validationSupport()) {
        return result;
    }

    try {

        Window::create(800, 800);


        Game game;
        
        game.mainloop();
        
        result = 1;

    }
    catch(std::runtime_error& e){
        std::cout << e.what();
        result = -1;
    }
    catch(...){
        std::cout << "an error";
        result = -1;
    }
    
    Window::destroy();


    return result;
}