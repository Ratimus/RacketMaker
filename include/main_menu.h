#pragma once

#include "canvas.h"
#include "ch_menu.h"

class MainMenu : public Canvas
{
  std::vector<std::unique_ptr<ChannelSettings>> channel_settings;

public:
  MainMenu()
      : Canvas("Main Menu")
  {
  }
};
