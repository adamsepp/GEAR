#pragma once
#include <imgui.h>

// ImVec2 operator overloads
inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 operator*(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x * b.x, a.y * b.y); }
inline ImVec2 operator*(const ImVec2& a, float b)         { return ImVec2(a.x * b, a.y * b); }
inline ImVec2 operator*(float b, const ImVec2& a)         { return ImVec2(a.x * b, a.y * b); }
inline ImVec2 operator/(const ImVec2& a, float b)         { return ImVec2(a.x / b, a.y / b); }

inline bool operator==(const ImVec2& a, const ImVec2& b)  { return a.x == b.x && a.y == b.y; }
inline bool operator!=(const ImVec2& a, const ImVec2& b)  { return !(a == b); }

// Optional: Min/Max helpers
inline ImVec2 ImMin(const ImVec2& a, const ImVec2& b)     { return ImVec2((a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y); }
inline ImVec2 ImMax(const ImVec2& a, const ImVec2& b)     { return ImVec2((a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y); }
