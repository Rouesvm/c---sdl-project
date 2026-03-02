#pragma once

#include <cmath>
#include <iostream>

struct Vector2f {
    float x, y;

    Vector2f():x(0.0f), y(0.0f) {}
    Vector2f(float p_x, float p_y):x(p_x), y(p_y) {}
    Vector2f(int p_x, int p_y):x(static_cast<float>(p_x)), y(static_cast<float>(p_y)) {}

    Vector2f operator- (const Vector2f& b) const {
        return Vector2f(x - b.x, y - b.y);
    }

    Vector2f operator+ (const Vector2f& b) const {
        return Vector2f(x + b.x, y + b.y);
    }

    Vector2f operator* (const Vector2f& b) const {
        return Vector2f(x * b.x, y * b.y);
    }

    Vector2f operator/ (const Vector2f& b) const {
        return Vector2f(x / b.x, y / b.y);
    }


    Vector2f operator* (float b) const {
        return Vector2f(x * b, y * b);
    }

    Vector2f operator/ (float b) const {
        return Vector2f(x / b, y / b);
    }

    Vector2f pow(const float& b) const {
        return Vector2f(std::pow(x, b), std::pow(y, b));
    }    
    
    Vector2f& operator+=(const Vector2f& b) {
        x += b.x;
        y += b.y;
        return *this;
    }

    Vector2f& operator-=(const Vector2f& b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    Vector2f& operator*=(const Vector2f& b) {
        x *= b.x;
        y *= b.y;
        return *this;
    }

    Vector2f& operator/=(const Vector2f& b) {
        x /= b.x;
        y /= b.y;
        return *this;
    }

    Vector2f& operator*=(float b) {
        x *= b;
        y *= b;
        return *this;
    }

    Vector2f& operator/=(float b) {
        x /= b;
        y /= b;
        return *this;
    }

    Vector2f& operator^= (float b) {
        x = std::pow(x, b);
        y = std::pow(y, b);
        return *this;
    }

    bool operator== (const Vector2f& b) const {
        return x == b.x && y == b.y;
    }
   
    bool operator!= (const Vector2f& b) const {
        return !(x == b.x && y == b.y);
    }

    Vector2f& floor() {
        x = std::floor(x);
        y = std::floor(y);
        return *this;
    }

    float magnitude() const {
        Vector2f squared = this->pow(2.0f);
        return std::sqrt(squared.x + squared.y);
    }

    float angle() const {
        return std::atan2(y, x);
    }
    
    void print() const {
        std::cout << x << ", " << y << '\n';
    }
};

struct Vector2i {
    int x = 0;
    int y = 0;

    constexpr Vector2i():x(0), y(0) {}
    constexpr Vector2i(int p_x, int p_y):x(p_x), y(p_y) {}

    static constexpr Vector2i zero() {
        return {};
    }

    bool equals(const Vector2i& b) const {
        return *this == b;
    }

    Vector2i operator- (const Vector2i& b) const {
        return Vector2i(x - b.x, y - b.y);
    }

    Vector2i operator+ (const Vector2i& b) const {
        return Vector2i(x + b.x, y + b.y);
    }

    Vector2i operator* (const Vector2i& b) const {
        return Vector2i(x * b.x, y * b.y);
    }

    Vector2i operator/ (const Vector2i& b) const {
        return Vector2i(x / b.x, y / b.y);
    }

    Vector2i operator* (int b) const {
        return Vector2i(x * b, y * b);
    }

    Vector2i operator/ (int b) const {
        return Vector2i(x / b, y / b);
    }

    Vector2i pow(const int& b) const {
        return Vector2i(std::pow(x, b), std::pow(y, b));
    }

    Vector2i operator% (int b) const {
        return Vector2i(x % b, y % b);
    }

    Vector2i& operator+=(const Vector2i& b) {
        x += b.x;
        y += b.y;
        return *this;
    }

    Vector2i& operator-=(const Vector2i& b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    Vector2i& operator*=(const Vector2i& b) {
        x *= b.x;
        y *= b.y;
        return *this;
    }

    Vector2i& operator/=(const Vector2i& b) {
        x /= b.x;
        y /= b.y;
        return *this;
    }

    Vector2i& operator*=(int b) {
        x *= b;
        y *= b;
        return *this;
    }

    Vector2i& operator/=(int b) {
        x /= b;
        y /= b;
        return *this;
    }

    Vector2i& operator^= (int b) {
        x = std::pow(x, b);
        y = std::pow(y, b);
        return *this;
    }

    bool operator< (const Vector2i& b) const {
        if (x != b.x) return x < b.x;
        return y < b.y;
    }

    bool operator== (const Vector2i& b) const {
        return x == b.x && y == b.y;
    }
   
    bool operator!= (const Vector2i& b) const {
        return !(x == b.x && y == b.y);
    }

    size_t hash() const noexcept {
        size_t h1 = std::hash<int>{}(x);
        size_t h2 = std::hash<int>{}(y);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }

    int magnitude() const {
        Vector2i squared = this->pow(2);
        return sqrt(squared.x + squared.y);
    }

    void print() const {
        std::cout << x << ", " << y << '\n';
    }
};

namespace std {
    template<>
    struct hash<Vector2i> {
        size_t operator()(const Vector2i& v) const noexcept {
            return v.hash();
        }
    };
}