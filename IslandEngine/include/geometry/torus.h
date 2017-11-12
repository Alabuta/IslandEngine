#pragma once

#ifndef CRUS_TORUS_H                // Include guard "torus.h"
#define CRUS_TORUS_H

#include <array>
#include <vector>
#include "engine.h"

namespace torus {

std::vector<isle::Position> positions;

std::vector<isle::math::Vector> normals;

std::vector<isle::UV> uvs;

std::vector<uint16> indices;

void generate(float radius = .5f, float tube = 0.16f, uint16 radialSegments = 64, uint16 tubularSegments = 24, float arc = 2 * isle::math::kPI)
{
    float u, v;

    isle::Position position;
    isle::math::Vector normal;
    isle::UV uv;

    for (uint16 j = 0; j <= radialSegments; ++j) {
        for (uint16 i = 0; i <= tubularSegments; ++i) {
            uv.u = i / static_cast<float>(tubularSegments);
            uv.v = j / static_cast<float>(radialSegments);

            u = uv.u * arc;
            v = uv.v * 2 * isle::math::kPI;

            position.x = (radius + tube * std::cos(v)) * std::cos(u);
            position.y = (radius + tube * std::cos(v)) * std::sin(u);
            position.z = tube * std::sin(v);

            normal.x = radius * std::cos(u);
            normal.y = radius * std::sin(v);
            normal.z = 0;

            normal = isle::math::Vector(position.xyz) - normal;
            normal.Normalize();

            positions.push_back(std::move(position));
            normals.push_back(std::move(normal));
            uvs.push_back(std::move(uv));
        }
    }

    for (uint16 j = 1; j <= radialSegments; ++j) {
        for (uint16 i = 1; i <= tubularSegments; ++i) {
            indices.insert(indices.end(), {
                static_cast<uint16>((tubularSegments + 1) * j + i - 1),
                static_cast<uint16>((tubularSegments + 1) * (j - 1) + i - 1),
                static_cast<uint16>((tubularSegments + 1) * j + i) }
            );

            indices.insert(indices.end(), {
                static_cast<uint16>((tubularSegments + 1) * (j - 1) + i - 1),
                static_cast<uint16>((tubularSegments + 1) * (j - 1) + i),
                static_cast<uint16>((tubularSegments + 1) * j + i) }
            );
        }
    }
}
};

#endif // CRUS_TORUS_H