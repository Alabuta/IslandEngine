/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: grid rendering routines implementation.
****
********************************************************************************************************************************/
#include <vector>
#include <tuple>
#include <cmath>

#include "System\CrusSystem.h"

#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

#include "Renderer\CrusPosition.h"
#include "Interface\CrusGrid.h"

namespace isle::intf {
auto Grid::Build(float _side, float _step, u16 _subdivs)
{
    auto lines = static_cast<u16>(std::floor(_side / _step));

    auto const total = static_cast<decltype(lines)>(_side / (_step / _subdivs));
    auto const sub = _step / _subdivs;

    std::vector<std::pair<Position, Color>> data;
    data.reserve(total * 2 * 4 + 4 + 8 + 6);

    auto color = colors::kGRAY;

    for (auto i = 1; i <= total; ++i) {
        color = i % _subdivs == 0 ? colors::kGRAY : colors::kDARKGRAY;

        data.emplace_back(Position{+i * sub, 0.f, +_side}, color);
        data.emplace_back(Position{+i * sub, 0.f, -_side}, color);
        data.emplace_back(Position{-i * sub, 0.f, +_side}, color);
        data.emplace_back(Position{-i * sub, 0.f, -_side}, color);

        data.emplace_back(Position{+_side, 0.f, +i * sub}, color);
        data.emplace_back(Position{-_side, 0.f, +i * sub}, color);
        data.emplace_back(Position{+_side, 0.f, -i * sub}, color);
        data.emplace_back(Position{-_side, 0.f, -i * sub}, color);
    }

    data.emplace_back(Position{+_side, 0.f, -_side}, colors::kDARKGRAY);
    data.emplace_back(Position{-_side, 0.f, -_side}, colors::kDARKGRAY);
    data.emplace_back(Position{-_side, 0.f, +_side}, colors::kDARKGRAY);
    data.emplace_back(Position{+_side, 0.f, +_side}, colors::kDARKGRAY);

    data.emplace_back(Position{-_side, 0.f, -_side}, colors::kDARKGRAY);
    data.emplace_back(Position{-_side, 0.f, +_side}, colors::kDARKGRAY);
    data.emplace_back(Position{+_side, 0.f, +_side}, colors::kDARKGRAY);
    data.emplace_back(Position{+_side, 0.f, -_side}, colors::kDARKGRAY);

    data.emplace_back(Position{0.f, 0.f, +_side}, colors::kBLACK);
    data.emplace_back(Position{0.f, 0.f, -_side}, colors::kBLACK);
    data.emplace_back(Position{+_side, 0.f, 0.f}, colors::kBLACK);
    data.emplace_back(Position{-_side, 0.f, 0.f}, colors::kBLACK);

    data.emplace_back(Position{0.f, 0.f, 0.f}, colors::kRED);
    data.emplace_back(Position{1.f, 0.f, 0.f}, colors::kRED);

    data.emplace_back(Position{0.f, 0.f, 0.f}, colors::kGREEN);
    data.emplace_back(Position{0.f, 1.f, 0.f}, colors::kGREEN);

    data.emplace_back(Position{0.f, 0.f, 0.f}, colors::kBLUE);
    data.emplace_back(Position{0.f, 0.f, 1.f}, colors::kBLUE);

    count_ = static_cast<decltype(count_)>(data.size());

    return data;
}

void Grid::Update(float _side, float _step, u16 _subdivs)
{
    shader_.AssignNew({"Interface/grid.glsl"});

    if (_side < 1.f) {
        log::Warning() << "side value (length and width) may not be less than 1.";
        _side = 15.f;
    }

    if (_step < 0.f) {
        log::Warning() << "step value (grid line spacing) must be greater than 0.";
        _step = 1.f;
    }

    if (_subdivs < 1) {
        log::Warning() << "subdivisions value may not be less than 1.";
        _subdivs = 5;
    }

    auto data = Build(_side, _step, _subdivs);

    if (data.empty())
        return;

    if (glIsVertexArray(vao_) == GL_TRUE)
        glDeleteVertexArrays(1, &vao_);

    Render::inst().CreateVAO(vao_);

    {
        auto bo = 0u;
        Render::inst().CreateBO(bo);
        glNamedBufferStorage(bo, sizeof(data[0]) * data.size(), data.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayAttribBinding(vao_, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(vao_, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao_, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayAttribBinding(vao_, Render::eVERTEX_IN::nCOLOR, 0);
        glVertexArrayAttribFormat(vao_, Render::eVERTEX_IN::nCOLOR, 3, GL_FLOAT, GL_FALSE, sizeof(data[0].first));
        glEnableVertexArrayAttrib(vao_, Render::eVERTEX_IN::nCOLOR);

        glVertexArrayVertexBuffer(vao_, 0, bo, 0, sizeof(data[0]));
    }
}

void Grid::Draw()
{
    shader_.UseThis();

    glBindVertexArray(vao_);

    glDrawArrays(GL_LINES, 0, count_);
}
};