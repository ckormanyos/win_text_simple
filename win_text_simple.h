///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 1996 - 2023.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WIN_TEXT_SIMPLE_2022_08_05_H
  #define WIN_TEXT_SIMPLE_2022_08_05_H

  #include <chrono>
  #include <cstdint>
  #include <string>
  #include <thread>

  #include <windows.h>

  namespace win_text_simple_detail
  {
    const char WindowTitleDefault[] = "My Window Title";
  }

  template<const char* WindowTitle       = win_text_simple_detail::WindowTitleDefault,
           const int   IconId            = static_cast<int>(INT8_C(0)),
           const int   ScreenCoordinateX = static_cast<int>(UINT16_C(250)),
           const int   ScreenCoordinateY = static_cast<int>(UINT16_C(150)),
           const int   WindowWidth       = static_cast<int>(UINT16_C(330)),
           const int   WindowHeight      = static_cast<int>(UINT16_C(170))>
  class win_text_simple final
  {
  private:
    static constexpr auto screen_coordinate_x = static_cast<int>(ScreenCoordinateX); // Screen coordinate X
    static constexpr auto screen_coordinate_y = static_cast<int>(ScreenCoordinateY); // Screen coordinate Y
    static constexpr auto window_width        = static_cast<int>(WindowWidth);       // Window width
    static constexpr auto window_height       = static_cast<int>(WindowHeight);      // Window height

  public:
    win_text_simple() = default;

    win_text_simple(const win_text_simple&) = delete;
    win_text_simple(win_text_simple&&) noexcept = delete;

    ~win_text_simple() = default;

    auto operator=(const win_text_simple&) -> win_text_simple = delete;
    auto operator=(win_text_simple&&) noexcept -> win_text_simple = delete;

    static constexpr auto default_style() noexcept -> ::DWORD
    {
      return static_cast<::DWORD>(WS_MINIMIZEBOX | WS_CAPTION | WS_POPUP | WS_SYSMENU);
    }

    auto get_handle_to_window() const noexcept -> const HWND { return my_handle_to_window; }

    auto create_window(::HINSTANCE   hinst,                                                      // Handle to window instance
                       const ::DWORD basic_style    = default_style(),                           // Window basic style
                       const ::DWORD extended_style = static_cast<::DWORD>(UINT8_C(0))) -> bool  // Window extended style
    {
      my_handle_to_instance = hinst;

      using local_window_type = win_text_simple;

      {
        auto window_class = ::WNDCLASS { };

        window_class.style         =  static_cast<::UINT>(CS_HREDRAW | CS_VREDRAW | CS_OWNDC);
        window_class.lpfnWndProc   =  my_window_callback;
        window_class.cbClsExtra    =  static_cast<int>(INT8_C(0));
        window_class.cbWndExtra    =  static_cast<int>(INT8_C(0));
        window_class.hInstance     =  my_handle_to_instance;
        window_class.hIcon         =  ::LoadIcon(my_handle_to_instance, MAKEINTRESOURCE(local_window_type::icon_id()));
        window_class.hCursor       =  ::LoadCursor(nullptr, IDC_ARROW);
        window_class.hbrBackground =  reinterpret_cast<::HBRUSH>(COLOR_APPWORKSPACE);
        window_class.lpszMenuName  =  nullptr;
        window_class.lpszClassName =  local_window_type::window_title();

        const auto register_class_result = ::RegisterClass(&window_class);

        static_cast<void>(register_class_result);
      }

      my_handle_to_window = ::CreateWindowEx(extended_style,
                                             local_window_type::window_title(),
                                             local_window_type::window_title(),
                                             basic_style,
                                             screen_coordinate_x,
                                             screen_coordinate_y,
                                             window_width,
                                             window_height,
                                             nullptr,
                                             nullptr,
                                             my_handle_to_instance,
                                             nullptr);

      auto create_window_is_ok = (my_handle_to_window != nullptr);

      if(create_window_is_ok)
      {
        // Show the window.
        const auto redraw_window_is_ok =
          (::RedrawWindow(my_handle_to_window, nullptr, nullptr, static_cast<::UINT>(UINT8_C(0))) == TRUE);

        create_window_is_ok = (redraw_window_is_ok && create_window_is_ok);

        const auto show_window_result_is_ok =
          (::ShowWindow(my_handle_to_window, SW_SHOW) == TRUE);

        create_window_is_ok = (show_window_result_is_ok && create_window_is_ok);

        my_thread = std::thread(thread_process);

        // Detach this process from its spawning thread object.
        my_thread.detach();
      }

      const auto handle_to_active_window = ::GetActiveWindow();

      const auto create_window_result = (    create_window_is_ok
                                         && (handle_to_active_window != nullptr)
                                         && (my_handle_to_window == handle_to_active_window));

      return create_window_result;
    }

    static auto instance() -> win_text_simple&
    {
      static win_text_simple win_text_simple_instance;

      return win_text_simple_instance;
    }

    static auto win_main(::HINSTANCE hinst) -> int
    {
      // This implements the standard Win32 API main function.

      using local_window_type = win_text_simple;

      // Create the window.
      const bool create_window_is_ok = local_window_type::instance().create_window(hinst);

      static_cast<void>(create_window_is_ok);

      auto get_message_is_ok = true;

      // Enter the Windows message loop.
      while(get_message_is_ok)
      {
        auto message = ::MSG { };

        const auto peek_msg_result =
          ::PeekMessage(&message,
                        nullptr,
                        static_cast<::UINT>(UINT8_C(0)),
                        static_cast<::UINT>(UINT8_C(0)),
                        static_cast<::UINT>(PM_NOREMOVE));

        const auto peek_msg_has_message = (peek_msg_result == TRUE);

        if(!peek_msg_has_message)
        {
          ::Sleep(static_cast<::DWORD>(UINT8_C(50)));
        }
        else
        {
          ::Sleep(static_cast<::DWORD>(UINT8_C(0)));

          const auto get_message_result =
            ::GetMessage(&message, nullptr, static_cast<::UINT>(UINT8_C(0)), static_cast<::UINT>(UINT8_C(0)));

          get_message_is_ok = (get_message_result == TRUE);

          if(get_message_is_ok)
          {
            // Process Win32 API messages (via standard Windows message pump).

            const auto translate_message_is_ok = (::TranslateMessage(&message) == TRUE);
            const auto dispatch_message_is_ok  = (::DispatchMessage (&message) == TRUE);

            static_cast<void>(translate_message_is_ok);
            static_cast<void>(dispatch_message_is_ok);
          }
        }
      }

      return static_cast<int>(INT8_C(0));
    }

  private:
    ::HWND      my_handle_to_window   { nullptr };
    ::HINSTANCE my_handle_to_instance { nullptr };

    static std::thread     my_thread;
    static volatile ::LONG my_thread_wants_exit;
    static volatile ::LONG my_thread_did_exit;

    static constexpr auto window_title() noexcept -> const char* { return WindowTitle; }
    static constexpr auto icon_id() noexcept -> int { return IconId; }

    auto get_handle_to_instance() const noexcept -> const ::HINSTANCE { return my_handle_to_instance; }

    static auto CALLBACK my_window_callback(::HWND   handle_to_window,
                                            ::UINT   message,
                                            ::WPARAM w_param,
                                            ::LPARAM l_param) -> ::LRESULT
    {
      // This is the standard Win32 API callback function.

      if(message == static_cast<::UINT>(WM_PAINT))
      {
        auto ps = ::PAINTSTRUCT { };

        const auto hdc = ::BeginPaint(handle_to_window, &ps);

        auto rect = ::RECT { };

        ::GetClientRect(handle_to_window, &rect);

        // Obtain the value of std::chrono::high_resolution_clock::now().
        const auto local_now =
          std::chrono::duration_cast<std::chrono::nanoseconds>
          (
            std::chrono::high_resolution_clock::now().time_since_epoch()
          );

        const auto str =
          std::string
          (
              "Dynamic Text: "
            + std::to_string(static_cast<std::uintmax_t>(local_now.count()))
          );

        const auto result_draw =
          ::DrawText(hdc, str.c_str(), static_cast<int>(INT8_C(-1)), &rect, static_cast<::UINT>(DT_SINGLELINE | DT_CENTER | DT_VCENTER));

        const auto result_draw_is_ok = result_draw != static_cast<int>(INT8_C(0));

        const auto result_end_paint_is_ok = (::EndPaint(handle_to_window, &ps) == TRUE);

        const auto result_is_ok = (result_draw_is_ok && result_end_paint_is_ok);

        const auto lresult =
          static_cast<::LRESULT>
          (
            result_is_ok ? static_cast<::LRESULT>(INT8_C(0)) : static_cast<::LRESULT>(INT8_C(-1))
          );

        return lresult;
      }

      if(message == static_cast<::UINT>(WM_DESTROY))
      {
        // Exit the process thread.
        static_cast<void>(::InterlockedExchange(&my_thread_wants_exit, static_cast<::LONG>(INT8_C(1))));

        for(auto   i = static_cast<unsigned>(UINT8_C(0));
                   i < static_cast<unsigned>(UINT8_C(200));
                 ++i)
        {
          if(my_thread_did_exit == static_cast<::LONG>(INT8_C(1)))
          {
            break;
          }

          ::Sleep(static_cast<::DWORD>(UINT8_C(5)));
        }

        using local_window_type = win_text_simple;

        // Clean up the window and exit the program.
        static_cast<void>
        (
          ::UnregisterClass(local_window_type::window_title(),
                            local_window_type::instance().get_handle_to_instance())
        );


        ::PostQuitMessage(static_cast<int>(INT8_C(0)));
      }

      const auto def_window_proc_result = ::DefWindowProc(handle_to_window, message, w_param, l_param);

      return def_window_proc_result;
    }

    static auto thread_process() -> void
    {
      while(my_thread_wants_exit == static_cast<::LONG>(INT8_C(0)))
      {
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<unsigned>(UINT8_C(250))));

        using local_window_type = win_text_simple;

        auto rect = ::RECT { };

        ::GetClientRect(local_window_type::instance().get_handle_to_window(), &rect);

        ::RedrawWindow(local_window_type::instance().get_handle_to_window(), &rect, nullptr, RDW_INVALIDATE);
      }

      static_cast<void>(::InterlockedExchange(&my_thread_did_exit, static_cast<::LONG>(INT8_C(1))));
    }
  };

  template<const char* WindowTitle,
           const int   IconId,
           const int   ScreenCoordinateX,
           const int   ScreenCoordinateY,
           const int   WindowWidth,
           const int   WindowHeight>
  std::thread win_text_simple<WindowTitle, IconId, ScreenCoordinateX, ScreenCoordinateY, WindowWidth, WindowHeight>::my_thread;

  template<const char* WindowTitle,
           const int   IconId,
           const int   ScreenCoordinateX,
           const int   ScreenCoordinateY,
           const int   WindowWidth,
           const int   WindowHeight>
  volatile ::LONG win_text_simple<WindowTitle, IconId, ScreenCoordinateX, ScreenCoordinateY, WindowWidth, WindowHeight>::my_thread_wants_exit;

  template<const char* WindowTitle,
           const int   IconId,
           const int   ScreenCoordinateX,
           const int   ScreenCoordinateY,
           const int   WindowWidth,
           const int   WindowHeight>
  volatile ::LONG win_text_simple<WindowTitle, IconId, ScreenCoordinateX, ScreenCoordinateY, WindowWidth, WindowHeight>::my_thread_did_exit;

#endif // WIN_TEXT_SIMPLE_2022_08_05_H
