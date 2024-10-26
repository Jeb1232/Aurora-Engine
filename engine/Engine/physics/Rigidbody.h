#pragma once
#include<Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

class cPhysicsSystem;

class Rigidbody {
public:
	Rigidbody(cPhysicsSystem* lphysicsSystem);
	~Rigidbody();
	cPhysicsSystem *physicsSystem  = nullptr;

	JPH::RVec3 position;
	JPH::Quat rotation;
	JPH::Vec3 lVelocity;
	JPH::Vec3 aVelocity;

	JPH::Body* rBody = nullptr;
	JPH::BodyCreationSettings cSettings;
private:
};
