#pragma once

#include <vector>
#include <memory>

#include <esp32_ui/ui_manager.h>
#include <SequencerTools/ClickEncoder.h>
#include <SequencerTools/MagicButton.h>
#include <SequencerTools/Latchable.h>

#include "pin_map.h"
#include "hw_inputs.h"
#include "ch_menu.h"

namespace esp32_ui
{
  class SamplerUI : public UIManager
  {
    HWInputs *inputs = nullptr;

  public:
    SamplerUI();
    ~SamplerUI() = default;

    void encoder_handler();
    void button_handler();

    virtual void update() override
    {
      encoder_handler();
      button_handler();
    }

    virtual void screen_saver() override
    {
      static uint8_t free_idx[4]{0, 0, 0, 0};
      static int16_t radii[4][8]{{-1}, {-1}, {-1}, {-1}};

      for (uint8_t channel = 0; channel < NUM_VOICES; ++channel)
      {
        if (voice[channel].started())
        {
          // Add a new circle
          radii[channel][free_idx[channel]] = 0;
          ++free_idx[channel];
          if (free_idx[channel] == 8)
          {
            free_idx[channel] = 0;
          }
        }
      }

      auto *display = Display::instance();
      if (root_node->dirty_screen)
      {
        display->clearBuffer();
      }

      for (uint8_t ch = 0; ch < 4; ++ch)
      {
        for (uint8_t i = 0; i < 8; ++i)
        {
          if (radii[ch][i] > 128) // cap for 128x32 screen
          {
            radii[ch][i] = -1;
            if (!root_node->dirty_screen)
            {
              display->clearBuffer();
              root_node->dirty_screen = true;
            }
          }

          if (radii[ch][i] < 0)
          {
            continue;
          }

          if (!root_node->dirty_screen)
          {
            display->clearBuffer();
            root_node->dirty_screen = true;
          }

          display->drawCircle((1 + ch) * (128 / 5), 12, radii[ch][i]);
          radii[ch][i] += 1 + (radii[ch][i] / 3); // speed up as it grows
        }
      }

      if (root_node->dirty_screen)
      {
        // Show mute status
        display->setCursor(0, 23);
        display->printf(" %s     %s     %s     %s",
                        voice[0].muted.in ? "M" : " ",
                        voice[1].muted.in ? "M" : " ",
                        voice[2].muted.in ? "M" : " ",
                        voice[3].muted.in ? "M" : " ");

        display->sendBuffer();
        root_node->dirty_screen = false;
      }
    }
  };
} // namespace esp32_ui
