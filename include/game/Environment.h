#pragma once

#include "Common.h"
#include "game/Terrain.h"
#include "serial/StreamBuffer.h"

#include <map>
#include <memory>
#include <set>

class Environment {

public:
    typedef std::shared_ptr<Environment> Shared;
    static Shared alloc();

    Environment();

    void addTerrain(uint32_t, Terrain::Shared);
    void removeTerrain(uint32_t);
    const std::map<uint32_t, Terrain::Shared>& terrain() const;

    Intersection intersect(const glm::vec3&, const glm::vec3&, bool ignoreBehindRay = true, bool backFaceCull = true) const;

private:
    // prevent copy-construction
    Environment(const Environment&);
    // prevent assignment
    Environment& operator=(const Environment&);

    std::map<uint32_t, Terrain::Shared> terrain_;
};

// Environment::Shared interpolate(const Environment::Shared&, const Environment::Shared&, float32_t);
