/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 4th May 2012.
****    Description: game objects definition file.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_OBJECT_H               // Include guard "CrusObject.h"
#define CRUS_OBJECT_H

namespace isle
{
class Object {
public:
    static enum class eSTATE {nIDLE = 0, nPLAYING, nSTOPPED} state;

    static Program program;

    Object();
    ~Object() {};
    
    virtual void Init(Texture const *texture, float zoom, uint16 x, uint16 y, uint16 w, uint16 h);

    virtual void Start() {};

    virtual void Update(){};
    virtual void Render();

    float *const rect() { return texDispCoord_.rect_; };

    float const &w() const { return rectSprite_.w_; };
    float const &h() const { return rectSprite_.h_; };

    float const &w(float w) { return (rectSprite_.w_ = w); };
    float const &h(float h) { return (rectSprite_.h_ = h); };

    /*math::Vector const &pos() const { return pos_; };
    math::Vector const &pos(math::Vector pos) { return pos_ = pos; };*/

    float &x() { return x_; };
    float &y() { return y_; };
    float &z() { return z_; };

    math::Matrix &mModel() { return mModel_; };
    math::Matrix const &mModel() const { return mModel_; };

    Texture const *const texture() const { return texture_; };
    Texture const *const texture(Texture const *texture) { return (texture_ = texture); };

protected:
    uint32 vao_{0}, vbo_{0};

    struct {
        float w_{0}, h_{0};
    } rectSprite_;

    union {
        struct {
            float x_, y_, w_, h_;
        };

        struct {
            float rect_[4];
        };
    } texDispCoord_;

    math::Vector pos_{0, 0, 0};

    union {
        struct {
            math::Matrix mModel_;
        };

        struct {
            math::Vector xAxis_; float x_;
            math::Vector yAxis_; float y_;
            math::Vector zAxis_; float z_;
        };
    };

    Texture const *texture_{nullptr};
};

class Bird : public Object {
private:
    float const kDispX{0.32f}, kInitialAccel{9.8f * 0.5f};

public:
    float const kRadius{0.04f};

    float accel_[2]{0.64f, 0};
    float vel_prev_{0}, vel_{0};
    float roll_{0};

    void Start() override;
    void Update() override;
};

class Ground : public Object {
public:

    void Init(Texture const *texture, float zoom, uint16 x, uint16 y, uint16 w, uint16 h) override;
    void Update() override;
};

class Pillar : public Object {
public:

    void Init(Texture const *texture, float zoom, uint16 x, uint16 y, uint16 w, uint16 h) override;
    void Render() override;
};

class Title : public Object {
public:

    void Update() override;
};

class Button : public Object {
public:

    void Update() override;
};
};

#endif // CRUS_OBJECT_H