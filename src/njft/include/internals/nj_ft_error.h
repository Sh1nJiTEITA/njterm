#pragma once
#ifndef NJ_FT_ERROR_H
#define NJ_FT_ERROR_H

// clang-format off
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fmt/base.h>
#undef FTERRORS_H_
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, NULL } };

const struct
{
    int          errorCode;
    const char*  errorMessage;
} freetypeErrorMessages[] =

#include <freetype/fterrors.h>

// clang-format on
#endif
