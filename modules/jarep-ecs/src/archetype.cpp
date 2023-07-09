//
// Created by Sebastian Borsch on 01.07.23.
//

#include "archetype.hpp"

Archetype *Archetype::createEmpty() {
    auto instance = new Archetype();
    return instance;
}

template<class T>
Archetype *Archetype::createFromAdd(Archetype &fromArchetype) {

    auto instance = new Archetype();
    // Take the existing archetype and create a new component instance collection with only empty vectors.
    size_t fromColumnLength = fromArchetype.componentCollectionsLength;
    for (int i = 0; i < fromColumnLength; ++i) {
        instance->componentCollections[i] = fromArchetype.componentCollections[i]->createNewAndEmpty();
    }
    instance->componentCollections[fromColumnLength] = new InstanceCollection<T>();
    instance->componentCollectionsLength += 1;

    // Iterate over the exisiting typemap and copy it, as well as collecting all types from the old archetype.
    instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
    auto typesInArchetype = new std::vector<std::type_index>();
    for (const auto typeEntry: fromArchetype.componentTypeMap) {
        instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
        typesInArchetype->push_back(typeEntry.first);
    }

    // Assign the new generic component to all maps and lists so the new archetype will be different from the old one.
    instance->componentTypeMap.insert_or_assign(typeid(T), instance->componentCollectionsLength);
    typesInArchetype->push_back(typeid(T));

    // Create a new archetype instance and return it.
    //auto instance = new Archetype();
    //instance->componentCollections = *columns;
    //instance->componentTypeMap = *typeMap;
    instance->generate_hash(typesInArchetype);

    return instance;
}

template<class T>
Archetype *Archetype::createFromRemove(Archetype &fromArchetype)
{
    auto instance = new Archetype();

    // Start with coping the existing types and filter out the index of the type to remove in the process.
    auto typeMap = new std::unordered_map<std::type_index, size_t>();
    auto typesInArchetype = new std::vector<std::type_index>();
    size_t targetIndexToRemove = -1; // Setting this to an invalid value to provoke an error if nothing happens here.TODO: Add proper error handling here.
    for (const auto typeEntry: fromArchetype.componentTypeMap) {

        // If the typeid is the one to remove we do not copy it memorize the index of that type in the archetype lists.
        if (typeEntry.first == typeid(T)) {
            targetIndexToRemove = typeEntry.second;
            continue;
        }

        typeMap->insert_or_assign(typeEntry.first, typeEntry.second);
        typesInArchetype->push_back(typeEntry.first);
    }


    // Copy the component lists and instantiate them empty except for the collection at the memorized index.
    size_t newComponentCollectionsLength = fromArchetype.componentCollectionsLength--;
    for (int i = 0; i < newComponentCollectionsLength; ++i) {
        if (i == targetIndexToRemove) continue;
        instance->componentCollections[i] = fromArchetype.componentCollections[i]->createNewAndEmpty();
    }

    // Create a new archetype and return the instance pointer.

    instance->componentTypeMap = *typeMap;
    instance->generate_hash(typesInArchetype);

    return instance;
}

void Archetype::generate_hash(std::vector<std::type_index> *componentTypes) {
    // Create a unique hash from the component types in the archetype.
    size_t seed = componentTypes->size();
    for (const auto type: *componentTypes) {
        // ChatGPT suggested these constants to generate more secure and unique hash values.
        seed ^= type.hash_code() * 0x9e3779b9 + (seed << 6) + (seed << 2);
    }
    typeHash = seed;
}

void Archetype::removeEntity(Entity &entity) {

}

void Archetype::migrateEntity(Archetype *from, Entity entity) {

}

template<class T>
std::optional<std::vector<std::tuple<T *, Entity>>> Archetype::getComponentsWithEntities() {
    return std::optional<std::vector<std::tuple<T *, Entity>>>();
}

template<class T>
std::optional<T *> Archetype::getComponent(size_t index) {
    return std::optional<T *>();
}

template<class T>
void Archetype::setComponentInstance(T componentInstance) {

}

template<class T>
bool Archetype::containsType() {
    return 0;
}

