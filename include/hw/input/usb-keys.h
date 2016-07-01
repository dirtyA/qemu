/*
 * QEMU USB HID Emulator
 *
 * Copyright (c) 2016 John Arbuckle
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 *  File: usb-keys.h
 *  Description: Creates an enum of all the USB keycodes.
 *  Additional information: http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
 *                          page 53
 */

#ifndef USB_KEYS_H
#define USB_KEYS_H

enum {
    USB_HID_A = 0x04,
    USB_HID_B = 0x05,
    USB_HID_C = 0x06,
    USB_HID_D = 0x07,
    USB_HID_E = 0x08,
    USB_HID_F = 0x09,
    USB_HID_G = 0x0a,
    USB_HID_H = 0x0b,
    USB_HID_I = 0x0c,
    USB_HID_J = 0x0d,
    USB_HID_K = 0x0e,
    USB_HID_L = 0x0f,
    USB_HID_M = 0x10,
    USB_HID_N = 0x11,
    USB_HID_O = 0x12,
    USB_HID_P = 0x13,
    USB_HID_Q = 0x14,
    USB_HID_R = 0x15,
    USB_HID_S = 0x16,
    USB_HID_T = 0x17,
    USB_HID_U = 0x18,
    USB_HID_V = 0x19,
    USB_HID_W = 0x1a,
    USB_HID_X = 0x1b,
    USB_HID_Y = 0x1c,
    USB_HID_Z = 0x1d,

    USB_HID_1 = 0x1e,
    USB_HID_2 = 0x1f,
    USB_HID_3 = 0x20,
    USB_HID_4 = 0x21,
    USB_HID_5 = 0x22,
    USB_HID_6 = 0x23,
    USB_HID_7 = 0x24,
    USB_HID_8 = 0x25,
    USB_HID_9 = 0x26,
    USB_HID_0 = 0x27,

    USB_HID_RETURN = 0x28,
    USB_HID_ESC = 0x29,
    USB_HID_DELETE = 0x2a,
    USB_HID_TAB = 0x2b,
    USB_HID_SPACE = 0x2c,
    USB_HID_MINUS = 0x2d,
    USB_HID_EQUALS = 0x2e,
    USB_HID_LEFT_BRACKET = 0x2f,
    USB_HID_RIGHT_BRACKET = 0x30,
    USB_HID_BACKSLASH = 0x31,
    USB_HID_NON_US_NUMBER_SIGN = 0x32,
    USB_HID_SEMICOLON = 0x33,
    USB_HID_QUOTE = 0x34,
    USB_HID_GRAVE_ACCENT = 0x35,
    USB_HID_COMMA = 0x36,
    USB_HID_PERIOD = 0x37,
    USB_HID_FORWARD_SLASH = 0x38,
    USB_HID_CAPS_LOCK = 0x39,

    USB_HID_F1 = 0x3a,
    USB_HID_F2 = 0x3b,
    USB_HID_F3 = 0x3c,
    USB_HID_F4 = 0x3d,
    USB_HID_F5 = 0x3e,
    USB_HID_F6 = 0x3f,
    USB_HID_F7 = 0x40,
    USB_HID_F8 = 0x41,
    USB_HID_F9 = 0x42,
    USB_HID_F10 = 0x43,
    USB_HID_F11 = 0x44,
    USB_HID_F12 = 0x45,
    USB_HID_PRINT = 0x46,
    USB_HID_SCROLL_LOCK = 0x47,
    USB_HID_PAUSE = 0x48,

    USB_HID_INSERT = 0x49,
    USB_HID_HOME = 0x4a,
    USB_HID_PAGE_UP = 0x4b,
    USB_HID_FORWARD_DELETE = 0x4c,
    USB_HID_END = 0x4d,
    USB_HID_PAGE_DOWN = 0x4e,
    USB_HID_RIGHT_ARROW = 0x4f,
    USB_HID_LEFT_ARROW = 0x50,
    USB_HID_DOWN_ARROW = 0x51,
    USB_HID_UP_ARROW = 0x52,

    USB_HID_CLEAR = 0x53,
    USB_HID_KP_DIVIDE = 0x54,
    USB_HID_KP_MULTIPLY = 0x55,
    USB_HID_KP_MINUS = 0x56,
    USB_HID_KP_ADD = 0x57,
    USB_HID_KP_ENTER = 0x58,
    USB_HID_KP_1 = 0x59,
    USB_HID_KP_2 = 0x5a,
    USB_HID_KP_3  = 0x5b,
    USB_HID_KP_4 = 0x5c,
    USB_HID_KP_5 = 0x5d,
    USB_HID_KP_6 = 0x5e,
    USB_HID_KP_7 = 0x5f,
    USB_HID_KP_8 = 0x60,
    USB_HID_KP_9 = 0x61,
    USB_HID_KP_0 = 0x62,
    USB_HID_KP_PERIOD = 0x63,

    USB_HID_NON_US_BACKSLASH = 0x64,
    USB_HID_APPLICATION = 0x65,
    USB_HID_POWER = 0x66,
    USB_HID_KP_EQUALS = 0x67,
    USB_HID_F13 = 0x68,
    USB_HID_F14 = 0x69,
    USB_HID_F15 = 0x6a,
    USB_HID_EXECUTE = 0x74,
    USB_HID_HELP = 0x75,
    USB_HID_MENU = 0x76,
    USB_HID_SELECT = 0x77,
    USB_HID_STOP = 0x78,
    USB_HID_AGAIN = 0x79,
    USB_HID_UNDO = 0x7a,
    USB_HID_CUT = 0x7b,
    USB_HID_COPY = 0x7c,
    USB_HID_PASTE = 0x7d,
    USB_HID_FIND = 0x7e,
    USB_HID_MUTE = 0x7f,
    USB_HID_VOLUME_UP = 0x80,
    USB_HID_VOLUME_DOWN = 0x81,
    USB_HID_KP_COMMA = 0x85,

    USB_HID_LEFT_CONTROL = 0xe0,
    USB_HID_LEFT_SHIFT = 0xe1,
    USB_HID_LEFT_OPTION = 0xe2,
    USB_HID_LEFT_GUI = 0xe3,
    USB_HID_RIGHT_CONTROL = 0xe4,
    USB_HID_RIGHT_SHIFT = 0xe5,
    USB_HID_RIGHT_OPTION = 0xe6,
    USB_HID_RIGHT_GUI = 0xe7,
};

#endif /* USB_KEYS_H */
