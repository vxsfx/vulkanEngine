#include "Object/Object.h"

#ifndef SCENE
#define SCENE
class Scene {
public:
    std::vector<Object*> objects;

    inline void addObject(Object* obj){
        //temp inline
        objects.push_back(obj);
    };

    inline void removeObject(std::string name){
        for(int o = 0; o < objects.size(); o++){
            if(objects[o]->name == name){
                objects.erase(objects.begin() + o);
                break;
            }
        }
    }

    void changeSkybox();

    inline ~Scene(){
        objects.clear();
    }
};
#endif