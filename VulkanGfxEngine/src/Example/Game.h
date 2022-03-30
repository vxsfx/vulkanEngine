//#include "../Engine/Engine.h"


#include "../Engine/Scene.hpp"
//#include "../Engine/Object/Components/Camera.h"
#include "../Engine/Object/Components/Model.h"
#include "../Engine/Object/Components/Primitives.h"
#include "../Engine/Object/Components/TransformController.hpp"


class Game{
public:
    Game();
    Scene* currentScene;
    void mainloop();
    ~Game();
};