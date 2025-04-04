/**
 *
 * MIT License
 *
 * Copyright (c) 2023 GavinNL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// This header provides templated functions for
// numerical widgets
//
//
#ifndef IMJSCHEMA_WIDGETS_T_H
#define IMJSCHEMA_WIDGETS_T_H

#include <imgui.h>
#include <type_traits>
#include <cstdint>
#include <limits>
#include <string>


namespace ImJSchema
{

/**
 * @brief toImGuiDataType
 * @return
 *
 * Given an arithmetic datatype, return the imgui type
 */
template<typename _T>
constexpr ImGuiDataType_ toImGuiDataType()
{
    using T = std::decay_t<_T>;
    static_assert( std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, "Must be an arithmetic type or string");

    if constexpr( std::is_same_v<T, float> )
    {
        return ImGuiDataType_Float;
    }
    else if constexpr( std::is_same_v<T, double> )
    {
        return ImGuiDataType_Double;
    }
    else if constexpr( std::is_same_v<T, bool> )
    {
        return ImGuiDataType_Bool;
    }
    else if constexpr( std::is_same_v<T, std::string> )
    {
        return ImGuiDataType_String;
    }
    else if constexpr( std::is_signed_v<T> )
    {
        switch( sizeof(T))
        {
            case sizeof(int8_t):  return ImGuiDataType_S8;
            case sizeof(int16_t): return ImGuiDataType_S16;
            case sizeof(int32_t): return ImGuiDataType_S32;
            case sizeof(int64_t): return ImGuiDataType_S64;
        }
    }
    else if constexpr( std::is_unsigned_v<T> )
    {
        switch( sizeof(T))
        {
            case sizeof(uint8_t):  return ImGuiDataType_U8;
            case sizeof(uint16_t): return ImGuiDataType_U16;
            case sizeof(uint32_t): return ImGuiDataType_U32;
            case sizeof(uint64_t): return ImGuiDataType_U64;
        }
    }
}

template<typename T>
bool Input_T(const char* label,
             T* v,
             T step = std::numeric_limits<T>::max(),
             T step_fast = std::numeric_limits<T>::max(),
             const char* format_=nullptr,
             ImGuiInputTextFlags flags = 0)
{
    auto _step     = step < std::numeric_limits<T>::max() ? &step : nullptr;
    auto _fastStep = step_fast < std::numeric_limits<T>::max() ? &step_fast : nullptr;

    static_assert( std::is_arithmetic_v<T>, "This function specifically requires arithmetic types (float, double, [u]int[8,16,32,64]");

    return ImGui::InputScalar(label, toImGuiDataType<T>(), static_cast<void*>(v), static_cast<void*>(_step), static_cast<void*>(_fastStep), format_, flags);
}


template<typename T>
bool Drag_T(const char* label,
            T* v,
            float v_speed = 1.0f,
            T _min = std::numeric_limits<T>::lowest(),
            T _max = std::numeric_limits<T>::max(),
            const char* format_=nullptr,
            ImGuiSliderFlags flags = 0)
{
    static_assert( std::is_arithmetic_v<T>, "This function specifically requires arithmetic types (float, double, [u]int[8,16,32,64]");
    return ImGui::DragScalar(label, toImGuiDataType<T>(), v, v_speed, &_min, &_max, format_,  flags);
}

template<typename T>
bool Slider_T(const char* label,
              T* v,
              T _min,
              T _max,
              const char* format_=nullptr,
              ImGuiSliderFlags flags = 0)
{
    static_assert( std::is_arithmetic_v<T>, "This function specifically requires arithmetic types (float, double, [u]int[8,16,32,64]");
    return ImGui::SliderScalar(label, toImGuiDataType<T>(), v, &_min, &_max, format_,  flags);
}

}

#endif
