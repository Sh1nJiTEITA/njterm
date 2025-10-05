#include "nj_text_buffer.h"
#include <cassert>
#include <cstring>

namespace nj::buf {

// clang-format off
uint32_t PackColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (uint32_t(r) << 24)
	 | (uint32_t(g) << 16)
	 | (uint32_t(b) << 8) 
	 | (uint32_t(a));
};
// clang-format on

TextBuffer::TextBuffer(size_t sz) : cursor1d{0} { cells.resize(sz); }

size_t TextBuffer::Size() const noexcept { return cells.size(); }

Cell& TextBuffer::GetCellByIndex(size_t idx) {
    assert(cells.size() > idx && "Invalid text buffer index");
    return cells[idx];
}

Cell& TextBuffer::GetCellUnderCursor() {
    assert(cells.size() > cursor1d && "Invalid text buffer index");
    return cells[cursor1d];
}

void TextBuffer::SetCursor(size_t idx) {
    assert(cells.size() > idx && "Invalid text buffer index");
    cursor1d = idx;
}

size_t TextBuffer::SetCursor2D(size_t cols, size_t row, size_t col) {
    const size_t pos = col + row * cols;
    cursor1d = pos;
    return pos;
}

size_t TextBuffer::CalcRowsCount(size_t cols) const {
    const size_t sz = cells.size();
    const float ratio = sz / static_cast<float>(cols);
    return static_cast<size_t>(ratio);
}

size_t TextBuffer::CalcRowsResidualCount(size_t cols) const {
    const size_t sz = cells.size();
    const size_t rows = CalcRowsCount(cols);
    const size_t residual = sz - rows * cols;
    assert(residual >= 0 && "Invalid rows residual");
    return residual;
}

size_t TextBuffer::CalcColsCount(size_t rows) const {
    const size_t sz = cells.size();
    const float ratio = sz / static_cast<float>(rows);
    return static_cast<size_t>(ratio);
}

size_t TextBuffer::CalcColsResidualCount(size_t rows) const {
    const size_t sz = cells.size();
    const size_t cols = CalcColsCount(rows);
    const size_t residual = sz - rows * cols;
    assert(residual >= 0 && "Invalid cols residual");
    return residual;
}

size_t TextBuffer::GetCursor() const noexcept { return cursor1d; }

void TextBuffer::MapDataTo(void* data) const {
    memcpy(data, cells.data(), sizeof(Cell) * cells.size());
    return;
}

} // namespace nj::buf
