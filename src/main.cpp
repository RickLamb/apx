// 
#include <iostream>
#include "component.h"

#include <assert.h>
#include <chrono>
#include <thread>

#include <unordered_map>

// jobsystem settings
#define JOBSYSTEM_ENABLE_PROFILING                ///< Enables worker/job profiling, and an ascii profile dump on shutdown.
#define JOBSYSTEM_ASSERT(...) assert(__VA_ARGS__) ///< Directs internal system asserts to app-specific assert mechanism.

// jobsystem include
#include "jobsystem.h"

auto hello() -> int;
auto registerMyComponents(ComponentManager* manager) -> void;

typedef std::unordered_map<std::string, jobsystem::JobStatePtr> JobMap;
typedef std::vector<jobsystem::JobStatePtr> JobList;

enum class UpdateStep { PreUpdate, Update, PostUpdate };
const std::string GetUpdateStepName(UpdateStep step) {
	switch (step) {
	case UpdateStep::PreUpdate: return "PreUpdate";
	case UpdateStep::Update: return "Update";
	case UpdateStep::PostUpdate: return "PostUpdate";
	default: return "";
	};
	return "";
}

void runFrameUpdateStep(jobsystem::JobManager& jobManager, EntityManager& entityManager, JobMap& writeMap, JobList& list, const UpdateStep updateStep) {
	ComponentManager* mgr = entityManager.GetComponentMgr();

	for (IComponentSystem* sys : mgr->mSystems) {
		const auto& d = sys->GetComponentFunctions();
		const std::string componentName = sys->Name();
		const uint32_t numComponents = sys->NumComponents();
		if (numComponents == 0) {
			continue;
		}

		for (const ComponentTask& fn : d) {
			std::cout << "Hello " << fn.mName << "\n";

			if (fn.mName == GetUpdateStepName(updateStep)) {

				jobsystem::JobDelegate jobFunc = [entityManager, mgr, sys, updateStep] () {
					int i=0; ++i;
					const std::string n = sys->Name();
					if (updateStep == UpdateStep::Update) {
						std::cout << "Running " << n << " " << GetUpdateStepName(updateStep) << "\n";
						const ECS_Context& ctx = entityManager.GetContext();
						sys->FrameUpdate(ctx, mgr);
					}
				};

				jobsystem::JobStatePtr newJob = jobManager.AddJob(jobFunc, componentName[0]);
				list.push_back(newJob);

				for (const ComponentTask::Dependency& dn : fn.mDepends) {
					std::cout << "Dependency " << dn.mComponentName << "\n";
					auto lastWrite = writeMap.find(dn.mComponentName);
					//bool thisComponent = dn.mComponentName == componentName; dn.mComponentName == '_' when using this
					bool bDependencyOnLastWrite = false;
					bool bNewWriteDependency = false;
					jobsystem::JobStatePtr dependency = nullptr;

					if (dn.mType == ComponentTask::Dependency::Type::This) {
						bDependencyOnLastWrite = true;
						lastWrite = writeMap.find(componentName);
						if (dn.mDir != ComponentTask::Dependency::Direction::in) {
							// Direction::out || Direction::inout
							bNewWriteDependency = true;
						}
					}
					else if (dn.mType == ComponentTask::Dependency::Type::MyComponent) {
						bDependencyOnLastWrite = true;
						if (dn.mDir != ComponentTask::Dependency::Direction::in) {
							// Direction::out || Direction::inout
							bNewWriteDependency = true;
						}
					}
					else if (dn.mType == ComponentTask::Dependency::Type::AllComponents) {
						bDependencyOnLastWrite = true;
						if (dn.mDir != ComponentTask::Dependency::Direction::in) {
							// Direction::out || Direction::inout
							bNewWriteDependency = true;
						}
					}

					if (bDependencyOnLastWrite) {
						if (lastWrite != writeMap.end()) {
							dependency = lastWrite->second;
						}
					}

					if (dependency != nullptr) {
						dependency->AddDependant(newJob);
					}

					if (bNewWriteDependency) {
						writeMap[dn.mComponentName] = newJob;

						//enum class Direction { unknown, in, out, inout };
						//enum class Type { Unknown, This, Ctx, MyComponent, AllComponents };

						//Direction		mDir;
						//Type			mType;
						//std::string		mComponentName;
						//std::string		mName;
					}
				}

				//todo: can't launch jobs that may finish before dependency added
				//newJob->SetReady();
			}
		}
	}
}

void runFrameUpdate(jobsystem::JobManager& jobManager, EntityManager& entityManager) {
	JobMap writeMap;
	JobList jobList;

	runFrameUpdateStep(jobManager, entityManager, writeMap, jobList, UpdateStep::PreUpdate);
	runFrameUpdateStep(jobManager, entityManager, writeMap, jobList, UpdateStep::Update);
	runFrameUpdateStep(jobManager, entityManager, writeMap, jobList, UpdateStep::PostUpdate);

	for(auto& t : jobList) {
		t->SetReady();
	}

	jobManager.AssistUntilDone();
	//Wait till all finished
	//for (auto& t : jobList) {
	//	t->Wait();
	//}	
}

int main()
{
	// setup workers
	jobsystem::JobManagerDescriptor jobManagerDesc;

	const size_t kWorkerCount = 8;
	for (size_t i = 0; i < kWorkerCount; ++i)
	{
		jobsystem::JobWorkerDescriptor jd("Worker");
		jd.m_enableWorkStealing = 1;
		jobManagerDesc.m_workers.push_back(jd);
	}

	std::unique_ptr<jobsystem::JobManager> jobManager = std::make_unique<jobsystem::JobManager>();
	if (!jobManager->Create(jobManagerDesc))
	{
		std::cout << "Failed jobsystem::JobManager Create()\n";
		return -1;
	}

	//jobsystem::JobChainBuilder<10000> builder(jobManager);

	// setup entity system
	std::unique_ptr < EntityManager > entityManager = std::make_unique<EntityManager>();
    ComponentManager* mgr = entityManager->GetComponentMgr();
    registerMyComponents(mgr);

	hello();

	IComponentSystem* pTransformSystem = mgr->GetSystemByName("Transform");
	IComponentSystem* pPlayerDataSystem = mgr->GetSystemByName("PlayerData");
	IComponentSystem* pMoveForwardSystem = mgr->GetSystemByName("MoveForward");
	IComponentSystem* pFacePlayerSystem = mgr->GetSystemByName("FacePlayer");
	//IComponentSystem* pNameSystem = mgr->GetSystemByName("Name");
	//IComponentSystem* pMyComponentSystem = mgr->GetSystemByName("MyComponent");

#if 1
	std::vector<EntityId> players;
	const int numPlayers = 100;
	for(int iPlayer=0; iPlayer< numPlayers; ++iPlayer) {
		// Create player entity
		EntityId player = entityManager->AllocEntity();
		pTransformSystem->Alloc(player);
		pPlayerDataSystem->Alloc(player);
		players.push_back(player);

		// Create 100 tanks entities per player that move towards player
		const int numTanks = 100;
		for (int i = 0; i < numTanks; ++i) {
			EntityId tank = entityManager->AllocEntity();
			pTransformSystem->Alloc(tank);
			pMoveForwardSystem->Alloc(tank);
			pFacePlayerSystem->Alloc(tank);
		}
	}
#endif

	//const auto& d = pFacePlayerSystem->GetComponentFunctions();
	hello();
	runFrameUpdate(*jobManager, *entityManager);

	//// Run entity system for 100 frames
	//for (int i = 0; i < 100; ++i) {
	//	entityManager->FrameUpdate();

	//	item->job = mgr.AddJob(delegate, debugChar);

	//	m_allJobs.push_back(item->job);

	//	if (m_dependency)
	//	{
	//		m_dependency->job->AddDependant(item->job);
	//		m_dependency = nullptr;
	//	}


	//	builder.Do(something, 'a')
	//		.Then()
	//		.Do(somethingAfterThat, 'b')
	//		.Then()
	//		.Together()
	//		  .Do(parallelThing1, 'c')
	//		  .Do(parallelThing2, 'd')
	//		  .Do(parallelThing3, 'e')
	//		.Close()
	//		.Then()
	//		.Do(finalThing, 'F')

	//	// Run the jobs and assist until complete.
	//	builder
	//		.Go()
	//		.AssistAndWaitForAll();
	//}


    hello();
    std::cout << "Hello World" << std::endl;
    return 0;
}

