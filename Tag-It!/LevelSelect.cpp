#include "LevelSelect.h"

LevelSelect::LevelSelect(std::string name) : Scene(name) {
}

void LevelSelect::InitScene(float windowWidth, float windowHeight) {
    //Dynamically allocates the register (so when you unload the scene when you switch between scenes you can later re Init this scene)
    m_sceneReg = new entt::registry;
    //Attach the register
    ECS::AttachRegister(m_sceneReg);

    //Sets up aspect ratio for the camera
    float aspectRatio = windowWidth / windowHeight;

    //Setup main camera entity
    {
        //Creates entity
        auto entity = ECS::CreateEntity();
        EntityIdentifier::MainCamera(entity);

        //Creates new orthographic camera
        ECS::AttachComponent<Camera>(entity);
        //ECS::AttachComponent<Spawn>(entity);

        vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
        ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
        ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);
        ECS::GetComponent<Camera>(entity).Zoom(75.f);
        vec4 size = ECS::GetComponent<Camera>(entity).GetOrthoSize();

        //Sets up the Identifier
        unsigned int bitHolder = EntityIdentifier::CameraBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Main Cam");
        ECS::SetIsMainCamera(entity, true);
    }

    //setup for level 1 (waterfall)
    {
        //Creates entity
        auto entity = ECS::CreateEntity();

        //Adds components
        ECS::AttachComponent<Sprite>(entity);
        ECS::AttachComponent<Transform>(entity);

        //sets up components
        std::string fileName = "screenshot.png";

        //sets up sprite and transform components
        ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
        ECS::GetComponent<Transform>(entity).SetPosition(vec3(-15.f, 0.f, 0.f));

        //Setup indentifier
        unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Level 1");
        ECS::SetIsMainPlayer(entity, true);
    }

    //setup level 2 (tbd)
    {
        //Creates entity
        auto entity = ECS::CreateEntity();

        //Adds components
        ECS::AttachComponent<Sprite>(entity);
        ECS::AttachComponent<Transform>(entity);

        //sets up components
        std::string fileName = "screenshot.png";

        //sets up sprite and transform components
        ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
        ECS::GetComponent<Transform>(entity).SetPosition(vec3(15.f, 0.f, 0.f));

        //Setup indentifier
        unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Level 2");
        ECS::SetIsSecondPlayer(entity, true);
    }

}

void LevelSelect::MouseClick(SDL_MouseButtonEvent evnt) {
    printf("Mouse moved (%f, %f)\n", float(evnt.x), float(evnt.y));
}

void LevelSelect::MouseMotion(SDL_MouseMotionEvent evnt) {
    if ((float(evnt.x) >= 366.f && float(evnt.x) <= 906.f) && (float(evnt.y) >= 377.f && float(evnt.y) <= 701.f)) {
      //  printf("MOVED");
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
        std::string fileName = "level1H.png";
        spr.LoadSprite(fileName, 25, 15);
    }

    else {
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
        std::string fileName = "screenshot.png";
        spr.LoadSprite(fileName, 25, 15);
    }


    if ((float(evnt.x) >= 1014.f && float(evnt.x) <= 1553.f) && (float(evnt.y) >= 377.f && float(evnt.y) <= 701.f)) {
        //  printf("MOVED");
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
        std::string fileName = "level1H.png";
        spr.LoadSprite(fileName, 25, 15);
    }

    else {
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
        std::string fileName = "screenshot.png";
        spr.LoadSprite(fileName, 25, 15);
    }



}
