#pragma once

#include <esp32_ui/canvas.h>
#include "ch_menu.h"

namespace esp32_ui
{
  class MainMenu : public Canvas
  {
    std::vector<std::unique_ptr<ChannelSettings>> channel_settings;

  public:
    MainMenu()
        : Canvas("Main Menu")
    {
    }
  };
} // namespace esp32_ui
