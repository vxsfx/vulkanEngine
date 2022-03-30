#include "Game.h"


Game::Game() {
    //Scene scene;
    currentScene = new Scene();

    Graphics::init();

    //use pointers for dynamic objects/class (will not go out of scope when chuck in vector)
    //CREATE POOL AFTER GFX OBJECTS INITED TO SET DESCPOOL, THEN INIT DESCSETS

    Object* cameraObject = new Object("Camera");
    
    Object* rectObject = new Object("rectangle-test-container");
    
    Object* floorObject = new Object("the-floor");

    Transform* floorTf = new Transform("floor-transform", { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
    
    Transform* rectTf = new Transform("rect-transform", { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

    Transform* cameraTransform = new Transform( "camera-transform", {0.0f, 0.1f, 0.0f}, {0.0f, 0.0f, 0.0f});
    Controller* controller = new Controller("cont", cameraTransform);
    cameraObject->addComponent(cameraTransform);
    cameraObject->addComponent(controller);

    //make camera
    Camera* main = new Camera("main-Camera", Graphics::swapExtent.width, Graphics::swapExtent.width, 2);
    Graphics::bindCamera(main->images);
    main->setProjection(
        main->extent.width/main->extent.height, 
        90.0f, 0.01f, 10.0f 
        );
    main->cameraTransform = cameraTransform;
    main->lookAt({ 0.0f, 0.0f, 0.0f });
    main->createImageViews(Graphics::swapImageFormat);
    main->createDepthResource();

    main->createRenderPass();
    main->createFrameBuffers();


    std::vector<Vertex>* vertices = new std::vector<Vertex>(4); 
    *vertices = {
        {{ -5, 0.0f, -5}, {0.0f, 0.0f}},
        {{-5, 0.0f, -5,}, {0.0f, 1.0f}},
        {{5, 0.0f, 5, }, {1.0f, 1.0f} },
        {{5, 0.0f, -5,}, {1.0f, 0.0f}}
    };

    Rectangle* floor = new Rectangle("floor", vertices);

    floor->createTexure("res/Textures/checkers.jpg");
    floor->createBuffers(main->images.size());

    VkDescriptorImageInfo imageInfo2{};

    imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo2.imageView = *floor->textureView;
    imageInfo2.sampler = *floor->textureSampler;


    std::vector<VkDescriptorImageInfo> imageInfos2 = { imageInfo2, imageInfo2 };

    floor->createDescriptorSets(2, imageInfos2);
    floor->createPipeline(main->renderPass, main->extent);//need to pass pipelineIndex for multi-Cameras

    std::vector<Vertex>* verts2 = new std::vector<Vertex>(4);

    *verts2 = {
        {{ -1.f, -1.f, 0}, {0.0f, 0.0f}},
        {{-1.f, 1.0f, 0,}, {0.0f, 1.0f}},
        {{1, 1.0f, 0, }, {1.0f, 1.0f} },
        {{1, -1.0f, 0,}, {1.0f, 0.0f}}
    };


    Rectangle* rect = new Rectangle("rect-component", verts2);
    
    
    //for camera texture
    //std::vector<VkDescriptorImageInfo> imageInfos = main->getDescriptorImageInfos();
    
    //rect->createTexure("res/Textures/modelTest.png");
    
    
    
    //Image::transitionLayout(main->images[0], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);




    //rect->textureImage = &main->images[0];
    rect->textureView = &main->imageViews[0];// main->renderView;// &main->imageViews[0];
    


    rect->textureSampler = new VkSampler();

    Image::createSampler(rect->textureSampler);


    rect->createBuffers(main->images.size());

    
    //model texture
    
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = main->imageViews[0];
    imageInfo.sampler = *rect->textureSampler;


    VkDescriptorImageInfo imageInfo3{};
    imageInfo3.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo3.imageView = main->imageViews[1];
    imageInfo3.sampler = *rect->textureSampler;//*sampler2;// *rect->textureSampler;

    
    std::vector<VkDescriptorImageInfo> imageInfos = { imageInfo3, imageInfo };
    
    
    
    
    //createdescsetlayouts
    rect->createDescriptorSets(2, imageInfos);
    rect->createPipeline(main->renderPass, main->extent);
    
    
    
    
    main->createCommandBuffers();

    rect->addTransform(rectTf);
    floor->addTransform(floorTf);

    //cameraObject.components.push_back(&main);
    cameraObject->addComponent(main);

    rectObject->addComponent(rect);
    rectObject->addComponent(rectTf);

    floorObject->addComponent(floor);
    floorObject->addComponent(floorTf);
    


    //scene.objects.push_back(&cameraObject);
    currentScene->addObject(cameraObject);
    currentScene->addObject(rectObject);

    currentScene->addObject(floorObject);

    Camera::main = main;

    //currentScene = &scene;


}

void Game::mainloop(){
    while (!glfwWindowShouldClose(Window::wnd)) {
        //call updat on all objects
        //Camera::main->update();
        
        //test
        

        //BaseObjectGfx::allGfx[0]->update();
        for (auto obj : currentScene->objects) {
            obj->update();
        }
        
        
        Camera::draw();
        glfwPollEvents();
    }

    vkDeviceWaitIdle(Graphics::logiDevice);
}


Game::~Game(){
    Graphics::destroy();
    delete currentScene;
};