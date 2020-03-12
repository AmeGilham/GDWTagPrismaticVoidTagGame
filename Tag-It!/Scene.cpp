#include "Scene.h"

Scene::Scene(std::string name)
{
	m_physicsWorld = new b2World(m_gravity);
	m_name = name;
}

void Scene::Unload()
{
	if (m_sceneReg != nullptr)
	{
		delete m_sceneReg;
		m_sceneReg = nullptr;
	}
}

void Scene::SaveScene()
{
	//Our scene
	nlohmann::json scene;

	//Casts to scene so all inherited classes can be saved the same way
	//The only data that matters for saving is the actual registry
	//which exists in the base class, meaning data won't be lost
	scene["Scene"] = *this;

	//Create the file and output the scene's contents to it
	File::CreateJSON(m_name + ".json", scene);
}

entt::registry* Scene::GetScene() const
{
	return m_sceneReg;
}

void Scene::SetScene(entt::registry& scene){
	m_sceneReg = &scene;
}

std::string Scene::GetName() const
{
	return m_name;
}

void Scene::SetName(std::string name)
{
	m_name = name;
}

b2Vec2 Scene::GetGravity() const
{
	return m_gravity;
}

void Scene::SetGravity(b2Vec2 grav)
{
	m_gravity = grav;
}

b2World & Scene::GetPhysicsWorld()
{
	// TODO: insert return statement here
	return *m_physicsWorld;
}

myListener * Scene::GetListener()
{
	// TODO: insert return statement here
	return &listener;
}

void Scene::SetWindowSize(float windowWidth, float windowHeight)
{
	auto& tempCam = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera());
	
	tempCam.SetWindowSize(vec2(windowWidth, windowHeight));
	tempCam.Orthographic(float(windowWidth / windowHeight), tempCam.GetOrthoSize().x, tempCam.GetOrthoSize().y,
															tempCam.GetOrthoSize().z, tempCam.GetOrthoSize().w,
															tempCam.GetNear(), tempCam.GetFar());
}
