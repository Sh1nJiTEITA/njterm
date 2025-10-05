#pragma once

#include <memory>
#ifndef NG_TEXT_BUFFER_H
#define NG_TEXT_BUFFER_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nj::buf {

using Attributes = uint32_t;
enum class Attribute : Attributes {
    Bold = 1 << 0,      //!< 0000'0001
    Underling = 1 << 1, //!< 0000'0010
    Cursive = 1 << 2    //!< 0000'0100
};

//! @struct Holds terminal single cell attributes
//! @note Struct must work with std140 standart so it
//! must be aligned as 16 bytes structure.
struct alignas(16) Cell {
    uint32_t character;    //!< Character code | 4 Bytes
    uint32_t fgColor;      //!< Packed color   | 4 Bytes
    uint32_t bgColor;      //!< Packed color   | 4 Bytes
    Attributes attributes; //!< Characted att  | 4 Bytes
};

//! Packs default 256-bit colors (rgba) to single 32-bit integer
uint32_t PackColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

struct TextBuffer {
    TextBuffer(size_t sz);

    size_t Size() const noexcept;
    Cell& GetCellByIndex(size_t idx);

    Cell& GetCellUnderCursor();
    //! Sets inner 1D cursor with idx
    //! @param idx Index to set
    //! @note Curosr idx must be less then \ref ::cells.size
    void SetCursor(size_t idx);

    //! Sets inner 1D cursro as if data will be represanted as 2D array
    //! @param cols Number of cols in 2D array
    //! @param row Row index to set to cursor
    //! @param col Column index to set to cursor
    //! @return Calculated 1D cursor position
    size_t SetCursor2D(size_t cols, size_t row, size_t col);

    //! Calculates rows count which will make strong 2D array with input
    //! number of cols
    //! @note If returned rows * input cols are not exectly == \ref Size
    //! then other cells are dropped
    //! @param cols Columns to calc rows
    size_t CalcRowsCount(size_t cols) const;

    //! Calculates number of residual cells if trying to reshaping 1D array to
    //! 2D with number of input cols
    //! @param cols Columns to calc residual cells for
    size_t CalcRowsResidualCount(size_t cols) const;

    //! Calculates rows count which will make strong 2D array with input
    //! number of cols
    //! @note If returned rows * input cols are not exectly == \ref Size
    //! then other cells are dropped
    //! @param rows Rows to calc columns
    size_t CalcColsCount(size_t rows) const;

    //! Calculates number of residual cells if trying to reshaping 1D array to
    //! 2D with number of input rows
    //! @param rows Rows to calc residual cells for
    size_t CalcColsResidualCount(size_t rows) const;

    size_t GetCursor() const noexcept;

    void FillWithRainbow();
    void MapDataTo(void* data) const;

private:
    size_t cursor1d;
    std::vector<Cell> cells;
};

using TextBufferH = std::shared_ptr<TextBuffer>;

}; // namespace nj::buf

#endif
