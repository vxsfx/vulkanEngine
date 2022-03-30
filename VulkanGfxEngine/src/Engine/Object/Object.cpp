#include "Object.h"
Object::Object(std::string giveName) {
    name = giveName;
}

void Object::addComponent(Component* component) {
    components.push_back(component);
};

void Object::update() {
    for (auto comp : components) {
        comp->update();
    }
};

void Object::removeComponent(std::string name) {
    for (int c = 0; c < components.size(); c++) {
        if (components[c]->name == name) {
            components.erase(components.begin() + c);
            break;
        }
    }
}