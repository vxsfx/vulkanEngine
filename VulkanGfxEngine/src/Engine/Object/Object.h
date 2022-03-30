#include "Components/Component.h"

#include <vector>

#ifndef OBJECT
#define OBJECT
class Object {
public:
    std::string name;
    Object(std::string giveName);


    //tf not need stored here, tf stored in custom scripts maybe
    //Component* tranform;



    std::vector<Component*> components;  
    
    void update();//update any non gfx physics object(as will be updated seperatly by gfx physx handlers)
    
    
    //ool isUniqueName(Component& component);
    void addComponent(Component* component);

    void removeComponent(std::string name);
 
    inline ~Object() {
        //do nothing
    };
 
    //void findComponent(std::string name);
};
#endif