/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 4th May 2012.
****    Description: game objects implemetation file.
****
****************************************************************************************/
#include <string>

#include "System\CrusSystem.h"

#include "System\CrusMouse.h"

#include "Math\CrusMath.h"
#include "Math\CrusVector.h"
#include "Math\CrusMatrix.h"

#include "Renderer\CrusRenderer.h"
#include "Renderer\CrusProgram.h"
#include "Renderer\CrusTexture.h"

#include "Game\CrusObject.h"

static isle::math::Matrix matrix;

namespace isle
{
Object::eSTATE Object::state = Object::eSTATE::nIDLE;

Program Object::program;

Object::Object()
{
    mModel_.MakeIdentity();
}

void Object::Init(Texture const *_texture, float _zoom, uint16 _x, uint16 _y, uint16 _w, uint16 _h)
{
    if (_texture == nullptr) {
        Book::AddEvent(NOTE::nERROR, "texture pointer is invalid.");
        return;
    }

    else texture(_texture);

    mModel_.MakeIdentity();

    w(static_cast<float>(_w));
    h(static_cast<float>(_h));

    float const ratio = h() / w();

    rect()[0] = static_cast<float>(_x) / texture_->w();
    rect()[1] = static_cast<float>(_y) / texture_->h();

    float const quad_plane[] = {
        -_zoom,  _zoom * ratio, 0.0f, 0.0f,                h() / texture_->h(),
        -_zoom, -_zoom * ratio, 0.0f, 0.0f,                0.0f,
         _zoom,  _zoom * ratio, 0.0f, w() / texture_->w(), h() / texture_->h(),
         _zoom, -_zoom * ratio, 0.0f, w() / texture_->w(), 0.0f
    };

    isle::Renderer::inst().CreateVAO(vao_);
    isle::Renderer::inst().CreateVBO(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);

    glVertexAttribPointer(isle::Program::nVERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(isle::Program::nVERTEX);

    glVertexAttribPointer(isle::Program::nTEXCRD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void const *>(sizeof(float) * 3));
    glEnableVertexAttribArray(isle::Program::nTEXCRD);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Object::Render()
{
    Renderer::inst().UpdateTRSM(0, 1, &mModel_);

    glProgramUniform4fv(program.GetName(), program.GetUniformLoc("inDispCoord"), 1, rect());

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Bird::Start()
{
    accel_[0] = 0.64f;
    accel_[1] = 9.8f * 5.0f;

    vel_ = 0;//accel_[1] * System::time.delta();
    vel_prev_ = 0.0f;

    //roll_ = 0.0f;
}

void Bird::Update()
{
    static float frames = 0, elapsedSeconds = 0;
    elapsedSeconds += System::time.delta();

    static float rollRestrict = 16.0f;

    switch (state) {
        case eSTATE::nIDLE:
            x_ = 0.0f;
            y_ = -0.16f;

            vel_prev_ = 0.0f;
            roll_ = 0.0f;

            y_ -= std::cosf(2.0f * math::kPI * elapsedSeconds / 0.72f) * 0.016f;
            break;

        case eSTATE::nPLAYING:
            rollRestrict = 16.0f;

            Input::Mouse::CheckKeySync(VK_LBUTTON,
                [&, this]() {
                frames = 0.0f;
                vel_prev_ = 0.0f;
                roll_ = 0.0f;
            }, [&, this]() {
                frames += System::time.delta();

                if (frames > 0.061f)
                    accel_[1] = -kInitialAccel;

                else accel_[1] = 5.0f * kInitialAccel;
            }, []() {},
                [&, this]() {
                accel_[1] = -kInitialAccel;
            });

            break;

        case eSTATE::nSTOPPED:
            elapsedSeconds = 0.0f;
            accel_[0] = 0.0f;
            accel_[1] = -kInitialAccel;
            //accel_[1] = 0.0f;

            //vel_prev_ += -accel_[1] * System::time.delta();

            rect()[2] = w() / texture()->w();
            break;
    }

    x_ += accel_[0] * System::time.delta();
    vel_ = accel_[1] * System::time.delta();

    y_ += vel_prev_ * System::time.delta() + vel_ * System::time.delta() * 0.5f;
    vel_prev_ += vel_;

    if (state != eSTATE::nSTOPPED)
        rect()[2] = (std::roundf(std::sinf(3.2f * math::kPI * elapsedSeconds)) + 1.0f) * w() / texture()->w();

    if (roll_ < 90.0f)
        roll_ -= rollRestrict * vel_;

    mModel_.RotateOnOZ(-roll_);
    float const zoom = -0.002511f * Renderer::inst().vp_.w() * 336 * 0.5f / 288.0f + kRadius;

    if (state == eSTATE::nSTOPPED && y_ < zoom)
        y_ = zoom;

    Camera::inst().SetPos(x_ + kDispX, Camera::inst().pos().y(), Camera::inst().pos().z());
}

void Ground::Init(Texture const *_texture, float _zoom, uint16 _x, uint16 _y, uint16 _w, uint16 _h)
{
    Object::Init(_texture, _zoom, _x, _y, _w, _h);

    float const ratio = h() / w();

    float const quad_plane[] = {
		-_zoom,  0.0f,                 0.0f, 0.0f,                         h() / texture_->h(),
		-_zoom, -2.0f * _zoom * ratio, 0.0f, 0.0f,                         0.0f,
         _zoom,  0.0f,                 0.0f, 0.5f * w() / texture_->w(),   h() / texture_->h(),
         _zoom, -2.0f * _zoom * ratio, 0.0f, 0.5f * w() / texture_->w(),   0.0f
    };

    glNamedBufferDataEXT(vbo_, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);
}

void Ground::Update()
{
    if (state == eSTATE::nSTOPPED)
        return;

    rect()[2] += 0.64f * 132.0f / texture_->w() * System::time.delta();

    if (rect()[2] > 0.5f * w() / texture_->w())
        rect()[2] -= 0.5f * w() / texture_->w();

    mModel_.Translate(Camera::inst().pos().x(), y(), 0);
}

void Pillar::Init(Texture const *_texture, float _zoom, uint16 _x, uint16 _y, uint16 _w, uint16 _h)
{
    Object::Init(_texture, _zoom, _x, _y, _w, _h);

    float const ratio = h() / w();

    float const quad_plane[] = {
		-_zoom, 0.16f + 2 * _zoom * ratio,  0.0f, 0.0f,                h() / texture_->h(),
		-_zoom, 0.16f,                      0.0f, 0.0f,                0.0f,
         _zoom, 0.16f + 2 * _zoom * ratio,  0.0f, w() / texture_->w(), h() / texture_->h(),
         _zoom, 0.16f,                      0.0f, w() / texture_->w(), 0.0f
    };

    glNamedBufferDataEXT(vbo_, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);
}

void Pillar::Render()
{
    Renderer::inst().UpdateTRSM(0, 1, &mModel_);

    glProgramUniform4fv(program.GetName(), program.GetUniformLoc("inDispCoord"), 1, rect());

    Renderer::inst().UpdateTRSM(0, 1, &mModel_);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    matrix = mModel_;
    matrix.Scale(1, -1, 1);
    Renderer::inst().UpdateTRSM(0, 1, &matrix);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Title::Update()
{
    if (state == eSTATE::nPLAYING)
        return;

    static float elapsedSeconds = 0;
    elapsedSeconds += System::time.delta();

    switch (state) {
        case eSTATE::nIDLE:
            rect()[2] = 0;
            break;

        case eSTATE::nPLAYING:
            break;

        case eSTATE::nSTOPPED:
            rect()[2] = w() / texture()->w();
            break;
    }

    mModel_.Translate(Camera::inst().pos().x(), y(), z());
}

void Button::Update()
{
    if (state == eSTATE::nPLAYING)
        return;

    static float elapsedSeconds = 0;
    elapsedSeconds += System::time.delta();

    switch (state) {
        case eSTATE::nIDLE:
            rect()[2] = 0;
            break;

        case eSTATE::nPLAYING:
            break;

        case eSTATE::nSTOPPED:
            rect()[2] = w() / texture()->w();

            POINT pt;
            GetCursorPos(&pt);

            Input::Mouse::CheckKeySync(VK_LBUTTON,
                [&, this]() {
                    if (pt.x > 884 && pt.x < 1031 && pt.y > 600 && pt.y < 675) {
                        y() -= 0.01f;
                    }
                }, []() {}, [&, this]() {
                    if (pt.x > 884 && pt.x < 1031 && pt.y > 600 && pt.y < 675) {
                        state = Object::eSTATE::nIDLE;
                        y() += 0.01f;
                    }
                }, []() {});


            break;
    }

    mModel_.Translate(Camera::inst().pos().x(), y(), z());
}
};