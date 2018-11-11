#include "engine.h"
using namespace isle;

namespace cubemap {

auto cubemapIndirectBO = 0u;

Program cubemapProgram;
u32 cubemapVAO;
Texture cubemapTexture(Texture::eTEXTURE_TYPE::nCUBE, R"(Skybox/skybox)");

bool InitCubemap()
{
    if (!cubemapTexture.Init())
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

    Render::inst().CreateVAO(cubemapVAO);

    {
        Render::inst().CreateBO(cubemapIndirectBO);

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
        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(vertices), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayAttribBinding(cubemapVAO, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(cubemapVAO, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(cubemapVAO, 0, bo, 0, sizeof(Position));

        glEnableVertexArrayAttrib(cubemapVAO, Render::eVERTEX_IN::nPOSITION);
    }

    return true;
}

void DrawCubemap()
{
    cubemapTexture.Bind();
    cubemapProgram.bind();

    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cubemapIndirectBO);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
}
}