/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 21th March 2012.
****    Description: Vector class unit-testing.
****
********************************************************************************************************************************/
#include "Math\CrusVector.h"

UNIT_SUITE_CLASS(isle::math::Vector)
{
    using xUnit::rand;

    // The class constructor testing...
    {
        std::array<float, 3> const vec{rand<float>(), rand<float>(), rand<float>()};

        Vector const v1(vec[0], vec[1], vec[2]), v2(vec), v3(v2);

        CHECK_EQUAL(vec[0], v1.x, "class member 'x'");
        CHECK_EQUAL(vec[1], v1.y, "class member 'y'");
        CHECK_EQUAL(vec[2], v1.z, "class member 'z'");

        CHECK_EQUAL(vec[0], v2.x, "class member 'x'");
        CHECK_EQUAL(vec[1], v2.y, "class member 'y'");
        CHECK_EQUAL(vec[2], v2.z, "class member 'z'");

        CHECK_EQUAL(v2.x, v3.x, "class member 'x'");
        CHECK_EQUAL(v2.y, v3.y, "class member 'y'");
        CHECK_EQUAL(v2.z, v3.z, "class member 'z'");

        Vector v4(std::move(vec));

        CHECK_EQUAL(v2.x, v4.x, "class member 'x'");
        CHECK_EQUAL(v2.y, v4.y, "class member 'y'");
        CHECK_EQUAL(v2.z, v4.z, "class member 'z'");

        Vector const v5(std::move(v4));

        CHECK_EQUAL(v2.x, v5.x, "class member 'x'");
        CHECK_EQUAL(v2.y, v5.y, "class member 'y'");
        CHECK_EQUAL(v2.z, v5.z, "class member 'z'");
    }

    // The sum and sub operators...
    {
        float const x1 = rand<float>(), y1 = rand<float>(), z1 = rand<float>();
        float const x2 = rand<float>(), y2 = rand<float>(), z2 = rand<float>();

        Vector const v1(x1, y1, z1), v2(x2, y2, z2);

        CHECK_EQUAL(x1 + x2, (v1 + v2).x, "operator+ (Vector const &) const: 'x'")
        CHECK_EQUAL(y1 + y2, (v1 + v2).y, "operator+ (Vector const &) const: 'y'")
        CHECK_EQUAL(z1 + z2, (v1 + v2).z, "operator+ (Vector const &) const: 'z'")

        CHECK_EQUAL(x1 - x2, (v1 - v2).x, "operator- (Vector const &) const: 'x'")
        CHECK_EQUAL(y1 - y2, (v1 - v2).y, "operator- (Vector const &) const: 'y'")
        CHECK_EQUAL(z1 - z2, (v1 - v2).z, "operator- (Vector const &) const: 'z'")
    }

    // The cross and dot product operators...
    {
        Vector const v1(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f, 0.0f);

        CHECK_EQUAL(+0.0f, (v1 ^ v2).x, "operator^ (Vector const &) const: 'x'")
        CHECK_EQUAL(+0.0f, (v1 ^ v2).y, "operator^ (Vector const &) const: 'y'")
        CHECK_EQUAL(-1.0f, (v1 ^ v2).z, "operator^ (Vector const &) const: 'z'")

        CHECK_EQUAL(+0.0f, v1 * v2, "operator* (Vector const &) const")
    }

    // The sum, sub, mult and div with scalar operators...
    {
        float const vx = rand<float>(), vy = rand<float>(), vz = rand<float>();
        float const s = rand<float>();

        Vector const v(vx, vy, vz);

        CHECK_EQUAL(vx + s, (v + s).x, "operator+ (float) const: 'x'")
        CHECK_EQUAL(vy + s, (v + s).y, "operator+ (float) const: 'y'")
        CHECK_EQUAL(vz + s, (v + s).z, "operator+ (float) const: 'z'")

        CHECK_EQUAL(vx - s, (v - s).x, "operator- (float) const: 'x'")
        CHECK_EQUAL(vy - s, (v - s).y, "operator- (float) const: 'y'")
        CHECK_EQUAL(vz - s, (v - s).z, "operator- (float) const: 'z'")

        CHECK_EQUAL(vx * s, (v * s).x, "operator* (float) const: 'x'")
        CHECK_EQUAL(vy * s, (v * s).y, "operator* (float) const: 'y'")
        CHECK_EQUAL(vz * s, (v * s).z, "operator* (float) const: 'z'")

        CHECK_EQUAL(vx / s, (v / s).x, "operator/ (float) const: 'x'")
        CHECK_EQUAL(vy / s, (v / s).y, "operator/ (float) const: 'y'")
        CHECK_EQUAL(vz / s, (v / s).z, "operator/ (float) const: 'z'")
    }

    // The assignment operators...
    {
        float const s = rand<float>();
        Vector v;

        CHECK_EQUAL(s, (v = s).x, "operator= (float): 'x'")
        CHECK_EQUAL(s, (v = s).y, "operator= (float): 'y'")
        CHECK_EQUAL(s, (v = s).z, "operator= (float): 'z'")

        Vector v1, v2(rand<float>(), rand<float>(), rand<float>());

        CHECK_EQUAL(v2.x, (v1 = v2).x, "operator= (Vector const &): 'x'")
        CHECK_EQUAL(v2.y, (v1 = v2).y, "operator= (Vector const &): 'y'")
        CHECK_EQUAL(v2.z, (v1 = v2).z, "operator= (Vector const &): 'z'")
    }

    // The bool operators...
    {
        float const rx = rand<float>(), ry = rand<float>(), rz = rand<float>();
        Vector const v1(rx, ry, rz), v2(rx, ry, rz);

        CHECK(v1 == v2, "operator== (Vector const &) const")
        CHECK(!(v1 != v2), "operator!= (Vector const &) const")
    }

    // The sum and sub operators...
    {
        float const x1 = rand<float>(), y1 = rand<float>(), z1 = rand<float>();
        float const x2 = rand<float>(), y2 = rand<float>(), z2 = rand<float>();

        Vector const v1(x1, y1, z1), v2(x2, y2, z2);
        Vector v3;

        v3 = v1;
        v3 += v2;

        CHECK_EQUAL(x1 + x2, v3.x, "operator+= (Vector const &): 'x'")
        CHECK_EQUAL(y1 + y2, v3.y, "operator+= (Vector const &): 'y'")
        CHECK_EQUAL(z1 + z2, v3.z, "operator+= (Vector const &): 'z'")

        v3 = v1;
        v3 -= v2;

        CHECK_EQUAL(x1 - x2, v3.x, "operator-= (Vector const &): 'x'")
        CHECK_EQUAL(y1 - y2, v3.y, "operator-= (Vector const &): 'y'")
        CHECK_EQUAL(z1 - z2, v3.z, "operator-= (Vector const &): 'z'")
    }

    // The cross and dot product operators...
    {
        Vector v1(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f, 0.0f);
        v1 ^= v2;

        CHECK_EQUAL(+0.0f, v1.x, "operator^= (Vector const &): 'x'")
        CHECK_EQUAL(+0.0f, v1.y, "operator^= (Vector const &): 'y'")
        CHECK_EQUAL(-1.0f, v1.z, "operator^= (Vector const &): 'z'")
    }

    // The sum, sub, mult and div with scalar operators...
    {
        Vector const v1(rand<float>(), rand<float>(), rand<float>());
        float const s = rand<float>();

        Vector v2;

        v2 = v1;
        v2 += s;

        CHECK_EQUAL(v1.x + s, v2.x, "operator+= (float): 'x'")
        CHECK_EQUAL(v1.y + s, v2.y, "operator+= (float): 'y'")
        CHECK_EQUAL(v1.z + s, v2.z, "operator+= (float): 'z'")

        v2 = v1;
        v2 -= s;

        CHECK_EQUAL(v1.x - s, v2.x, "operator-= (float): 'x'")
        CHECK_EQUAL(v1.y - s, v2.y, "operator-= (float): 'y'")
        CHECK_EQUAL(v1.z - s, v2.z, "operator-= (float): 'z'")

        v2 = v1;
        v2 *= s;

        CHECK_EQUAL(v1.x * s, v2.x, "operator*= (float): 'x'")
        CHECK_EQUAL(v1.y * s, v2.y, "operator*= (float): 'y'")
        CHECK_EQUAL(v1.z * s, v2.z, "operator*= (float): 'z'")

        v2 = v1;
        v2 /= s;

        CHECK_EQUAL(v1.x / s, v2.x, "operator/= (float): 'x'")
        CHECK_EQUAL(v1.y / s, v2.y, "operator/= (float): 'y'")
        CHECK_EQUAL(v1.z / s, v2.z, "operator/= (float): 'z'")
    }

    // The positive and negative operators...
    {
        float const vx = rand<float>(), vy = rand<float>(), vz = rand<float>();
        Vector const v1(vx, vy, vz), v2(-vx, -vy, -vz);

        CHECK_EQUAL(+v1.x, (-v2).x, "operator-(+) () const: 'x'")
        CHECK_EQUAL(+v1.y, (-v2).y, "operator-(+) () const: 'y'")
        CHECK_EQUAL(+v1.z, (-v2).z, "operator-(+) () const: 'z'")
    }

    // Increment and decrement operators...
    {
        float const vx = rand<float>(), vy = rand<float>(), vz = rand<float>();
        Vector v1(vx, vy, vz), v2(vx, vy, vz);

        CHECK_EQUAL(vx + 1.0f, (++v1).x, "operator++ (): 'x'")
        CHECK_EQUAL(vy + 2.0f, (++v1).y, "operator++ (): 'y'")
        CHECK_EQUAL(vz + 3.0f, (++v1).z, "operator++ (): 'z'")

        CHECK_EQUAL(vx - 1.0f, (--v2).x, "operator-- (): 'x'")
        CHECK_EQUAL(vy - 2.0f, (--v2).y, "operator-- (): 'y'")
        CHECK_EQUAL(vz - 3.0f, (--v2).z, "operator-- (): 'z'")

        CHECK_EQUAL(vx + 3.0f, (v1++).x, "operator++ (Vector &, int32): 'x'")
        CHECK_EQUAL(vy + 4.0f, (v1++).y, "operator++ (Vector &, int32): 'y'")
        CHECK_EQUAL(vz + 5.0f, (v1++).z, "operator++ (Vector &, int32): 'z'")

        CHECK_EQUAL(vx - 3.0f, (v2--).x, "operator-- (Vector &, int32): 'x'")
        CHECK_EQUAL(vy - 4.0f, (v2--).y, "operator-- (Vector &, int32): 'y'")
        CHECK_EQUAL(vz - 5.0f, (v2--).z, "operator-- (Vector &, int32): 'z'")
    }

    // The vector routines...
    {
        Vector v(1.0f, -1.0f, 1.0f);

        CHECK_EQUAL(1.0f + 1.0f + 1.0f, v.GetNorm(), "GetNorm() const");
        CHECK_EQUAL(sqrtf(v.GetNorm()), v.GetLenght(), "GetLenght() const");

        CHECK_EQUAL(1.0f, v.Normalize().GetLenght(), "Normalize()");
        CHECK_EQUAL(1.0f, Vector::GetNormalized(v).GetLenght(), "Normalize(Vector const &)");
        CHECK_EQUAL(1.0f, Vector::GetNormalized(1.0f, -1.0f, 1.0f).GetLenght(), "Normalize(float, float, float)");
    }
}