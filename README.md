win_text_simple
==================

win_text_simple makes a simple Win32-API window with dynamic text.

## The Win32-API and ckormanyos/win_text_simple

In chapter 2 of [1], the author describes and provides a code listing
for a simple text-based window. The example uses what was then known as the
Windows 95 API, which would later become the Win32-API.

Microsoft(R) Visual Studio(R) products (even Community-Editions)
still support the Win32-API. This API is very conventient
and can still be used to program Windows(R)-based applications.

ckormanyos/win_text_simple generally follows the code supplied in
Fig. 2-1 _The_ _HELLOWWIN_ _Program_ of [1]. It does, however,
add additional timer and threading facilities to create a dynamic,
text-based Win32-API program.

The main assignment of the program is a non-trivial
functionality that dynamically prints the value of `std::chrono`'s
high-performance clock in the text field of the window.

The application is written in header-only, modern C++ and is compatible
with C++14, 17, 20, 23 and beyond.

## References

[1] Charles Petzold, _Programming_ _Windows(R)_ _95_,
Microsoft Press 1996.
