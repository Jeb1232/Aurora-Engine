#include "Rigidbody.h"
#include"aPhysicsSystem.h"

Rigidbody::Rigidbody(cPhysicsSystem* lphysicsSystem) {
	physicsSystem = lphysicsSystem;
	cSettings = JPH::BodyCreationSettings();
	physicsSystem->physicsSystem.GetBodyInterface().CreateBody(cSettings);
	physicsSystem->physicsSystem.GetBodyInterface().AddBody(rBody->GetID(), JPH::EActivation::Activate);
}

Rigidbody::~Rigidbody() {

}