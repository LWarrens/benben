#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

namespace Benben {
typedef uint32_t EntityID;
typedef uint64_t ComponentID;
typedef uint64_t InterfaceID;
typedef unique_ptr<void, void (*)(void*)> ComponentPtrType;
typedef unordered_map<ComponentID, ComponentPtrType> ComponentMap;
namespace {
typedef unordered_set<InterfaceID> InterfaceSet;
template <typename T>
struct component_deleter {
  void operator()(T* ptr) const { delete ptr; }
};
}  // namespace
class Entity {
 public:
  template <typename T, typename D = component_deleter<T>, typename... _Args>
  T* create_component(const ComponentID& id, _Args&&... args);
  bool remove_component(ComponentID id);
  template <typename T = void>
  T* get_component(ComponentID id);

 private:
  InterfaceSet interface_set;
  ComponentMap component_map;

  friend class Environment;
};

template <typename T, typename Deleter, typename... _Args>
T* Entity::create_component(const ComponentID& id, _Args&&... args) {
  T* ret = new T(forward<_Args>(args)...);
  component_map[id] = std::make_unique(ret, Deleter());
  return ret;
}
bool Entity::remove_component(ComponentID id) {
  auto found = component_map.find(id);
  if (found != component_map.end()) {
    component_map.erase(found);
    return true;
  }
  return false;
}
template <typename T>
T* Entity::get_component(ComponentID id) {
  auto found = component_map.find(id);
  return found != component_map.end() ? (T*)found->second.get() : nullptr;
}

class Environment {
 public:
  EntityID create_entity();
  Entity* get_entity(EntityID entityID);
  void destroy_entity(EntityID entityID);
  void register_entity(EntityID entityID, InterfaceID interfaceID);
  void unregister_entity(EntityID entityID, InterfaceID interfaceID);
  void clear();
  vector<EntityID>* get_interface(InterfaceID interfaceID);
  size_t get_num_entities();

 private:
  unordered_map<EntityID, Entity> entity_container;
  unordered_map<InterfaceID, vector<EntityID>> interfaces;
  vector<EntityID> recyclable_IDs;
};

inline EntityID Environment::create_entity() {
  EntityID id;
  if (recyclable_IDs.size() > 0) {
    id = recyclable_IDs.back();
    recyclable_IDs.pop_back();
  } else {
    id = (EntityID)entity_container.size() + 1;
  }
  entity_container[id] = Entity();
  return id;
}

inline Entity* Environment::get_entity(EntityID entityID) {
  auto found = entity_container.find(entityID);
  return found != entity_container.end() ? &found->second : nullptr;
}

inline void Environment::destroy_entity(EntityID entityID) {
  InterfaceSet interface_set = entity_container[entityID].interface_set;
  for (auto interfaceID : interface_set) {
    unregister_entity(entityID, interfaceID);
  }
  if (entity_container.erase(entityID) != 0) {
    recyclable_IDs.push_back(entityID);
  }
}

inline void Environment::register_entity(EntityID entityID,
                                         InterfaceID interfaceID) {
  interfaces[interfaceID].push_back(entityID);
  entity_container[entityID].interface_set.insert(interfaceID);
}

inline void Environment::unregister_entity(EntityID entityID,
                                           InterfaceID interfaceID) {
  auto& _interface = interfaces[interfaceID];
  _interface.erase(remove(_interface.begin(), _interface.end(), entityID),
                   _interface.end());
  entity_container[entityID].interface_set.erase(interfaceID);
}

inline vector<EntityID>* Environment::get_interface(InterfaceID interfaceID) {
  auto found = interfaces.find(interfaceID);
  return found != interfaces.end() ? &found->second : nullptr;
}

inline size_t Environment::get_num_entities() {
  return entity_container.size();
}

inline void Environment::clear() {
  interfaces.clear();
  entity_container.clear();
}
}  // namespace Benben
