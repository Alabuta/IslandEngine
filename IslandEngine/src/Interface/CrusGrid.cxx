/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: grid rendering routines implementation.
****
********************************************************************************************************************************/
#include <vector>

#include "System\CrusSystem.h"

#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

#include "Renderer\CrusPosition.h"
#include "Interface\CrusGrid.h"

namespace isle {
namespace intf {
Position *Grid::Build(float _law, float _grle, uint16 _subdivs)
{
    float const div = _law / _grle;

    count_[0] = 8 * (uint16(div * (_subdivs - 1)) + 1);
    count_[1] = 8 * uint16(div - 1);

    std::vector<Position> data_;
    data_.reserve(count_[0] + count_[1] + 4 + 2 * 3);

    data_.emplace_back(Position(+_law, 0.0f, -_law));
    data_.emplace_back(Position(-_law, 0.0f, -_law));
    data_.emplace_back(Position(-_law, 0.0f, +_law));
    data_.emplace_back(Position(+_law, 0.0f, +_law));

    data_.emplace_back(Position(-_law, 0.0f, -_law));
    data_.emplace_back(Position(-_law, 0.0f, +_law));
    data_.emplace_back(Position(+_law, 0.0f, +_law));
    data_.emplace_back(Position(+_law, 0.0f, -_law));

    Position *const data = (Position *)malloc(sizeof(Position) * (count_[0] + count_[1] + 4 + 2 * 3));

    data[0] = Position(_law, 0.0f, -_law);    data[4] = Position(-_law, 0.0f, -_law);
    data[1] = Position(-_law, 0.0f, -_law);    data[5] = Position(-_law, 0.0f, _law);
    data[2] = Position(-_law, 0.0f, _law);    data[6] = Position(_law, 0.0f, _law);
    data[3] = Position(_law, 0.0f, _law);    data[7] = Position(_law, 0.0f, -_law);

    float incr = _grle / _subdivs, i = 0.0f;
    uint16 j = 8;

    for (uint32 k = 0; j < count_[0]; i += incr, ++k) {
        if ((k % (_subdivs - 1)) == 0ui16)
            i += incr;

        data[j++] = Position(-i, 0.0f, -_law);    data[j++] = Position(-i, 0.0f, _law);
        data[j++] = Position(i, 0.0f, _law);    data[j++] = Position(i, 0.0f, -_law);
        data[j++] = Position(-_law, 0.0f, -i);    data[j++] = Position(_law, 0.0f, -i);
        data[j++] = Position(_law, 0.0f, i);    data[j++] = Position(-_law, 0.0f, i);
    }

    i = _grle;

    for (; j < count_[0] + count_[1]; i += _grle) {
        data[j++] = Position(-i, 0.0f, -_law);    data[j++] = Position(-i, 0.0f, _law);
        data[j++] = Position(i, 0.0f, _law);    data[j++] = Position(i, 0.0f, -_law);
        data[j++] = Position(-_law, 0.0f, -i);    data[j++] = Position(_law, 0.0f, -i);
        data[j++] = Position(_law, 0.0f, i);    data[j++] = Position(-_law, 0.0f, i);
    }

    if (_grle == 0.0f || _subdivs == 0.0f)
        return data;

    data[j++] = Position(_law, 0.0f, 0.0f);    data[j++] = Position(-_law, 0.0f, 0.0f);
    data[j++] = Position(0.0f, 0.0f, _law);    data[j++] = Position(0.0f, 0.0f, -_law);

    data[j++] = Position(0.0f, 0.0f, 0.0f);     data[j++] = Position(1.0f, 0.0f, 0.0f);
    data[j++] = Position(0.0f, 0.0f, 0.0f);     data[j++] = Position(0.0f, 1.0f, 0.0f);
    data[j++] = Position(0.0f, 0.0f, 0.0f);     data[j++] = Position(0.0f, 0.0f, 1.0f);

    return data;
}

void Grid::Update(float _law, float _grle, uint16 _subdivs)
{
    shader_.AssignNew({"Interface/grid.glsl"});

    Position *data = Build(_law, _grle, _subdivs);
    if (data == nullptr)
        return;

    size_t const length = sizeof(Position) * (count_[0] + count_[1] + 4 + 2 * 3);

    if (glIsVertexArray(vao_) == GL_TRUE)
        glDeleteVertexArrays(1, &vao_);

    Render::inst().CreateVAO(vao_);

    {
        uint32 vbo = 0;
        Render::inst().CreateBO(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);

    glVertexAttribPointer(Program::nVERTEX, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(Program::nVERTEX);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    free(data);
}

void Grid::Draw()
{
    using namespace colors;

    shader_.SwitchOn();

    Render::inst().UpdateTransform(0, 1, &Render::inst().vp_.projView());

    glBindVertexArray(vao_);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kGRAY.c());
    glDrawArrays(GL_LINES, 0, count_[0]);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kDARKGRAY.c());
    glDrawArrays(GL_LINES, count_[0], count_[1]);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kBLACK.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1], 4);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kRED.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4, 2);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kGREEN.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4 + 2, 2);

    glUniform4fv(Program::eUNIFORM_ID::nMAIN_COLOR, 1, kBLUE.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4 + 2 + 2, 2);
}
};
};