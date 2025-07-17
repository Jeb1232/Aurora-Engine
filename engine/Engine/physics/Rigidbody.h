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
class Rigidbody {
public:
	Rigidbody(JPH::BodyCreationSettings settings);
	~Rigidbody();
	//cPhysicsSystem *physicsSystem  = nullptr;
	//JPH::BodyCreationSettings settings
	JPH::Vec3 position;
	JPH::Vec3 rotation;
	JPH::Vec3 lVelocity;
	JPH::Vec3 aVelocity;

	bool initialized = false;

	JPH::Body* rBody = nullptr;
	JPH::BodyCreationSettings cSettings;
private:
};
