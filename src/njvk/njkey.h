#pragma once
#ifndef NJ_KEY_H
#define NJ_KEY_H

#include <cstdint>

namespace nj {
namespace win {

using KeyType = uint32_t;

enum class Key : KeyType {
    CapsLock,
    Space,
    Tab,
    Enter,
    Escape,
    Backspace,
    ScrollLock,
    Delete,
    Insert,
    Home,
    End,
    PgUp,
    PgDn,
    Up,
    Down,
    Left,
    Right,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    LeftWin,
    RightWin,
    Ctrl,
    Alt,
    Shift,
    LeftCtrl,
    RightCtrl,
    LeftShift,
    RightShift,
    LeftAlt,
    RightAlt,
    Apps, // what is it? :)
    PrintScreen,
    Pause,
    Help,
    Sleep,
    MouseLeft,
    MouseRight,
    MouseMiddle,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    Slash,      // /
    BackSlash,  //
    SemiColon,  // ;
    Apostrophe, // '
    Period,     // .
    Comma,      // ,
    LeftBrace,  // [
    RightBrace  // ]
};

} // namespace win
} // namespace nj

#endif
