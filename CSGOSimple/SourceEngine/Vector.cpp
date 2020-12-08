// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Vector.hpp"
#include <limits>
#include <random>

namespace SourceEngine
{

    void VectorCopy(const Vector& src, Vector& dst)
    {
        dst.x = src.x;
        dst.y = src.y;
        dst.z = src.z;
    }
    void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest)
    {
        dest.x = src1.x + (src2.x - src1.x) * t;
        dest.y = src1.y + (src2.y - src1.y) * t;
        dest.z = src1.z + (src2.z - src1.z) * t;
    }
    float VectorLength(const Vector& v)
    {
        return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    }
    void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result)
    {
        result.x = a.y*b.z - a.z*b.y;
        result.y = a.z*b.x - a.x*b.z;
        result.z = a.x*b.y - a.y*b.x;
    }
    vec_t NormalizeVector(Vector& v)
    {
	    const auto l = v.Length();
        if(l != 0.0f) {
            v /= l;
        } else {
            v.x = v.y = v.z = 0.0f;
        }
        return l;
    }
    //-----------------------------------------------------------------------------
    // initialization
    //-----------------------------------------------------------------------------

    void Vector::Init(vec_t ix, vec_t iy, vec_t iz)
    {
        x = ix; y = iy; z = iz;
    }

    Vector Vector::Normalized() const
    {
        Vector res = *this;
	    const vec_t l = res.Length();
        if(l != 0.0f) {
            res /= l;
        } else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }
    vec_t Vector::NormalizeInPlace()
    {
        return NormalizeVector(*this);
    }

	float GetRandomFloat(const float& min, const float& max)
	{
		std::random_device Random;
		std::mt19937 RandomGenerator(Random());
		const std::uniform_real_distribution<float> random_distribute(min, max);
		return random_distribute(RandomGenerator);
	}

    void Vector::Random(vec_t minVal, vec_t maxVal)
    {
        x = GetRandomFloat(minVal, maxVal);
        y = GetRandomFloat(minVal, maxVal);
        z = GetRandomFloat(minVal, maxVal);
    }

    // This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
    void Vector::Zero()
    {
        x = y = z = 0.0f;
    }

    //-----------------------------------------------------------------------------
    // assignment
    //-----------------------------------------------------------------------------

    Vector& Vector::operator=(const Vector &vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }


    //-----------------------------------------------------------------------------
    // Array access
    //-----------------------------------------------------------------------------
    vec_t& Vector::operator[](size_t i)
    {
        return reinterpret_cast<vec_t*>(this)[i];
    }

    vec_t Vector::operator[](size_t i) const
    {
        return ((vec_t*)this)[i];
    }


    //-----------------------------------------------------------------------------
    // Base address...
    //-----------------------------------------------------------------------------
    vec_t* Vector::Base()
    {
        return reinterpret_cast<vec_t*>(this);
    }

    vec_t const* Vector::Base() const
    {
        return reinterpret_cast<vec_t const*>(this);
    }

    //-----------------------------------------------------------------------------
    // IsValid?
    //-----------------------------------------------------------------------------

    bool isinfinite(float x)
    {
        return x == std::numeric_limits<float>::infinity();
    }

    bool Vector::IsValid() const
    {
        return isinfinite(z) ? isinfinite(x) && isinfinite(y) : false;
    }

    //-----------------------------------------------------------------------------
    // Invalidate
    //-----------------------------------------------------------------------------

    void Vector::Invalidate()
    {
        //#ifdef _DEBUG
        //#ifdef VECTOR_PARANOIA
        x = y = z = std::numeric_limits<float>::infinity();
		//memset(this, std::numeric_limits<float>::infinity(), sizeof(Vector));
        //#endif
        //#endif
    }

    //-----------------------------------------------------------------------------
    // comparison
    //-----------------------------------------------------------------------------

    bool Vector::operator==(const Vector& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }

    bool Vector::operator!=(const Vector& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }


    //-----------------------------------------------------------------------------
    // Copy
    //-----------------------------------------------------------------------------
    void	Vector::CopyToArray(float* rgfl) const
    {
        rgfl[0] = x, rgfl[1] = y, rgfl[2] = z;
    }

    //-----------------------------------------------------------------------------
    // standard math operations
    //-----------------------------------------------------------------------------
    // #pragma message("TODO: these should be SSE")

    void Vector::Negate()
    {
        x = -x; y = -y; z = -z;
    }

    // get the component of this vector parallel to some other given vector
    Vector Vector::ProjectOnto(const Vector& onto) const
    {
        return onto * (this->Dot(onto) / (onto.LengthSqr()));
    }

    // FIXME: Remove
    // For backwards compatability
    void	Vector::MulAdd(const Vector& a, const Vector& b, float scalar)
    {
        x = a.x + b.x * scalar;
        y = a.y + b.y * scalar;
        z = a.z + b.z * scalar;
    }

    Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t)
    {
        Vector result;
        VectorLerp(src1, src2, t, result);
        return result;
    }

    vec_t Vector::Dot(const Vector& b) const
    {
        return (x*b.x + y*b.y + z*b.z);
    }
    void VectorClear(Vector& a)
    {
        a.x = a.y = a.z = 0.0f;
    }

    vec_t Vector::Length() const
    {
        return sqrtf(x*x + y*y + z*z);
    }

    // check a point against a box
    bool Vector::WithinAABox(Vector const &boxmin, Vector const &boxmax) const
    {
        return (
            (x >= boxmin.x) && (x <= boxmax.x) &&
            (y >= boxmin.y) && (y <= boxmax.y) &&
            (z >= boxmin.z) && (z <= boxmax.z)
            );
    }

    //-----------------------------------------------------------------------------
    // Get the distance from this vector to the other one 
    //-----------------------------------------------------------------------------
    vec_t Vector::DistTo(const Vector &vOther) const
    {
	    Vector delta = *this - vOther;
        return delta.Length();
    }

    //-----------------------------------------------------------------------------
    // Returns a vector with the min or max in X, Y, and Z.
    //-----------------------------------------------------------------------------
    Vector Vector::Min(const Vector &vOther) const
    {
        return Vector(x < vOther.x ? x : vOther.x,
            y < vOther.y ? y : vOther.y,
            z < vOther.z ? z : vOther.z);
    }

    Vector Vector::Max(const Vector &vOther) const
    {
        return Vector(x > vOther.x ? x : vOther.x,
            y > vOther.y ? y : vOther.y,
            z > vOther.z ? z : vOther.z);
    }


    //-----------------------------------------------------------------------------
    // arithmetic operations
    //-----------------------------------------------------------------------------

    Vector Vector::operator-() const
    {
        return Vector(-x, -y, -z);
    }

    Vector Vector::operator+(const Vector& v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    Vector Vector::operator-(const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    Vector Vector::operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }

    Vector Vector::operator*(const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }

    Vector Vector::operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }

    Vector Vector::operator/(const Vector& v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    Vector operator*(float fl, const Vector& v)
    {
        return v * fl;
    }

    //-----------------------------------------------------------------------------
    // cross product
    //-----------------------------------------------------------------------------

    Vector Vector::Cross(const Vector& vOther) const
    {
        Vector res;
        VectorCrossProduct(*this, vOther, res);
        return res;
    }
}
