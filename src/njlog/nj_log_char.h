#pragma once
#ifndef NJ_LOG_COLOR_H
#define NJ_LOG_COLOR_H

#include <cstdint>
#include <fmt/color.h>
#include <vector>

namespace nj::log {

constexpr inline fmt::rgb GrayScale(uint8_t color) {
    return fmt::rgb(color, color, color);
};

inline void PrintChar(const std::vector<uint8_t> &data, size_t rows,
                      size_t cols, bool isolate = false) {
    if (isolate)
        fmt::println("");
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            const size_t idx = j + i * cols;
            const fmt::rgb color = GrayScale(data[idx]);
            const fmt::text_style style = fmt::bg(color) | fmt::fg(color);
            fmt::print(style, "{}", 0);
        }
        fmt::println("");
    }
    if (isolate)
        fmt::println("");
}

}; // namespace nj::log

#endif
