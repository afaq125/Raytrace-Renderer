#pragma once

#define CUSTOM_OPERATOR_VECTOR(op) \
    auto result = *this; \
    const Size count = result.Count(); \
    for (Size i = 0; i < count; ++i) { result.m_data[i] op= rhs.m_data[i]; } \
    return result;

#define CUSTOM_OPERATOR_VECTOR_T(op) \
    auto result = *this; \
    const Size count = result.Count(); \
    for (Size i = 0; i < count; ++i) { result.m_data[i] op= rhs; } \
    return result;

#define CUSTOM_OPERATOR_VECTOR_EQUALS(op) \
    const Size count = Count(); \
    for (Size i = 0; i < count; ++i) { m_data[i] op= rhs.m_data[i]; } \
    return *this;

#define CUSTOM_OPERATOR_VECTOR_T_EQUALS(op) \
    const Size count = Count(); \
    for (Size i = 0; i < count; ++i) { m_data[i] op= rhs; } \
    return *this;

namespace Renderer
{
    namespace Math
    {
        template<typename T>
        class Matrix;

        template <typename T, Size S>
        class Vector
        {
        public:
            Vector();
            Vector(const T value);
            Vector(const T* data);
            Vector(std::vector<T> data);
            Vector(const std::initializer_list<T>& data);
            virtual ~Vector() {}

            Vector(const Vector &rhs) = default;
            Vector(Vector &&rhs) = default;
            Vector& operator=(const Vector& vector) = default;
            Vector& operator=(Vector&& vector) = default;

            // Accessors
            T& operator[] (Size i) { return m_data[i]; }
            const T& operator[] (Size i) const { return m_data[i]; }
            std::vector<T>& Data() { return m_data; }
            const std::vector<T>& Data() const { return m_data; }
            Size Count() const { return m_data.size(); }

            T Length() const;
            void Normalize();
            Vector Normalized() const;
            void Clamp(const Size index, const T a, const T b);
            void Clamp(const T a, const T b);
            T DotProduct(const Vector& v) const;
            T Distance(const Vector& v) const;
            void SetNaNsOrINFs(const T value, const bool setNaNs = true, bool setINFs = true);
            void Pow(const T exponent);

            static Vector Mix(const Vector& a, const Vector& b, const T &amount);
            static Vector Min(const Vector& a, const Vector& b);
            static Vector Max(const Vector& a, const Vector& b);

            // Size dependant functions.
            Vector MatrixMultiply(const Matrix<T>& matrix) const;
            Vector CrossProduct(const Vector& other) const;

            // Operators
            Vector operator* (const Vector& rhs) const { CUSTOM_OPERATOR_VECTOR(*) }
            Vector operator+ (const Vector& rhs) const { CUSTOM_OPERATOR_VECTOR(+) }
            Vector operator- (const Vector& rhs) const { CUSTOM_OPERATOR_VECTOR(-) }
            Vector operator/ (const Vector& rhs) const { CUSTOM_OPERATOR_VECTOR(/ ) }
            
            Vector& operator*= (const Vector& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(*) }
            Vector& operator+= (const Vector& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(+) }
            Vector& operator-= (const Vector& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(-) }
            Vector& operator/= (const Vector& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(/ ) }
            
            Vector operator* (const T& rhs) const { CUSTOM_OPERATOR_VECTOR_T(*) }
            Vector operator+ (const T& rhs) const { CUSTOM_OPERATOR_VECTOR_T(+) }
            Vector operator- (const T& rhs) const { CUSTOM_OPERATOR_VECTOR_T(-) }
            Vector operator/ (const T& rhs) const { CUSTOM_OPERATOR_VECTOR_T(/ ) }
            
            Vector& operator*= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(*) }
            Vector& operator+= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(+) }
            Vector& operator-= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(-) }
            Vector& operator/= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(/ ) }

        protected:
            std::vector<T> m_data;
        };

        using Vector2 = Vector<float, 2>;
        using Vector3 = Vector<float, 3>;

        const Vector3 X_AXIS = { 1.0, 0.0, 0.0 };
        const Vector3 Y_AXIS = { 0.0, 1.0, 0.0 };
        const Vector3 Z_AXIS = { 0.0, 0.0, 1.0 };
        const Vector3 X_MINUS_AXIS = { -1.0, 0.0, 0.0 };
        const Vector3 Y_MINUS_AXIS = { 0.0, -1.0, 0.0 };
        const Vector3 Z_MINUS_AXIS = { 0.0, 0.0, -1.0 };
    }
}