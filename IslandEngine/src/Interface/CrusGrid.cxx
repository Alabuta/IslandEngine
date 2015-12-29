/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    8th September 2012 / 12th March 2010.
****    Description: grid rendering routines implementation.
****
****************************************************************************************/
#include "System\CrusSystem.h"

#include "Renderer\CrusRenderer.h"
#include "Renderer\CrusProgram.h"

#include "Renderer\CrusVertex.h"
#include "Interface\CrusGrid.h"

namespace isle
{
namespace intf
{
Vertex *Grid::Build(float _law, float _grle, uint16 _subdivs)
{
    float const div = _law / _grle;

    count_[0] = 8 * (uint16(div * (_subdivs - 1)) + 1);
    count_[1] = 8 *  uint16(div - 1);

    Vertex *const data = (Vertex *)malloc(sizeof(Vertex) * (count_[0] + count_[1] + 4 + 2 * 3));

    data[0] = Vertex( _law, 0.0f, -_law);    data[4] = Vertex(-_law, 0.0f, -_law);
    data[1] = Vertex(-_law, 0.0f, -_law);    data[5] = Vertex(-_law, 0.0f,  _law);
    data[2] = Vertex(-_law, 0.0f,  _law);    data[6] = Vertex( _law, 0.0f,  _law);
    data[3] = Vertex( _law, 0.0f,  _law);    data[7] = Vertex( _law, 0.0f, -_law);

    float incr = _grle / _subdivs, i = 0.0f;
    uint16 j = 8;

    for(uint32 k = 0; j < count_[0]; i += incr, ++k){
        if((k % (_subdivs - 1)) == 0ui16)
            i += incr;

        data[j++] = Vertex(-i, 0.0f, -_law);    data[j++] = Vertex(-i, 0.0f,  _law);
        data[j++] = Vertex( i, 0.0f,  _law);    data[j++] = Vertex( i, 0.0f, -_law);
        data[j++] = Vertex(-_law, 0.0f, -i);    data[j++] = Vertex( _law, 0.0f, -i);
        data[j++] = Vertex( _law, 0.0f,  i);    data[j++] = Vertex(-_law, 0.0f,  i);
    }

    i = _grle;

    for(; j < count_[0] + count_[1]; i += _grle){
        data[j++] = Vertex(-i, 0.0f, -_law);    data[j++] = Vertex(-i, 0.0f,  _law);
        data[j++] = Vertex( i, 0.0f,  _law);    data[j++] = Vertex( i, 0.0f, -_law);
        data[j++] = Vertex(-_law, 0.0f, -i);    data[j++] = Vertex( _law, 0.0f, -i);
        data[j++] = Vertex( _law, 0.0f,  i);    data[j++] = Vertex(-_law, 0.0f,  i);
    }

    if(_grle == 0.0f || _subdivs == 0.0f)
        return data;

    data[j++] = Vertex(_law, 0.0f,  0.0f);    data[j++] = Vertex(-_law, 0.0f,  0.0f);
    data[j++] = Vertex( 0.0f, 0.0f, _law);    data[j++] = Vertex( 0.0f, 0.0f, -_law);

    data[j++] = Vertex(0.0f, 0.0f, 0.0f);     data[j++] = Vertex(1.0f, 0.0f, 0.0f);
    data[j++] = Vertex(0.0f, 0.0f, 0.0f);     data[j++] = Vertex(0.0f, 1.0f, 0.0f);
    data[j++] = Vertex(0.0f, 0.0f, 0.0f);     data[j++] = Vertex(0.0f, 0.0f, 1.0f);

    return data;
}

void Grid::Update(float _law, float _grle, uint16 _subdivs)
{
    shader_.AssignNew({"Interface\\grid.glsl"});

    Vertex *data = Build(_law, _grle, _subdivs);
    if(data == nullptr)
        return;

    size_t const length = sizeof(Vertex) * (count_[0] + count_[1] + 4 + 2 * 3);

    if(glIsVertexArray(vao_) == GL_TRUE)
        glDeleteVertexArrays(1, &vao_);

    Renderer::inst().CreateVAO(vao_);

    {
        uint32 vbo = 0;
        Renderer::inst().CreateVBO(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);

    glVertexAttribPointer(Program::nVERTEX, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(Program::nVERTEX);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    free(data);

    Book::AddEvent(NOTE::nNOTICE, "grid updated (%0.2f; %0.2f; %hu).", _law, _grle, _subdivs);
}

void Grid::Draw()
{
    shader_.SwitchOn();
    glBindVertexArray(vao_);

    glUniform4fv(shader_.GetUniformLoc("LineColor"), 1, kGRAY.c());
    glDrawArrays(GL_LINES, 0, count_[0]);

    glUniform4fv(Program::GetLasUniformLoc(), 1, kDARKGRAY.c());
    glDrawArrays(GL_LINES, count_[0], count_[1]);

    glUniform4fv(Program::GetLasUniformLoc(), 1, kBLACK.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1], 4);

    glUniform4fv(Program::GetLasUniformLoc(), 1, kRED.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4, 2);

    glUniform4fv(Program::GetLasUniformLoc(), 1, kGREEN.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4 + 2, 2);

    glUniform4fv(Program::GetLasUniformLoc(), 1, kBLUE.c());
    glDrawArrays(GL_LINES, count_[0] + count_[1] + 4 + 2 + 2, 2);
}
};
};