#pragma once
#include <memory>
#include <array>

#include <SequencerTools/GateIn.h>
#include <SequencerTools/MagicButton.h>
#include <SequencerTools/Latchable.h>
#include <SequencerTools/ClickEncoder.h>
#include <esp32_ui/menu_event.h>
#include "pin_map.h"

using MenuEvent = esp32_ui::MenuEvent;

extern TaskHandle_t hw_input_task_handle;

class HWInputs
{
  std::unique_ptr<ClickEncoder> encoder;
  std::array<MagicButton, 4> push_buttons;

  std::array<latchable<ButtonState>, 4> button_states;
  latchable<ButtonState> encoder_button;
  latchable<int64_t> encoder_val;

public:
  volatile uint8_t rising_flags = 0;
  volatile uint8_t falling_flags = 0;

  void init_cv();

  static HWInputs *instance()
  {
    static HWInputs __instance__;
    return &__instance__;
  }

  static void start_hw_input_task();
  void service()
  {
    for (auto &pb : push_buttons)
    {
      pb.service();
    }
    encoder->service();
  }

  // This makes MenuEvents out of encoder states
  MenuEvent read_encoder()
  {
    MenuEvent ev(MenuEvent::Source::Encoder, MenuEvent::Type::NoType, MAIN_ENCODER_INDEX);

    encoder_val.set_input(encoder->readPosition());
    encoder_button.set_input(encoder->readButton());
    if (!encoder_val.pending() && !encoder_button.pending())
    {
      return ev;
    }

    int64_t diff = encoder_val.in - encoder_val.out;
    encoder_val.clock();
    ButtonState next = encoder_button.clock();

    if (diff > 0)
    {
      if (next == ButtonState::Held)
      {
        ev.type = MenuEvent::Type::NavDown;
      }
      else
      {
        ev.type = MenuEvent::Type::NavRight;
      }
    }
    else if (diff < 0)
    {
      if (next == ButtonState::Held)
      {
        ev.type = MenuEvent::Type::NavUp;
      }
      else
      {
        ev.type = MenuEvent::Type::NavLeft;
      }
    }
    else if (next == ButtonState::Clicked)
    {
      ev.type = MenuEvent::Type::Select;
    }
    else if (next == ButtonState::DoubleClicked)
    {
      ev.type = MenuEvent::Type::Back;
    }
    else
    {
      // Other button states (Held, Open, etc.) are ignored here unless/until needed
    }

    return ev;
  }

  // This makes MenuEvents out of button states
  MenuEvent read_button(uint8_t idx)
  {
    button_states[idx].in = push_buttons[idx].read();
    MenuEvent ev(MenuEvent::Source::PushButton, MenuEvent::Type::NoType, idx);
    if (button_states[idx].pending())
    {
      switch (button_states[idx].clock())
      {
      case ButtonState::Clicked:
        ev.type = MenuEvent::Type::Select;
        break;
      case ButtonState::DoubleClicked:
        ev.type = MenuEvent::Type::Back;
        break;
      case ButtonState::Held:
        ev.type = MenuEvent::Type::ButtonHeld;
        break;
      case ButtonState::Released:
        ev.type = MenuEvent::Type::ButtonReleased;
        break;
      default:
        break;
      }
    }
    return ev;
  }

private:
  HWInputs();
};
