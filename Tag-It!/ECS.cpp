#include "ECS.h"
#include "PhysicsBody.h"

entt::registry* ECS::m_reg = nullptr;

void ECS::AttachRegister(entt::registry * reg)
{
	//Attach reference to the registry
	m_reg = reg;
}

void ECS::UnattachRegister()
{
	//Unattaches the register
	m_reg = nullptr;
}

unsigned ECS::CreateEntity()
{
	//create the entity
	auto entity = m_reg->create();

	//Attach Entity Identifier to the entity
	m_reg->assign<EntityIdentifier>(entity);

	//Return entity ID
	return entity;
}

void ECS::DestroyEntity(unsigned entity){
	if (m_reg->has<PhysicsBody>(entity)) {
		m_reg->get<PhysicsBody>(entity).DeleteBody();
	}
	//Destroys the entity
	m_reg->destroy(entity);
}

void ECS::SetUpIdentifier(unsigned entity, unsigned componentID, std::string name)
{
	//Gets reference to the component
	auto& id = GetComponent<EntityIdentifier>(entity);
	//Set the entity
	id.SetEntity(entity);
	//Set the entity identifier unsigned int
	id.AddComponent(componentID);
	//Set the name of the entity
	id.SetName(name);
}

void ECS::SetIsMainCamera(unsigned entity, bool mainCamera)
{
	//Gets reference to the component
	auto& id = GetComponent<EntityIdentifier>(entity);
	//Sets whether this entity is the main camera
	id.SetIsMainCamera(mainCamera);
}

void ECS::SetIsMainPlayer(unsigned entity, bool mainPlayer)
{
	//Gets reference to the component
	auto& id = GetComponent<EntityIdentifier>(entity);
	//Sets whether this entity is the main player
	id.SetIsMainPlayer(mainPlayer);
	id.MainPlayer(entity);
}

void ECS::SetIsSecondPlayer(unsigned entity, bool secondPlayer)
{
	//Gets reference to the component
	auto& id = GetComponent<EntityIdentifier>(entity);
	//Sets whether this entity is the second player
	id.SetIsSecondPlayer(secondPlayer);
	id.SecondPlayer(entity);
}

void ECS::SetIsNotItObjective(unsigned entity, bool notItObj)
{
	//Gets reference to the component
	auto& id = GetComponent<EntityIdentifier>(entity);
	//Sets whether this entity is the objective at the beginning of the game to become not it 
	id.SetIsNotItObjective(notItObj);
	id.NotItObjective(entity);
}