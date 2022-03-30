#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "util/Debug.hpp"
#include "Input.hpp"

#ifndef WINDOW
#define WINDOW
class Window {
    //store surface and window data
    public:
        inline static GLFWwindow* wnd;
        inline static VkInstance instance;
        inline static VkSurfaceKHR surface;


        static inline void create(int width, int height){
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            wnd = glfwCreateWindow(width, height, "Game", nullptr, nullptr);

            createInstance();


            glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(wnd, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

            glfwSetCursorPosCallback(wnd, Input::mouse_callback);
            glfwSetKeyCallback(wnd, Input::keyPress_callback);
        };


        static inline void createInstance(){
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "VkGame";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "No Engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            



            uint32_t extCount = 0;
            const char** Exts;
            Exts = glfwGetRequiredInstanceExtensions(&extCount);
            createInfo.enabledExtensionCount = extCount;
            createInfo.ppEnabledExtensionNames = Exts;
            createInfo.enabledLayerCount = 0;
            
            //validation
            auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;




            isValidGfx(vkCreateInstance(&createInfo, nullptr, &instance));
            setupDebugMessenger(instance);
            isValidGfx(glfwCreateWindowSurface(instance, wnd, nullptr, &surface));
        };               
        
        
        static inline void destroy(){
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);
            glfwDestroyWindow(wnd);
            glfwTerminate();
        };
};
#endif