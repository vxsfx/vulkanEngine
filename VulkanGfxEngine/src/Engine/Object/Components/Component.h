//#include <string>
//#include <vector>
#include "../../util/include.h"

#ifndef COMPONENT
#define COMPONENT
class Component {
    public:
        
    enum class COMPONENT_TYPE {
        DEFAULT,
        GRAPHICS,
        PHYSICS,
        CUSTOM,
        CAMERA
    };



        std::string name;
        const COMPONENT_TYPE type = COMPONENT_TYPE::DEFAULT;

        Component(std::string giveName);

    //methods should use statics, if args required, call separate method

        //virtual void init();
        virtual void update();
        //virtual void end();
        inline ~Component() {
            //do nothin
        }
};
#endif