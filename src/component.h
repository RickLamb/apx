#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

template< typename T >
struct TypeName;

template< typename T >
struct TypeName
{
	static const char* Get(void) {
		return nullptr;
	}
};

class EntityId
{
public:
	EntityId()
	: mIndex(UINT32_MAX)
	, mCount(0)
	{
	}

	EntityId(uint32_t index, uint32_t count)
	: mIndex(index)
	, mCount(count)
	{
	}

	EntityId(const EntityId& c)
		: mIndex(c.mIndex)
		, mCount(c.mCount)
	{
	}

	EntityId(uint64_t i)
	{
		uint64_t mask = UINT32_MAX;
		mask = ~(mask << 32);

		mIndex = uint32_t(i & mask);
		mCount = (i >> 32);
	}

	uint64_t	ToInt() const
	{
		uint64_t count = mCount;
		uint64_t index = mIndex;
		return (count << 32) | index;
	}

	uint32_t	mIndex;
	uint32_t   	mCount;
};


class ECS_Math {
public:
	float lookAt(float ax, float ay, float bx, float by) const {
		//placeholder
		return 0.0f;
	};

	float forward(float r) const {
		//placeholder
		return 0.0f;
	}
};

class ECS_Context {
public:
	ECS_Context()
	: thisEntityId(EntityId())
	, deltaTime(1.0f / 30.0f)
	, isGameOver(false)
	, taskManager(nullptr)
	{
	}

	EntityId	thisEntityId;
	float		deltaTime;
	bool		isGameOver;
	ECS_Math	math;				// guaranteed pure / thread-safe math library
	void*		taskManager;		// guaranteed thread-safe task manager TODO:
};


struct ComponentTask
{
	struct Dependency
	{
		enum class Direction { unknown, in, out, inout };
		enum class Type { Unknown, This, Ctx, MyComponent, AllComponents };
	
		Dependency(Direction dir, Type type, const std::string& component, const std::string& name)
		: mDir(dir)
		, mType(type)
		, mComponentName(component)
		, mName(name)
		{
		}

		Direction		mDir;
		Type			mType;
		std::string		mComponentName;
		std::string		mName;
	};

	ComponentTask(const std::string& name, std::vector<Dependency> i)
	: mName(name)
	{
	}

	std::string						mName;
	std::vector<Dependency>			mDepends;
};

//class Component {
//private: std::string m_id;
//public: Component() = default;
//public: Component(Component const&) = delete; /* No 'that' constructor, suppress copy */
//public: auto operator=(Component const&) -> void = delete;
//
//};
class ComponentManager;

class IComponentSystem
{
public:
	virtual const std::string Name() = 0;

	virtual void Alloc(EntityId id) = 0;

	virtual uint32_t NumComponents() const = 0;

	virtual void FrameUpdate(const ECS_Context& ctx, ComponentManager* cm) = 0;

	virtual std::vector<ComponentTask> GetComponentFunctions() = 0;
};



template< class T  >
class ComponentSystem : public IComponentSystem
{
public:
	ComponentSystem(const std::string& name)
	: mName(name)
    {
    }

	virtual const std::string Name() {
		return mName;
		//return TypeName<T>::Get();
	}

	virtual void Alloc(EntityId id) {
		mEntityMap[id.ToInt()] = mData.size();
		mReverseEntityMap[mData.size()] = id.ToInt();
		mData.emplace_back(T());
	}

	const T* Get(EntityId id) const {
		uint64_t i = id.ToInt();
		auto it = mEntityMap.find(i);
		if (it != mEntityMap.end())
		{
			uint32_t componentIndex = it->second;
			return &(mData[componentIndex]);
		}
		return nullptr;
	}

	T* Get(EntityId id) {
		uint64_t i = id.ToInt();
		auto it = mEntityMap.find(i);
		if (it != mEntityMap.end())
		{
			uint32_t componentIndex = it->second;
			return &(mData[componentIndex]);
		}
		return nullptr;
	}

	virtual uint32_t	NumComponents() const {
		return mEntityMap.size();
	}

	virtual void FrameUpdate(const ECS_Context& ctx, ComponentManager* cm) {
		ECS_Context thisCtx = ctx;
		const int numComponents = mData.size();
		for(int iComponent =0; iComponent <numComponents; ++iComponent) {
			uint64_t id = mReverseEntityMap[iComponent];
			thisCtx.thisEntityId = EntityId(id);
			mData[iComponent].CallUpdate(thisCtx, *cm);
		}
	}

	virtual std::vector<ComponentTask> GetComponentFunctions() {
		return T::ComponentFunctions();
	}

private:
	std::string										mName;
	std::vector<T>									mData;
	std::unordered_map< uint64_t, uint32_t >		mEntityMap;
	std::unordered_map< uint32_t, uint64_t >		mReverseEntityMap;
};

class ComponentManager
{
public:
	IComponentSystem* GetSystemByName(const std::string findName) {
		for (auto sys : mSystems) {
			if (sys->Name() == findName) {
				return sys;
			}
		}
		return nullptr;
	}

	template< class T  >
	ComponentSystem<T>*		GetSystem(const std::string findName) {
		//const char* findName = TypeName<T>::Get();
		//if (findName == nullptr) {
		//	return nullptr;
		//}

		for(auto sys : mSystems) {
			if (findName == sys->Name()) {
				return static_cast<ComponentSystem<T>*>(sys);
			}
		}
		return nullptr;
	}

	void FrameUpdate(const ECS_Context& ctx) {
		for(auto sys : mSystems) {
			sys->FrameUpdate(ctx, this);
		}
	}

//private:
	std::vector< IComponentSystem* > mSystems;
};

template<class C>
void RegisterComponent(ComponentManager* mgr, const std::string& name) {
	auto deps = C::ComponentFunctions();
	auto* cs = new ComponentSystem<C>(name);
	mgr->mSystems.push_back(cs);
};

class EntityManager
{
public:

	EntityId	AllocEntity()
	{
		// TODO: Free and recycle entity ids
		EntityId id(mEntities.size(), 1);
		mEntities.push_back(id);
		return id;
	}

	void	FrameUpdate() {
		mCtx.deltaTime = 1.0f / 30.0f;
		mComponentManager.FrameUpdate(mCtx);
	}

	ComponentManager*		GetComponentMgr() {
		return &mComponentManager;
	}

	const ECS_Context&		GetContext() const {
		return mCtx;
	}

private:
	ComponentManager		mComponentManager;
	ECS_Context				mCtx;
	std::vector<EntityId>	mEntities;
};

//
//class TestComponentA {
//public:
//	int m_a;
//
//
//
//};
//template<> struct TypeName< TestComponentA > { static const char* Get(void) { return "TestComponentA"; } };
//
//class TestComponentB {
//public:
//	int m_b;
//
//	void Update(ComponentSystem<TestComponentA>& a) {
//
//	}
//
//	// generated
////private:
////	friend class ComponentManager;
////
////	public: [[nodiscard]] static auto members()->std::vector<std::string> = {
////		return {};
////	}
////
////	void UpdateShim(ComponentManager* mgr) {
////		ComponentSystem<TestComponentA>* sys1 = mgr->GetSystem<TestComponentA>();
////		Update(*sys1);
////	}
//
//};
//template<> struct TypeName< TestComponentB > { static const char* Get(void) { return "TestComponentB"; } };

// https://h3.gd/how-not-to-use-dlls/

/*
* 
* https://ruby0x1.github.io/machinery_blog_archive/post/dll-hot-reloading-in-theory-and-practice/index.html
* 
* https://github.com/fungos/cr
* 
int main()
{
	CoffBinary coff;
	if (openCoff("plugin.obj", &coff)) {
		coff.parse();

		unsigned long prevProtect;
		VirtualProtect(
			coff.rawData, coff.rawDataSize,
			PAGE_EXECUTE_READWRITE, &prevProtect);

		auto foo = (int(*)(int, int))
			(coff.rawData + coff.sections[2].PointerToRawData);
		printf("foo(2, 3) = %d\n", foo(2, 3));
	}

	getchar();
	return 0;
}

*/

 