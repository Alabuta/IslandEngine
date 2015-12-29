/****************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    27th November 2013 / 2nd April 2012.
****    Description: Quaternion class unit-testing.
****
****************************************************************************************/
#include "Math\CrusQuaternion.h"

UNIT_SUITE_CLASS(isle::math::Quaternion)
{
    using xUnit::rand;

    // The class constructor testing...
    {
        float const q[4] = {rand<float>(), rand<float>(), rand<float>(), rand<float>()};
        Quaternion const q1(q[0], q[1], q[2], q[3]), q2(q), q3(q2);

        CHECK_EQUAL(q[0], q1.w_, "class member 'w'")
        CHECK_EQUAL(q[1], q1.x_, "class member 'x'")
        CHECK_EQUAL(q[2], q1.y_, "class member 'y'")
        CHECK_EQUAL(q[3], q1.z_, "class member 'z'")

        CHECK_EQUAL(q[0], q2.w_, "class member 'w'")
        CHECK_EQUAL(q[1], q2.x_, "class member 'x'")
        CHECK_EQUAL(q[2], q2.y_, "class member 'y'")
        CHECK_EQUAL(q[3], q2.z_, "class member 'z'")

        CHECK_EQUAL(q2.w_, q3.w_, "class member 'w'")
        CHECK_EQUAL(q2.x_, q3.x_, "class member 'x'")
        CHECK_EQUAL(q2.y_, q3.y_, "class member 'y'")
        CHECK_EQUAL(q2.z_, q3.z_, "class member 'z'")
    }

    // Getters...
    {
        float const w = rand<float>();
        float const x = rand<float>(), y = rand<float>(), z = rand<float>();
        Quaternion const q(w, x, y, z);

        CHECK_EQUAL(w, q.w(), "w() const")
        CHECK_EQUAL(x, q.x(), "x() const")
        CHECK_EQUAL(y, q.y(), "y() const")
        CHECK_EQUAL(z, q.z(), "z() const")

        CHECK_EQUAL(w, q.q()[0], "q()[0] const")
        CHECK_EQUAL(x, q.q()[1], "q()[1] const")
        CHECK_EQUAL(y, q.q()[2], "q()[2] const")
        CHECK_EQUAL(z, q.q()[3], "q()[3] const")
    }

    // Setters...
    {
        float const w = rand<float>();
        float const x = rand<float>(), y = rand<float>(), z = rand<float>();
        Quaternion q;

        CHECK_EQUAL(w, q.w(w), "w(float)")
        CHECK_EQUAL(x, q.x(x), "x(float)")
        CHECK_EQUAL(y, q.y(y), "y(float)")
        CHECK_EQUAL(z, q.z(z), "z(float)")

        CHECK_EQUAL(w, q.w() = w, "w()")
        CHECK_EQUAL(x, q.x() = x, "x()")
        CHECK_EQUAL(y, q.y() = y, "y()")
        CHECK_EQUAL(z, q.z() = z, "z()")
    }

    // The sum and sub operators...
    {
        float const w1 = rand<float>(), w2 = rand<float>();
        float const x1 = rand<float>(), y1 = rand<float>(), z1 = rand<float>();
        float const x2 = rand<float>(), y2 = rand<float>(), z2 = rand<float>();

        Quaternion const q1(w1, x1, y1, z1), q2(w2, x2, y2, z2);

        CHECK_EQUAL(w1 + w2, (q1 + q2).w(), "operator+ (Quaternion const &) const: 'w'")
        CHECK_EQUAL(x1 + x2, (q1 + q2).x(), "operator+ (Quaternion const &) const: 'x'")
        CHECK_EQUAL(y1 + y2, (q1 + q2).y(), "operator+ (Quaternion const &) const: 'y'")
        CHECK_EQUAL(z1 + z2, (q1 + q2).z(), "operator+ (Quaternion const &) const: 'z'")

        CHECK_EQUAL(w1 - w2, (q1 - q2).w(), "operator- (Quaternion const &) const: 'w'")
        CHECK_EQUAL(x1 - x2, (q1 - q2).x(), "operator- (Quaternion const &) const: 'x'")
        CHECK_EQUAL(y1 - y2, (q1 - q2).y(), "operator- (Quaternion const &) const: 'y'")
        CHECK_EQUAL(z1 - z2, (q1 - q2).z(), "operator- (Quaternion const &) const: 'z'")
    }

    // The cross and dot product operators...
    {
        Quaternion q1(0.0f, -1.0f, -1.0f, 0.0f), q2(0.0f, 0.0f, 1.0f, 0.0f);

        q1.Normalize();
        q2.Normalize();

        /*CHECK_EQUAL(+0.0f, (q1 ^ q2).x(), "operator^ (Quaternion const &) const: 'x'")
        CHECK_EQUAL(+0.0f, (q1 ^ q2).y(), "operator^ (Quaternion const &) const: 'y'")
        CHECK_EQUAL(-1.0f, (q1 ^ q2).z(), "operator^ (Quaternion const &) const: 'z'")*/

        float const expected = cosf(DegToRad(270.0f / 2.0f));
        CHECK_EQUAL(expected, q1 * q2, "operator* (Quaternion const &) const")
    }

    // The sum, sub, mult and diq with scalar operators...
    {
        float const w = rand<float>();
        float const x = rand<float>(), y = rand<float>(), z = rand<float>();
        float const s = rand<float>();

        Quaternion const q(w, x, y, z);

        CHECK_EQUAL(w + s, (q + s).w(), "operator+ (float) const: 'w'")
        CHECK_EQUAL(x + s, (q + s).x(), "operator+ (float) const: 'x'")
        CHECK_EQUAL(y + s, (q + s).y(), "operator+ (float) const: 'y'")
        CHECK_EQUAL(z + s, (q + s).z(), "operator+ (float) const: 'z'")

        CHECK_EQUAL(w - s, (q - s).w(), "operator- (float) const: 'w'")
        CHECK_EQUAL(x - s, (q - s).x(), "operator- (float) const: 'x'")
        CHECK_EQUAL(y - s, (q - s).y(), "operator- (float) const: 'y'")
        CHECK_EQUAL(z - s, (q - s).z(), "operator- (float) const: 'z'")

        CHECK_EQUAL(w * s, (q * s).w(), "operator* (float) const: 'w'")
        CHECK_EQUAL(x * s, (q * s).x(), "operator* (float) const: 'x'")
        CHECK_EQUAL(y * s, (q * s).y(), "operator* (float) const: 'y'")
        CHECK_EQUAL(z * s, (q * s).z(), "operator* (float) const: 'z'")

        CHECK_EQUAL(w / s, (q / s).w(), "operator/ (float) const: 'w'")
        CHECK_EQUAL(x / s, (q / s).x(), "operator/ (float) const: 'x'")
        CHECK_EQUAL(y / s, (q / s).y(), "operator/ (float) const: 'y'")
        CHECK_EQUAL(z / s, (q / s).z(), "operator/ (float) const: 'z'")
    }

    // The assignment operators...
    {
        float const s = rand<float>();
        Quaternion q;

        CHECK_EQUAL(s, (q = s).w(), "operator= (float): 'w'")
        CHECK_EQUAL(s, (q = s).x(), "operator= (float): 'x'")
        CHECK_EQUAL(s, (q = s).y(), "operator= (float): 'y'")
        CHECK_EQUAL(s, (q = s).z(), "operator= (float): 'z'")

        Quaternion q1, q2(rand<float>(), rand<float>(), rand<float>(), rand<float>());

        CHECK_EQUAL(q2.w(), (q1 = q2).w(), "operator= (Quaternion const &): 'w'")
        CHECK_EQUAL(q2.x(), (q1 = q2).x(), "operator= (Quaternion const &): 'x'")
        CHECK_EQUAL(q2.y(), (q1 = q2).y(), "operator= (Quaternion const &): 'y'")
        CHECK_EQUAL(q2.z(), (q1 = q2).z(), "operator= (Quaternion const &): 'z'")
    }

    // The bool operators...
    {
        float const w = rand<float>();
        float const x = rand<float>(), y = rand<float>(), z = rand<float>();
        Quaternion const q1(w, x, y, z), q2(w, x, y, z);

        CHECK(q1 == q2, "operator== (Quaternion const &) const")
        CHECK(!(q1 != q2), "operator!= (Quaternion const &) const")
    }

    // The sum and sub operators...
    {
        float const w1 = rand<float>(), w2 = rand<float>();
        float const x1 = rand<float>(), y1 = rand<float>(), z1 = rand<float>();
        float const x2 = rand<float>(), y2 = rand<float>(), z2 = rand<float>();

        Quaternion const q1(w1, x1, y1, z1), q2(w2, x2, y2, z2);
        Quaternion q3;

        q3 = q1;
        q3 += q2;

        CHECK_EQUAL(w1 + w2, q3.w(), "operator+= (Quaternion const &): 'w'")
        CHECK_EQUAL(x1 + x2, q3.x(), "operator+= (Quaternion const &): 'x'")
        CHECK_EQUAL(y1 + y2, q3.y(), "operator+= (Quaternion const &): 'y'")
        CHECK_EQUAL(z1 + z2, q3.z(), "operator+= (Quaternion const &): 'z'")

        q3 = q1;
        q3 -= q2;

        CHECK_EQUAL(w1 - w2, q3.w(), "operator-= (Quaternion const &): 'w'")
        CHECK_EQUAL(x1 - x2, q3.x(), "operator-= (Quaternion const &): 'x'")
        CHECK_EQUAL(y1 - y2, q3.y(), "operator-= (Quaternion const &): 'y'")
        CHECK_EQUAL(z1 - z2, q3.z(), "operator-= (Quaternion const &): 'z'")
    }

    // The cross and dot product operators...
    {
    }

    // The sum, sub, mult and diq with scalar operators...
    {
        Quaternion const q1(rand<float>(), rand<float>(), rand<float>(), rand<float>());
        float const s = rand<float>();

        Quaternion q2;

        q2 = q1;
        q2 += s;

        CHECK_EQUAL(q1.w() + s, q2.w(), "operator+= (float): 'w'")
        CHECK_EQUAL(q1.x() + s, q2.x(), "operator+= (float): 'x'")
        CHECK_EQUAL(q1.y() + s, q2.y(), "operator+= (float): 'y'")
        CHECK_EQUAL(q1.z() + s, q2.z(), "operator+= (float): 'z'")

        q2 = q1;
        q2 -= s;

        CHECK_EQUAL(q1.w() - s, q2.w(), "operator-= (float): 'w'")
        CHECK_EQUAL(q1.x() - s, q2.x(), "operator-= (float): 'x'")
        CHECK_EQUAL(q1.y() - s, q2.y(), "operator-= (float): 'y'")
        CHECK_EQUAL(q1.z() - s, q2.z(), "operator-= (float): 'z'")

        q2 = q1;
        q2 *= s;

        CHECK_EQUAL(q1.w() * s, q2.w(), "operator*= (float): 'w'")
        CHECK_EQUAL(q1.x() * s, q2.x(), "operator*= (float): 'x'")
        CHECK_EQUAL(q1.y() * s, q2.y(), "operator*= (float): 'y'")
        CHECK_EQUAL(q1.z() * s, q2.z(), "operator*= (float): 'z'")

        q2 = q1;
        q2 /= s;

        CHECK_EQUAL(q1.w() / s, q2.w(), "operator/= (float): 'w'")
        CHECK_EQUAL(q1.x() / s, q2.x(), "operator/= (float): 'x'")
        CHECK_EQUAL(q1.y() / s, q2.y(), "operator/= (float): 'y'")
        CHECK_EQUAL(q1.z() / s, q2.z(), "operator/= (float): 'z'")
    }

    // The positiqe and negatiqe operators...
    {
        float const w = rand<float>();
        float const x = rand<float>(), y = rand<float>(), z = rand<float>();
        Quaternion const q1(w, x, y, z), q2(-w, -x, -y, -z);

        CHECK_EQUAL(+q1.w(), (-q2).w(), "operator-(+) () const: 'w'")
        CHECK_EQUAL(+q1.x(), (-q2).x(), "operator-(+) () const: 'x'")
        CHECK_EQUAL(+q1.y(), (-q2).y(), "operator-(+) () const: 'y'")
        CHECK_EQUAL(+q1.z(), (-q2).z(), "operator-(+) () const: 'z'")
    }

    // The quaternion routines...
    {
        float const w = -1.0f, x = 1.0f, y = -1.0f, z = 1.0f;
        Quaternion q0(1.0f, 0.0f, 0.0f, 0.0f), q1(w, x, y, z), q2(w, -x, -y, -z);

        CHECK_EQUAL(4.0f, q1.GetNorm(), "GetNorm() const")
        CHECK_EQUAL(sqrtf(q1.GetNorm()), q1.GetLenght(), "GetLenght() const")

        CHECK_EQUAL(q1.Conjugate().w(), -w, "Conjugate()")
        CHECK(q0 == (q1 ^ q2.Inverse()), "Inverse()")

        CHECK_EQUAL(1.0f, q1.Normalize().GetLenght(), "Normalize()")
        CHECK_EQUAL(1.0f, Quaternion::GetNormalized(q1.q()).GetLenght(),
                    "Normalize(Quaternion const &)")
        CHECK_EQUAL(1.0f, Quaternion::GetNormalized(w, x, y, z).GetLenght(),
                    "Normalize(float, float, float, float)")
    }
}