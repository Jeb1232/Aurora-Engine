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

	temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

	job_system = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, static_cast<int>(std::thread::hardware_concurrency() - 1));


	broad_phase_layer_interface = new BPLayerInterfaceImpl();
	physicsSystem = new JPH::PhysicsSystem();

	physicsSystem->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, *broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

	MyBodyActivationListener* body_activation_listener = new MyBodyActivationListener();
	physicsSystem->SetBodyActivationListener(body_activation_listener);

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyContactListener* contact_listener = new MyContactListener();
	physicsSystem->SetContactListener(contact_listener);

	physicsSystem->OptimizeBroadPhase();


	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	//body_interface = physicsSystem.GetBodyInterface();


	//physicsThread = std::thread(&cPhysicsSystem::StartPhysicsLoop, this);
}


cPhysicsSystem::~cPhysicsSystem() {
	
	for (int i = 0; i < rigidBodys.size(); i++) {
		if (rigidBodys[i].initialized) {
			physicsSystem->GetBodyInterface().RemoveBody(rigidBodys[i].rBody->GetID());
			physicsSystem->GetBodyInterface().DestroyBody(rigidBodys[i].rBody->GetID());
			rigidBodys[i].initialized = false;
		}
		if (rigidBodys[i].rBody != nullptr) {
			//free(rigidBodys[i].rBody);
		}
	}
	
	JPH::UnregisterTypes();

	delete JPH::Factory::sInstance;
	delete physicsSystem;
	delete job_system;
	delete temp_allocator;
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

void cPhysicsSystem::PhysicsStep() {

	const int cCollisionSteps = 1;

	for (int i = 0; i < rigidBodys.size(); i++) {
		if (!rigidBodys[i].initialized) {
			rigidBodys[i].rBody = physicsSystem->GetBodyInterface().CreateBody(rigidBodys[i].cSettings);

			JPH::EActivation activation = rigidBodys[i].cSettings.mMotionType != JPH::EMotionType::Static ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
			physicsSystem->GetBodyInterface().AddBody(rigidBodys[i].rBody->GetID(), activation);

			rigidBodys[i].initialized = true;
		}
		rigidBodys[i].position = rigidBodys[i].rBody->GetPosition();
		rigidBodys[i].rotation = rigidBodys[i].rBody->GetRotation().GetEulerAngles();

		rigidBodys[i].lVelocity = rigidBodys[i].rBody->GetLinearVelocity();
		rigidBodys[i].aVelocity = rigidBodys[i].rBody->GetAngularVelocity();

		//std::cout << rigidBodys[i].initialized << std::endl;

		//physicsSystem->GetBodyInterface().GetPositionAndRotation(rigidBodys[i].rBody->GetID(), rigidBodys[i].position, rigidBodys[i].rotation.GetEulerAngles());
		//physicsSystem->GetBodyInterface().GetLinearAndAngularVelocity(rigidBodys[i].rBody->GetID(), rigidBodys[i].lVelocity, rigidBodys[i].aVelocity);
	}

	JPH::BodyID dynamicBody;
	for (int i = 0; i < rigidBodys.size(); i++) {
		if (rigidBodys[i].cSettings.mMotionType == JPH::EMotionType::Dynamic) {
			dynamicBody = rigidBodys[i].rBody->GetID();
		}
	}

	// Step the world
	physicsSystem->Update(pDeltaTime, cCollisionSteps, temp_allocator, job_system);

}

