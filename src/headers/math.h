#ifndef MATH_H
#define MATH_H

#include "includes.h"

constexpr float pi = 3.141519265358979323846;

inline constexpr float deg2rad(float angle)
{
    return (angle / 180.f) * pi;
}

inline constexpr float rad2deg(float angle)
{
    return (angle / pi) * 180.f;
}

template <typename T, std::size_t size> struct Vector
{
    static_assert(std::is_arithmetic<T>::value);
    T data[size];
    inline constexpr Vector& operator=(const Vector<T, size>& lhs) = default;
    inline constexpr Vector(const Vector<T, size>& lhs) = default;
    inline constexpr Vector(const T& lhs = T(0))
    {
        for(std::size_t i = 0; i < size; i++)
            data[i] = lhs;
    }
    inline constexpr Vector(const T(&lhs)[size])
    {
        for(std::size_t i = 0; i < size; i++)
            data[i] = lhs[i];
    }
    template <std::size_t S = size, typename... Args> 
    inline constexpr Vector(
        const T& lhs, 
        const Args&... args,
        typename std::enable_if<(std::is_convertible<Args, T>::value && ...) && sizeof...(Args) == S - 1>::type* = 0) : data{lhs, args...}
    {
        return;
    }
    template <std::size_t N, std::size_t S = size, typename... Args>
    inline constexpr Vector(
        const Vector<T, N>& lhs, 
        const Args&... args,
        typename std::enable_if<(std::is_convertible<Args, T>::value && ...) && sizeof...(Args) + N == size>::type* = 0)
    {
        const T values[] = {args...};
        for(std::size_t i = 0; i < size; i++)
            data[i] = i < N ? lhs.data[i] : values[i];
    }
    inline constexpr T mag2() const
    {
        T res = T(0);
        for(std::size_t i = 0; i < size; i++)
            res += data[i] * data[i];
        return res;
    }
    inline constexpr T mag() const
    {
        return std::sqrt(mag2());
    }
    inline constexpr T min_element() const
    {
        T res = data[0];
        for(std::size_t i = 0; i < size; i++)
            res = std::min(res, data[i]);
        return res;
    }
    inline constexpr T max_element() const
    {
        T res = data[0];
        for(std::size_t i = 0; i < size; i++)
            res = std::max(res, data[i]);
        return res;
    }
    inline constexpr Vector<T, size> abs() const
    {
        Vector<T, size> res;
        for(std::size_t i = 0; i < size; i++)
            res.data[i] = std::abs(data[i]);
        return res;
    }
    inline constexpr Vector<T, size> norm() const
    {
        Vector<T, size> res;
        const T mag = this->mag();
        for(std::size_t i = 0; i < size; i++)
            res.data[i] = data[i] / mag;
        return res;
    }
    template <typename F> inline constexpr operator Vector<F, size>()
    {
        Vector<F, size> res;
        for(std::size_t i = 0; i < size; i++)
            res.data[i] = static_cast<F>(data[i]);
        return res;
    }
    inline T& operator[](const std::size_t& index)
    {
        return data[index];
    }
    inline const T operator[](const std::size_t& index) const
    {
        return data[index];
    }
};

template <typename T> struct Vector<T, 2>
{
    union
    {
        T data[2];
        struct { T x, y; };
        struct { T u, v; };
        struct { T s, t; };
        struct { T w, h; };
    };
    inline constexpr Vector(const T& lhs = T(0)) : x(lhs), y(lhs) {}
    inline constexpr Vector(const T& x, const T& y) : x(x), y(y) {}
    inline constexpr T mag2() const
    {
        return x * x + y * y;
    }
    inline constexpr T mag() const
    {
        return std::sqrt(mag2());
    }
    inline constexpr T min_element() const
    {
        return std::min(x, y);
    }
    inline constexpr T max_element() const
    {
        return std::max(x, y);
    }
    inline constexpr Vector<T, 2> abs() const
    {
        return {std::abs(x), std::abs(y)};
    }
    inline constexpr Vector<T, 2> norm() const
    {
        const T mag = this->mag();
        return {x / mag, y / mag};
    }
    template <typename F> inline constexpr operator Vector<F, 2>()
    {
        return {
            static_cast<F>(x), 
            static_cast<F>(y)
        };
    }
    inline T& operator[](const std::size_t& index)
    {
        return data[index];
    }
    inline const T operator[](const std::size_t& index) const
    {
        return data[index];
    }
};

template <typename T> struct Vector<T, 3>
{
    union 
    {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T h, s, v; };
        struct 
        { 
            T roll;
            T pitch;
            T yaw; 
        };
    };
    inline constexpr Vector(const T& lhs = T(0)) : x(lhs), y(lhs), z(lhs) {}
    inline constexpr Vector(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}
    inline constexpr T mag2() const
    {
        return x * x + y * y + z * z;
    }
    inline constexpr T mag() const
    {
        return std::sqrt(mag2());
    }
    inline constexpr T min_element() const
    {
        return std::min({x, y, z});
    }
    inline constexpr T max_element() const
    {
        return std::max({x, y, z});
    }
    inline constexpr Vector<T, 3> abs() const
    {
        return {std::abs(x), std::abs(y), std::abs(z)};
    }
    inline constexpr Vector<T, 3> norm() const
    {
        const T mag = this->mag();
        return {x / mag, y / mag, z / mag};
    }
    template <typename F> inline constexpr operator Vector<F, 3>()
    {
        return {
            static_cast<F>(x), 
            static_cast<F>(y),
            static_cast<F>(z)
        };
    }
    inline T& operator[](const std::size_t& index)
    {
        return data[index];
    }
    inline const T operator[](const std::size_t& index) const
    {
        return data[index];
    }
};

template <typename T> struct Vector<T, 4>
{
    union
    {
        T data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };
    inline constexpr Vector(const T& lhs = T(0)) : x(lhs), y(lhs), z(lhs), w(lhs) {}
    inline constexpr Vector(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {}
    inline constexpr T mag2() const
    {
        return x * x + y * y +  z * z + w * w;
    }
    inline constexpr T mag() const
    {
        return std::sqrt(mag2());
    }
    inline constexpr T min_element() const
    {
        return std::min({x, y, z, w});
    }
    inline constexpr T max_element() const
    {
        return std::max({x, y, z, w});
    }
    inline constexpr Vector<T, 4> abs() const
    {
        return {std::abs(x), std::abs(y), std::abs(z), std::abs(w)};
    }
    inline constexpr Vector<T, 4> norm() const
    {
        const T mag = this->mag();
        return {x / mag, y / mag, z / mag, w / mag};
    }
    template <typename F> inline constexpr operator Vector<F, 4>()
    {
        return {
            static_cast<F>(x), 
            static_cast<F>(y), 
            static_cast<F>(z), 
            static_cast<F>(w)
        };
    }
    inline T& operator[](const std::size_t& index)
    {
        return data[index];
    }
    inline const T operator[](const std::size_t& index) const
    {
        return data[index];
    }
};

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator*=(Vector<T, size>& lhs, const T& rhs) 
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] *= rhs;
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator/=(Vector<T, size>& lhs, const T& rhs) 
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] /= rhs;
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator+=(Vector<T, size>& lhs, const T& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] += rhs;
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-=(Vector<T, size>& lhs, const T& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] -= rhs;
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator+=(Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] += rhs.data[i];
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-=(Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] -= rhs.data[i];
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator*=(Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] *= rhs.data[i];
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator/=(Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    for(std::size_t i = 0; i < size; i++)
        lhs.data[i] /= rhs.data[i];
    return lhs;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-(const T& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res -= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator+(const T& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res += rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator*(const T& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res *= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator/(const T& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res /= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator*(const Vector<T, size>& lhs, const T& rhs)
{
    Vector<T, size> res = lhs;
    res *= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator/(const Vector<T, size>& lhs, const T& rhs)
{
    Vector<T, size> res = lhs;
    res /= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator+(const Vector<T, size>& lhs, const T& rhs)
{
    Vector<T, size> res = lhs;
    res += rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-(const Vector<T, size>& lhs, const T& rhs)
{
    Vector<T, size> res = lhs;
    res -= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator*(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res *= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator/(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res /= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator+(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res += rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res = lhs;
    res -= rhs;
    return res;
}

template <typename T, std::size_t size> inline constexpr bool operator==(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    bool res = true;
    for(std::size_t i = 0; i < size; i++)
        res = res && (lhs.data[i] == rhs.data[i]);
    return res;
}

template <typename T, std::size_t size> inline constexpr bool operator<(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    bool res = true;
    for(std::size_t i = 0; i < size; i++)
        res = res && (lhs.data[i] < rhs.data[i]);
    return res;
}

template <typename T, std::size_t size> inline constexpr bool operator>(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    bool res = true;
    for(std::size_t i = 0; i < size; i++)
        res = res && (lhs.data[i] > rhs.data[i]);
    return res;
}

template <typename T, std::size_t size> inline constexpr bool operator<(const Vector<T, size>& lhs, const T& rhs)
{
    return lhs < Vector<T, size>(rhs);
}

template <typename T, std::size_t size> inline constexpr bool operator>(const Vector<T, size>& lhs, const T& rhs)
{
    return lhs > Vector<T, size>(rhs);
}

template <typename T, std::size_t size> inline constexpr bool operator<(const T& lhs, const Vector<T, size>& rhs)
{
    return Vector<T, size>(lhs) < rhs;
}

template <typename T, std::size_t size> inline constexpr bool operator>(const T& lhs, const Vector<T, size>& rhs)
{
    return Vector<T, size>(lhs) > rhs;
}

template <typename T, std::size_t size> inline constexpr bool operator!=(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> operator-(const Vector<T, size>& lhs)
{
    Vector<T, size> res;
    for(std::size_t i = 0; i < size; i++)
        res.data[i] = -lhs.data[i];
    return res;
}

template <typename T, std::size_t size> inline constexpr T dot(const Vector<T, size>& lhs, const Vector<T, size>& rhs){
    T res = T(0);
    for(std::size_t i = 0; i < size; i++)
        res += lhs.data[i] * rhs.data[i];
    return res;
}

template <typename T> inline constexpr Vector<T, 3> cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
{
    return {
        lhs.data.y * rhs.data.z - lhs.data.z * rhs.data.y,
        lhs.data.z * rhs.data.x - lhs.data.x * rhs.data.z,
        lhs.data.x * rhs.data.y - lhs.data.y * rhs.data.x
    };
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> max(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res;
    for(std::size_t i = 0; i < size; i++)
        res.data[i] = std::max(lhs.data[i], rhs.data[i]);
    return res;
}

template <typename T, std::size_t size> inline constexpr Vector<T, size> min(const Vector<T, size>& lhs, const Vector<T, size>& rhs)
{
    Vector<T, size> res;
    for(std::size_t i = 0; i < size; i++)
        res.data[i] = std::min(lhs.data[i], rhs.data[i]);
    return res;
}

typedef Vector<float, 2> v2f;
typedef Vector<double, 2> v2d;
typedef Vector<int32_t, 2> v2i;
typedef Vector<uint32_t, 4> v2u;
typedef Vector<float, 3> v3f;
typedef Vector<double, 3> v3d;
typedef Vector<int32_t, 3> v3i;
typedef Vector<uint32_t, 4> v3u;
typedef Vector<float, 4> v4f;
typedef Vector<double, 4> v4d;
typedef Vector<int32_t, 4> v4i;
typedef Vector<uint32_t, 4> v4u;

template <typename T, std::size_t rows, std::size_t cols> struct Matrix
{
    static_assert(std::is_arithmetic<T>::value);
    union
    {
        T mat[rows][cols];
        T data[rows * cols];
    };
    inline constexpr Matrix& operator=(const Matrix<T, rows, cols>& lhs) = default;
    inline constexpr Matrix(const Matrix<T, rows, cols>& lhs) = default;
    template <std::size_t N = rows, std::size_t M = cols>
    inline constexpr Matrix(
        const T& lhs = T(0),
        typename std::enable_if<N == M>::type* = 0)
    {
        for(std::size_t i = 0; i < rows; i++)
            for(std::size_t j = 0; j < cols; j++)
                mat[i][j] = (i == j) ? lhs : T(0);
    }
    template <std::size_t N = rows, std::size_t M = cols>
    inline constexpr Matrix(
        const T& lhs = T(0),
        typename std::enable_if<N != M>::type* = 0)
    {
        for(std::size_t i = 0; i < rows * cols; i++)
            data[i] = lhs;
    }
    inline constexpr Matrix(const T(&lhs)[rows * cols])
    {
        for(std::size_t i = 0; i < rows * cols; i++)
            data[i] = lhs[i];
    }
    template <std::size_t S = rows * cols, typename... Args> 
    inline constexpr Matrix(
        const T& lhs, 
        const Args&... args,
        typename std::enable_if<(std::is_convertible<Args, T>::value && ...) && sizeof...(Args) = S - 1>::type* = 0) : data{lhs, args...}
    {
        return;
    }
    template <std::size_t N, std::size_t M, std::size_t R = rows, std::size_t C = cols> 
    inline constexpr Matrix(
        const Matrix<T, N, M>& lhs,
        typename std::enable_if<N < R && M < C>::type* = 0)
    {
        for(std::size_t i = 0; i < N; i++)
            for(std::size_t j = 0; j < M; j++)
                this->mat[i][j] = lhs.mat[i][j];
    }
    inline constexpr Vector<T, cols> row(const std::size_t& lhs) const
    {
        Vector<T, cols> res; 
        for(std::size_t i = 0; i < cols; i++) 
            res[i] = mat[lhs][i];
        return res;
    }
    inline constexpr Vector<T, rows> col(const std::size_t& lhs) const
    {
        Vector<T, rows> res; 
        for(std::size_t i = 0; i < rows; i++) 
            res[i] = mat[i][lhs]; 
        return res;
    }
    inline constexpr void set_row(const std::size_t& lhs, const Vector<T, cols>& rhs) 
    {
        for(std::size_t i = 0; i < cols; i++) 
            mat[lhs][i] = rhs[i];
    }
    inline constexpr void set_col(const std::size_t& lhs, const Vector<T, rows>& rhs)
    {
        for(std::size_t i = 0; i < rows; i++)
            mat[i][lhs] = rhs[i];
    }
    inline friend constexpr bool operator==(const Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs)
    {
        bool res = true;
        for(std::size_t i = 0; i < rows * cols; i++)
            res = res && (lhs.data[i] == rhs.data[i]);
        return res;
    }
    inline friend constexpr bool operator!=(const Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs)
    {
        return !(lhs == rhs);
    }
    inline constexpr Matrix<T, cols, rows> transpose() const
    {
        Matrix<T, cols, rows> res;
        for(std::size_t i = 0; i < cols; i++)
            res.set_row(i, col(i));
        return res;
    }
    template <std::size_t N = rows, std::size_t M = cols, typename = typename std::enable_if<N == M>::type>
    inline constexpr Matrix<T, rows, cols> inverse() const
    {
        Matrix<T, rows, cols> temp = *this;
        Matrix<T, rows, cols> res = Matrix<T, rows, cols>(T(1));
        for(std::size_t i = 0; i < rows; i++)
        {
            const T div = temp.mat[i][i];
            res.set_row(i, res.row(i) / div);
            temp.set_col(i, temp.col(i) / div);
            for(std::size_t j = 0; j < rows; j++)
            {
                if(i != j)
                {
                    const T mul = temp.mat[i][j];
                    res.set_row(j, res.row(j) - (res.row(i) * mul));
                    temp.set_col(j, temp.col(j) - (temp.col(i) * mul));
                }
            }
        }
        return res.transpose();
    }
    template <std::size_t N = rows, std::size_t M = cols, typename = typename std::enable_if<N == M>::type>
    inline constexpr Matrix<T, rows, cols> pow(const std::size_t& lhs) const
    {
        if(lhs == 0)
        {
            return Matrix<T, rows, cols>(T(1));
        }
        else if(lhs == 1)
        {
            return *this;
        }
        else 
        {
            Matrix<T, rows, cols> res = *this;
            for(std::size_t j = 1; j < lhs; j++)
                res = res * (*this);
            return res;
        }
    }
    template <std::size_t N = rows, std::size_t M = cols, typename = typename std::enable_if<N == M>::type>
    inline constexpr T determinant() const
    {
        T res = T(1);
        Matrix<T, rows, cols> temp = *this;
        for(std::size_t i = 0; i < rows; i++)
        {
            const T div = temp.mat[i][i];
            for(std::size_t j = i + 1; j < rows; j++)
            {
                const T mul = temp.mat[i][j];
                temp.set_col(j, temp.col(j) - (temp.col(i) / div * mul));
            }
        }
        for(std::size_t i = 0; i < rows; i++)
        {
            res *= temp.mat[i][i];
        }
        return res;
    }
    template <std::size_t N>
    inline friend constexpr Matrix<T, rows, N> operator*(const Matrix<T, rows, cols>& lhs, const Matrix<T, cols, N>& rhs)
    {
        Matrix<T, rows, N> res;
        for(std::size_t i = 0; i < N; i++)
            for(std::size_t j = 0; j < rows; j++)
                res.mat[j][i] = dot(lhs.row(j), rhs.col(i));
        return res;
    }
    inline friend constexpr Vector<T, rows> operator*(const Matrix<T, rows, cols>& lhs, const Vector<T, cols>& rhs)
    {
        Vector<T, rows> res;
        for(std::size_t i = 0; i < rows; i++)
            res.data[i] = dot(lhs.row(i), rhs);
        return res;
    }
    inline friend constexpr Vector<T, cols> operator*(const Vector<T, rows>& lhs, const Matrix<T, rows, cols>& rhs)
    {
        Vector<T, cols> res;
        for(std::size_t i = 0; i < cols; i++)
            res.data[i] = dot(lhs, rhs.col(i));
        return res;
    }
    template <typename F> inline constexpr operator Matrix<F, rows, cols>()
    {
        Matrix<F, rows, cols> res;
        for(std::size_t i = 0; i < rows * cols; i++)
            res.data[i] = static_cast<F>(data[i]);
        return res;
    }
};

template <typename T, std::size_t N> Matrix<T, N, N> mat_identity()
{
    return Matrix<T, N, N>(T(1));
}

template <typename T> inline constexpr Matrix<T, 4, 4> make_perspective_mat(const T& aspect, const T& fov, const T& near, const T& far)
{
    const T half_fov = std::tan(deg2rad(fov) / T(2));
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(0);
    res.mat[0][0] = T(1) / (aspect * half_fov);
    res.mat[1][1] = T(1) / (half_fov);
    res.mat[2][2] = (near + far) / (near - far);
    res.mat[3][2] = T(-1);
    res.mat[2][3] = (T(2) * far * near) / (near - far);
    return res;
}

template <typename T> inline constexpr Matrix<T, 4, 4> make_ortho_mat(const T& right, const T& left, const T& bottom, const T& top, const T& near, const T& far)
{
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    res.mat[0][0] = T(2) / (right - left);
    res.mat[1][1] = T(2) / (top - bottom);
    res.mat[2][2] = T(2) / (near - far);
    res.mat[0][3] = (right + left) / (left - right);
    res.mat[1][3] = (top + bottom) / (bottom - top);
    res.mat[2][3] = (far + near) / (near - far);
    return res;
}

template <typename T> inline constexpr Matrix<T, 4, 4> mat_look_at(const Vector<T, 3>& eye, const Vector<T, 3>& center, const Vector<T, 3>& up)
{
    Vector<T, 3> en = (center - eye).norm();
    Vector<T, 3> cn = up.norm();
    Vector<T, 3> un = cross(en, cn).norm();
    cn = cross(un, en);
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    res.mat[0][0] = un.x;
    res.mat[0][1] = un.y;
    res.mat[0][2] = un.z;
    res.mat[1][0] = cn.x;
    res.mat[1][1] = cn.y;
    res.mat[1][2] = cn.z;
    res.mat[2][0] = -en.x;
    res.mat[2][1] = -en.y;
    res.mat[2][2] = -en.z;
    res.mat[0][3] = -dot(un, eye);
    res.mat[1][3] = -dot(cn, eye);
    res.mat[2][3] = dot(en, eye);
    return res;
}

template <typename T> inline constexpr Matrix<T, 4, 4> rotate_mat(const T& angle, const Vector<T, 3>& axis)
{
    const T c = std::cos(angle);
    const T s = std::sin(angle);
    const Vector<T, 3> norm = axis.norm();
    const Vector<T, 3> vec = norm * (1 - c);
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    res.mat[0][0] = c + vec.x * norm.x;
    res.mat[1][0] = vec.x * norm.y + s * norm.z;
    res.mat[2][0] = vec.x * norm.z - s * norm.y;
    res.mat[0][1] = vec.y * norm.x - s * norm.z;
    res.mat[1][1] = c + vec.y * norm.y;
    res.mat[2][1] = vec.y * norm.z + s * norm.x;
    res.mat[0][2] = vec.z * norm.x + s * norm.y;
    res.mat[1][2] = vec.z * norm.y - s * norm.x;
    res.mat[2][2] = c + vec.z * norm.z;
    return res;
}

template <typename T> inline constexpr Matrix<T, 4, 4> translate_mat(const Vector<T, 3>& lhs)
{
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    res.mat[0][3] = lhs.x;
    res.mat[1][3] = lhs.y;
    res.mat[2][3] = lhs.z;
    return res;
}

template <typename T> inline constexpr Matrix<T, 4, 4> scale_mat(const Vector<T, 3>& lhs)
{
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    res.mat[0][0] = lhs.x;
    res.mat[1][1] = lhs.y;
    res.mat[2][2] = lhs.z;
    return res;
}

typedef Matrix<float, 2, 2> mat2x2f;
typedef Matrix<double, 2, 2> mat2x2d;
typedef Matrix<int32_t, 2, 2> mat2x2i;
typedef Matrix<uint32_t, 2, 2> mat2x2u;
typedef Matrix<float, 3, 3> mat3x3f;
typedef Matrix<double, 3, 3> mat3x3d;
typedef Matrix<int32_t, 3, 3> mat3x3i;
typedef Matrix<uint32_t, 3, 3> mat3x3u;
typedef Matrix<float, 4, 4> mat4x4f;
typedef Matrix<double, 4, 4> mat4x4d;
typedef Matrix<int32_t, 4, 4> mat4x4i;
typedef Matrix<uint32_t, 4, 4> mat4x4u;

template <typename T> struct Quaternion
{
    static_assert(std::is_arithmetic<T>::value);
    union
    {
        T data[4];
        struct { T scalar; Vector<T, 3> vec; };
    };
    inline constexpr Quaternion& operator=(const Quaternion<T>& lhs) = default;
    inline constexpr Quaternion(const Quaternion<T>& lhs) = default;
    inline constexpr Quaternion()
    {
        scalar = T(1);
        vec = T(0);
    }
    inline constexpr Quaternion(const T& lhs, const Vector<T, 3>& rhs)
    {
        scalar = lhs;
        vec = rhs;
    }
    inline constexpr Quaternion(const T& w, const T& x, const T& y, const T& z)
    {
        scalar = w;
        vec = Vector<T, 3>(x, y, z);
    }
    inline friend constexpr bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        return lhs.scalar == rhs.scalar && lhs.vec == rhs.vec;
    }
    inline friend constexpr bool operator!=(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        return !(lhs == rhs);
    }
    inline friend constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        Quaternion<T> res;
        res.scalar = lhs.scalar * rhs.scalar - lhs.vec.x * rhs.vec.x - lhs.vec.y * rhs.vec.y - lhs.vec.z * rhs.vec.z;
        res.vec.x = lhs.scalar * rhs.vec.x + lhs.vec.x * rhs.scalar + lhs.vec.y * rhs.vec.z - lhs.vec.z * rhs.vec.y;
        res.vec.y = lhs.scalar * rhs.vec.y - lhs.vec.x * rhs.vec.z + lhs.vec.y * rhs.scalar + lhs.vec.z * rhs.vec.x;
        res.vec.z = lhs.scalar * rhs.vec.z + lhs.vec.x * rhs.vec.y - lhs.vec.y * rhs.vec.x + lhs.vec.z * rhs.scalar;
        return res;
    }
    inline friend constexpr Quaternion<T> operator/(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        Quaternion<T> res;
        const T inv = T(1) / rhs.norm();
        res.scalar = (lhs.scalar * rhs.scalar + lhs.vec.x * rhs.vec.x + lhs.vec.y * rhs.vec.y + lhs.vec.z * rhs.vec.z) * inv;
        res.scalar = (lhs.scalar * rhs.vec.x - lhs.vec.x * rhs.scalar - lhs.vec.y * rhs.vec.z + lhs.vec.z * rhs.vec.y) * inv;
        res.scalar = (lhs.scalar * rhs.vec.y + lhs.vec.x * rhs.vec.z - lhs.vec.y * rhs.scalar - lhs.vec.z * rhs.vec.x) * inv;
        res.scalar = (lhs.scalar * rhs.vec.z - lhs.vec.x * rhs.vec.y + lhs.vec.y * rhs.vec.x - lhs.vec.z * rhs.scalar) * inv;
        return res;
    }
    inline friend constexpr Quaternion<T> operator+(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        Quaternion<T> res;
        res.scalar = lhs.scalar + rhs.scalar;
        res.vec = lhs.vec + rhs.vec;
        return res;
    }
    inline friend constexpr Quaternion<T> operator-(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
    {
        Quaternion<T> res;
        res.scalar = lhs.scalar - rhs.scalar;
        res.vec = lhs.vec - rhs.vec;
        return res;
    }
    inline friend constexpr Quaternion<T> operator/(const Quaternion<T>& lhs, const T& rhs)
    {
        return Quaternion<T>(lhs.scalar / rhs, lhs.vec / rhs);
    }
    inline friend constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const T& rhs)
    {
        return Quaternion<T>(lhs.scalar * rhs, lhs.vec * rhs);
    }
    inline friend constexpr Quaternion<T> operator+(const Quaternion<T>& lhs, const T& rhs)
    {
        return Quaternion<T>(lhs.scalar + rhs, lhs.vec + rhs);
    }
    inline friend constexpr Quaternion<T> operator-(const Quaternion<T>& lhs, const T& rhs)
    {
        return Quaternion<T>(lhs.scalar - rhs, lhs.vec - rhs);
    }
    inline constexpr Quaternion<T> conjugate() const
    {
        return Quaternion<T>(scalar, -vec);
    }
    inline constexpr Quaternion<T> inverse() const
    {
        const T mag = this->mag2();
        return Quaternion<T>(scalar / mag, -vec / mag);
    }
    inline constexpr T mag2() const
    {
        return vec.mag2() + scalar * scalar;
    }
    inline constexpr T norm() const
    {
        return std::sqrt(this->mag2());
    }
    inline constexpr Quaternion<T> normalize() const
    {
        const T mag = this->norm();
        return Quaternion<T>(scalar / mag, vec / mag);
    }
    inline const T operator[](const std::size_t& index) const
    {
        return data[index];
    }
    inline T& operator[](const std::size_t& index)
    {
        return data[index];
    }
};

template <typename T> inline constexpr Quaternion<T> quat_from_euler(const Vector<T, 3>& lhs)
{
    Quaternion<T> res;
    const Vector<T, 3> vec = lhs / T(2);
    const T cr = std::cos(vec.roll);
    const T sr = std::sin(vec.roll);
    const T cp = std::cos(vec.pitch);
    const T sp = std::sin(vec.pitch);
    const T cy = std::cos(vec.yaw);
    const T sy = std::sin(vec.yaw);
    res.scalar = cr * cp * cy + sr * sp * sy;
    res.vec.x = sr * cp * cy - cr * sp * sy;
    res.vec.y = cr * sp * cy + sr * cp * sy;
    res.vec.z = cr * cp * sy - sr * sp * cy;
    return res;
}

template <typename T> inline constexpr Quaternion<T> quat_from_axis(const Vector<T, 3>& axis, float angle)
{
    Quaternion<T> res;
    res.scalar = std::cos(angle / T(2));
    res.vec = std::sin(angle / T(2)) * axis;
    return res;
}

template <typename T> inline constexpr Vector<T, 3> quat_to_euler(const Quaternion<T>& lhs)
{
    return {
        std::atan2
        (
            T(2) * (lhs.scalar * lhs.vec.x + lhs.vec.y * lhs.vec.z),
            T(1) - T(2) * (lhs.vec.x * lhs.vec.x + lhs.vec.y * lhs.vec.y)
        ),
        T(2) * std::atan2
        (
            std::sqrt(T(1) + T(2) * (lhs.scalar * lhs.vec.y - lhs.vec.x * lhs.vec.z)),
            std::sqrt(T(1) - T(2) * (lhs.scalar * lhs.vec.y - lhs.vec.x * lhs.vec.z))
        ) - pi / T(2),
        std::atan2
        (
            T(2) * (lhs.scalar * lhs.vec.z + lhs.vec.x * lhs.vec.y),
            T(1) - T(2) * (lhs.vec.y * lhs.vec.y + lhs.vec.z * lhs.vec.z)
        )
    };
}

template <typename T> inline constexpr Matrix<T, 4, 4> mat_from_quat(const Quaternion<T>& lhs)
{
    Matrix<T, 4, 4> res = Matrix<T, 4, 4>(1);
    const T sx = lhs.vec.x * lhs.vec.x;
    const T sy = lhs.vec.y * lhs.vec.y;
    const T sz = lhs.vec.z * lhs.vec.z;
    const T sw = lhs.scalar * lhs.scalar;
    const T xy = lhs.vec.x * lhs.vec.y;
    const T zw = lhs.vec.z * lhs.scalar;
    const T xw = lhs.scalar * lhs.vec.x;
    const T yz = lhs.vec.z * lhs.vec.y;
    const T xz = lhs.vec.z * lhs.vec.x;
    const T yw = lhs.scalar * lhs.vec.y;
    const T inv = T(1) / (sx + sy + sz + sw);
    res.mat[0][0] = T(1) - T(2) * (sy + sz) * inv;
    res.mat[0][1] = T(2) * (xy - zw) * inv;
    res.mat[0][2] = T(2) * (xz + yw) * inv;
    res.mat[1][0] = T(2) * (xy + zw) * inv;
    res.mat[1][1] = T(1) - T(2) * (sx + sz) * inv;
    res.mat[1][2] = T(2) * (yz - xw) * inv;
    res.mat[2][0] = T(2) * (xz - yw) * inv;
    res.mat[2][1] = T(2) * (yz + xw) * inv;
    res.mat[2][2] = T(1) - T(2) * (sx + sy) * inv;
    return res;
}

template <typename T> inline constexpr Quaternion<T> quat_from_mat(const Matrix<T, 4, 4>& lhs)
{
    Quaternion<T> res;
    res.scalar = std::sqrt(std::max(T(0), T(1) + lhs.mat[0][0] + lhs.mat[1][1] + lhs.mat[2][2])) / T(2);
    res.vec.x = std::sqrt(std::max(T(0), T(1) + lhs.mat[0][0] - lhs.mat[1][1] - lhs.mat[2][2])) / T(2);
    res.vec.y = std::sqrt(std::max(T(0), T(1) - lhs.mat[0][0] + lhs.mat[1][1] - lhs.mat[2][2])) / T(2);
    res.vec.z = std::sqrt(std::max(T(0), T(1) - lhs.mat[0][0] - lhs.mat[1][1] + lhs.mat[2][2])) / T(2);
    res.vec.x = std::copysign(res.vec.x, lhs.mat[2][1] - lhs.mat[1][2]);
    res.vec.y = std::copysign(res.vec.y, lhs.mat[0][2] - lhs.mat[2][0]);
    res.vec.z = std::copysign(res.vec.z, lhs.mat[1][0] - lhs.mat[0][1]);
    return res;
}

template <typename T> inline constexpr T quat_to_axis(const Quaternion<T>& lhs, Vector<T, 3>& rhs)
{
    const T div = std::sqrt(T(1) - lhs.scalar * lhs.scalar);
    rhs /= div == T(0) ? Vector<T, 3>(1, 0, 0) : div;
    return T(2) * std::acos(lhs.scalar);
}

template <typename T> inline constexpr Vector<T, 3> quat_rotate(const Quaternion<T>& lhs, const Vector<T, 3>& rhs)
{
    Vector<T, 3> vec = cross(lhs.vec * T(2), rhs);
    return rhs + lhs.scalar * vec + cross(vec, lhs.vec);
}

template <typename T> inline constexpr Quaternion<T> quat_slerp(const Quaternion<T>& lhs, const Quaternion<T>& rhs, float frac)
{
    const T ch = lhs.scalar * rhs.scalar + dot(lhs.vec, rhs.vec);
    if(std::abs(ch) >= T(1))
    {
        return lhs;
    }
    const T ht = std::acos(ch);
    const T sh = std::sqrt(T(1) - ch * ch);
    if(std::abs(sh) == T(0))
    {
        return {
            lhs.scalar / T(2) + rhs.scalar / T(2),
            lhs.vec / T(2) + rhs.vec / T(2)
        };
    }
    const T ra = std::sin((1 - frac) * ht) / sh;
    const T rb = std::sin(frac * ht) / sh;
    return {
        lhs.scalar * ra + rhs.scalar * rb,
        lhs.vec * ra + rhs.vec * rb
    };
}

inline constexpr v2f rotate(float angle, v2f rhs)
{
    return {
        std::cos(angle) * rhs.x - std::sin(angle) * rhs.y,
        std::sin(angle) * rhs.x + std::cos(angle) * rhs.y
    };
}

template <typename T> inline T rand(const T& lhs, const T& rhs)
{
    return ((float)rand() / (float)RAND_MAX) * (rhs - lhs) + lhs;
}

template <typename T, std::size_t size> inline Vector<T, size> rand(const T& lhs, const T& rhs)
{
    Vector<T, size> res;
    for(std::size_t i = 0; i < size; i++)
        res.data[i] = rand(lhs, rhs);
    return res;
}

template <typename T, std::size_t N, std::size_t M> inline Matrix<T, N, M> rand(const T& lhs, const T& rhs)
{
    Matrix<T, N, M> res;
    for(std::size_t i = 0; i < N * M; i++)
        res.data[i] = rand(lhs, rhs);
    return res;
}

struct Transform
{
    mat3x3f transform;
    mat3x3f inverted;
    bool invertMatrix;
    inline Transform();
    inline void Rotate(float ang);
    inline void Scale(float sx, float sy);
    inline void Translate(float dx, float dy);
    inline void Forward(float x, float y, float& ox, float& oy);
    inline void Backward(float x, float y, float& ox, float& oy);
    inline void Reset();
    inline void Invert();
    ~Transform() {}
};

#endif

#ifdef MATH_H
#undef MATH_H

inline Transform::Transform()
{
    this->Reset();
}

inline void Transform::Rotate(float ang)
{
    mat3x3f rotate = mat3x3f(1.0f);
    rotate.mat[0][0] = std::cos(ang);
    rotate.mat[0][1] = std::sin(ang);
    rotate.mat[1][0] = -std::sin(ang);
    rotate.mat[1][1] = std::cos(ang);
    transform = transform * rotate;
    invertMatrix = true;
}

inline void Transform::Translate(float dx, float dy)
{
    mat3x3f translate = mat3x3f(1.0f);
    translate.mat[0][2] = dx;
    translate.mat[1][2] = dy;
    transform = transform * translate;
    invertMatrix = true;   
}

inline void Transform::Scale(float sx, float sy)
{
    mat3x3f scale = mat3x3f(1.0f);
    scale.mat[0][0] = sx;
    scale.mat[1][1] = sy;
    transform = transform * scale;
    invertMatrix = true;
}

inline void Transform::Reset()
{
    transform = mat3x3f(1.0f);
    inverted = mat3x3f(1.0f);
    invertMatrix = false;
}

inline void Transform::Forward(float x, float y, float& ox, float& oy)
{
    float oz;
    ox = transform.mat[0][0] * x + transform.mat[0][1] * y + transform.mat[0][2];
    oy = transform.mat[1][0] * x + transform.mat[1][1] * y + transform.mat[1][2];
    oz = transform.mat[2][0] * x + transform.mat[2][1] * y + transform.mat[2][2];
    ox /= (oz == 0 ? 1 : oz);
    oy /= (oz == 0 ? 1 : oz);
}

inline void Transform::Backward(float x, float y, float& ox, float& oy)
{
    float oz;
    ox = inverted.mat[0][0] * x + inverted.mat[0][1] * y + inverted.mat[0][2];
    oy = inverted.mat[1][0] * x + inverted.mat[1][1] * y + inverted.mat[1][2];
    oz = inverted.mat[2][0] * x + inverted.mat[2][1] * y + inverted.mat[2][2];
    ox /= (oz == 0 ? 1 : oz);
    oy /= (oz == 0 ? 1 : oz);
}

inline void Transform::Invert()
{
    if(invertMatrix)
    {
        inverted = transform.inverse();
        invertMatrix = false;
    }
}

#endif