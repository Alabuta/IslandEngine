#include "engine.h"
using namespace isle;

namespace cubemap {

auto cubemapIndirectBO = 0u;

Program cubemapProgram;
uint32 cubemapVAO;
Texture cubemapTexture(Texture::eTEXTURE_TYPE::nCUBE, R"(Skybox/skybox)");

bool InitCubemap()
{
    if (!cubemapTexture.Init())
        return false;

    cubemapProgram.AssignNew({R"(Defaults/Skybox-Cubed.glsl)"});

    std::array<Position, 4 * 6 + 4> const vertices = {
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
    };

    Render::inst().CreateVAO(cubemapVAO);

    {
        Render::inst().CreateBO(cubemapIndirectBO);

        struct {
            uint32  count;
            uint32  instanceCount;
            uint32  first;
            uint32  baseInstance;
        } const command = {
            static_cast<uint32>(vertices.size()), 1, 0, 0
        };

        glNamedBufferStorage(cubemapIndirectBO, sizeof(command), &command, GL_DYNAMIC_STORAGE_BIT);
    }

    {
        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(vertices), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayAttribBinding(cubemapVAO, Program::eIN_OUT_ID::nVERTEX, 0);
        glVertexArrayAttribFormat(cubemapVAO, Program::eIN_OUT_ID::nVERTEX, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(cubemapVAO, 0, bo, 0, sizeof(Position));

        glEnableVertexArrayAttrib(cubemapVAO, Program::eIN_OUT_ID::nVERTEX);
    }

    return true;
}

void DrawCubemap()
{
    cubemapTexture.Bind();
    cubemapProgram.UseThis();

    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cubemapIndirectBO);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
}
}