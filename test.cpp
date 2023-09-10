///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 1996, 2022 - 2023.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#define WIN_TEXT_SIMPLE_USE_DEFAULTS

#if !defined(WIN_TEXT_SIMPLE_USE_DEFAULTS)
#include <resource.h>
#endif

#include <win_text_simple.h>

#if !defined(WIN_TEXT_SIMPLE_USE_DEFAULTS)
namespace local
{
  const char MyWindowTitle[] = "My Window Title";
}
#endif

auto WINAPI WinMain(HINSTANCE h_wnd, ::HINSTANCE, LPSTR, int) -> int
{
  #if defined(WIN_TEXT_SIMPLE_USE_DEFAULTS)
  using local_window_type = win_text_simple<>;
  #else
  using local_window_type = win_text_simple<local::MyWindowTitle, IDI_CIRCLE_CYAN>;
  #endif

  const auto result_win_main = local_window_type::win_main(h_wnd);

  return result_win_main;
}
