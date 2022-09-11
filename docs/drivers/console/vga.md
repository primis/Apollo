VGA Console
===========

Introduction
------------
The VGA console driver is a simple text mode driver that utilizes 80 column 
text mode found on VGA hardware. It supports hardware cursor movement, and
several ANSI Escape codes. Of most note is color.

Screen Appearance
------------------
When first initialized, The VGA screen is set with a White-on-Blue text mode,
80x50 text mode with an 8x8 font.

Two Drivers in One
------------------
As VGA is not only used for text mode, the VGA driver was split between 
framebuffer and console. The VGA framebuffer driver handles mode selection,
font switching, and hardware cursor movement. This console driver is only
responsible for text mode operations to the screen.
