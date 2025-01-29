#include "aPhysicsSystem.h"

cPhysicsSystem::cPhysicsSystem() {

	JPH::Factory::sInstance = new JPH::Factory();

	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	JPH::RegisterTypes();

	physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);
	MyBodyActivationListener body_activation_listener;
	physicsSystem.SetBodyActivationListener(&body_activation_listener);

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyContactListener contact_listener;
	physicsSystem.SetContactListener(&contact_listener);

	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	//body_interface = physicsSystem.GetBodyInterface();


	//physicsThread = std::thread(&cPhysicsSystem::PhysicsLoop, this);
	PhysicsLoop();
}

cPhysicsSystem::~cPhysicsSystem() {
	
	for (int i = 0; i < rigidBodys.size(); i++) {
		if (!rigidBodys[i].initialized) {
			physicsSystem.GetBodyInterface().RemoveBody(rigidBodys[i].rBody->GetID());
			physicsSystem.GetBodyInterface().DestroyBody(rigidBodys[i].rBody->GetID());
			rigidBodys[i].initialized = false;
		}
		if (rigidBodys[i].rBody != nullptr) {
			free(rigidBodys[i].rBody);
		}
	}
	
	JPH::UnregisterTypes();

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void cPhysicsSystem::AddRigidbody(Rigidbody body) {
	rigidBodys.push_back(body);
}

void cPhysicsSystem::PhysicsLoop() {
	bool isSimulating = true;
	while (isSimulating) {
		const int cCollisionSteps = 1;

		for (int i = 0; i < rigidBodys.size(); i++) {
			if (!rigidBodys[i].initialized) {
				physicsSystem.GetBodyInterface().CreateBody(rigidBodys[i].cSettings);
				physicsSystem.GetBodyInterface().AddBody(rigidBodys[i].rBody->GetID(), JPH::EActivation::Activate);
				rigidBodys[i].initialized = true;
			}
			physicsSystem.GetBodyInterface().GetPositionAndRotation(rigidBodys[i].rBody->GetID(), rigidBodys[i].position, rigidBodys[i].rotation);
			physicsSystem.GetBodyInterface().GetLinearAndAngularVelocity(rigidBodys[i].rBody->GetID(), rigidBodys[i].lVelocity, rigidBodys[i].aVelocity);
		}

		// Step the world
		physicsSystem.Update(pDeltaTime, cCollisionSteps, &temp_allocator, &job_system);
	}
}

