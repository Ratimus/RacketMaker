#pragma once
#include <memory>
#include <array>

#include "gateIn.h"
#include "MagicButton.h"
#include "Latchable.h"
#include "ClickEncoder.h"
#include "pin_map.h"
#include "menu_event.h"

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
      ev.type = MenuEvent::Select;
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

  MenuEvent read_button(uint8_t idx)
  {
    // TODO: this
    MenuEvent ev(MenuEvent::Source::PushButton, MenuEvent::Type::NoType, idx);
    return ev;
  }

private:
  HWInputs();
};
