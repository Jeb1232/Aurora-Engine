#include "aPhysicsSystem.h"

cPhysicsSystem::cPhysicsSystem() {

	JPH::RegisterDefaultAllocator();

	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

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

Rigidbody cPhysicsSystem::GetRigidbody(Rigidbody body) {
	for (int i = 0; i < rigidBodys.size(); i++) {
		if (rigidBodys[i].cSettings.mMotionType == JPH::EMotionType::Dynamic) {
			return rigidBodys[i];
		}
	}
}

void cPhysicsSystem::PhysicsLoop() {

	JPH::TempAllocatorImpl temp_allocator = JPH::TempAllocatorImpl(10 * 1024 * 1024);

	JPH::JobSystemThreadPool job_system = JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	
	JPH::BodyID dynamicBody;
	for (int i = 0; i < rigidBodys.size(); i++) {
		if (rigidBodys[i].cSettings.mMotionType == JPH::EMotionType::Dynamic) {
			dynamicBody = rigidBodys[i].rBody->GetID();
		}
	}
	while (isSimulating) {
		const int cCollisionSteps = 1;

		for (int i = 0; i < rigidBodys.size(); i++) {
			if (!rigidBodys[i].initialized) {
				physicsSystem.GetBodyInterface().CreateBody(rigidBodys[i].cSettings);
				if (rigidBodys[i].cSettings.mMotionType == JPH::EMotionType::Dynamic) {
					physicsSystem.GetBodyInterface().AddBody(rigidBodys[i].rBody->GetID(), JPH::EActivation::Activate);
				}
				else if (rigidBodys[i].cSettings.mMotionType == JPH::EMotionType::Static) {
					physicsSystem.GetBodyInterface().AddBody(rigidBodys[i].rBody->GetID(), JPH::EActivation::DontActivate);
				}
				rigidBodys[i].initialized = true;
			}
			physicsSystem.GetBodyInterface().GetPositionAndRotation(rigidBodys[i].rBody->GetID(), rigidBodys[i].position, rigidBodys[i].rotation);
			physicsSystem.GetBodyInterface().GetLinearAndAngularVelocity(rigidBodys[i].rBody->GetID(), rigidBodys[i].lVelocity, rigidBodys[i].aVelocity);
		}

		// Step the world
		physicsSystem.Update(pDeltaTime, cCollisionSteps, &temp_allocator, &job_system);
	}
}

