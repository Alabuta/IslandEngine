#include "engine.h"
using namespace isle;

namespace cubemap {

auto cubemapIndirectBO = 0u;

Program cubemapProgram;
u32 cubemapVAO;
Texture envmap(Texture::eTEXTURE_TYPE::nCUBE, R"(Skybox/skybox)");

Texture const &texture()
{
    return envmap;
}

bool InitCubemap()
{
    if (!envmap.Init())
        return false;

    cubemapProgram.AssignNew({R"(Defaults/Skybox-Cubed.glsl)"});

    auto constexpr vertices = make_array(
        Position{+1, +1, +1},
        Position{-1, +1, +1},
        Position{+1, +1, -1},
        Position{-1, +1, -1},

        Position{-1, +1, -1},
        Position{-1, +1, -1},

        Position{-1, +1, -1},
        Position{-1, -1, -1},
        Position{+1, +1, -1},
        Position{+1, -1, -1},

        Position{+1, +1, -1},
        Position{+1, -1, -1},
        Position{+1, +1, +1},
        Position{+1, -1, +1},

        Position{+1, +1, +1},
        Position{+1, -1, +1},
        Position{-1, +1, +1},
        Position{-1, -1, +1},

        Position{-1, +1, +1},
        Position{-1, -1, +1},
        Position{-1, +1, -1},
        Position{-1, -1, -1},

        Position{-1, -1, -1},
        Position{-1, -1, -1},

        Position{-1, -1, -1},
        Position{-1, -1, +1},
        Position{+1, -1, -1},
        Position{+1, -1, +1}
    );

    cubemapVAO = Render::inst().createVAO();

    {
        cubemapIndirectBO = Render::inst().createBO();

        struct {
            u32  count;
            u32  instanceCount;
            u32  first;
            u32  baseInstance;
        } const command = {
            static_cast<u32>(vertices.size()), 1, 0, 0
        };

        glNamedBufferStorage(cubemapIndirectBO, sizeof(command), &command, GL_DYNAMIC_STORAGE_BIT);
    }

    {
        auto bo = Render::inst().createBO();

        glNamedBufferStorage(bo, sizeof(vertices), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayAttribBinding(cubemapVAO, semantic::position::index, 0);
        glVertexArrayAttribFormat(cubemapVAO, semantic::position::index, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(cubemapVAO, 0, bo, 0, sizeof(Position));

        glEnableVertexArrayAttrib(cubemapVAO, semantic::position::index);
    }

    return true;
}

void DrawCubemap()
{
    envmap.bind();
    cubemapProgram.bind();

    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cubemapIndirectBO);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
}
}