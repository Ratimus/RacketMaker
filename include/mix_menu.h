#pragma once

#include <esp32_ui/field.h>
#include <esp32_ui/canvas.h>
#include "sampler_voice.h"

class Mixer : public Canvas
{
public:
  Mixer()
      : Canvas("Mixer")
  {
    auto level_a = std::make_unique<ValueField<uint8_t>>("A", voice[0].mix.clock(), 0, 200, 1);
    level_a->register_getter(std::move([this]()
                                       { return voice[0].mix.in; }));
    level_a->register_setter(std::move([this](uint8_t level)
                                       { voice[0].mix.clock_in(level); }));
    auto W_level_a = this->add_element(std::move(level_a));
    W_level_a->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavLeft, MAIN_ENCODER_INDEX});
    W_level_a->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavRight, MAIN_ENCODER_INDEX});
    W_level_a->set_cursor_offset(0);
    W_level_a->hover_to_edit = true;

    auto level_b = std::make_unique<ValueField<uint8_t>>("B", voice[1].mix.clock(), 0, 200, 1);
    level_b->register_getter(std::move([this]()
                                       { return voice[1].mix.in; }));
    level_b->register_setter(std::move([this](uint8_t level)
                                       { voice[1].mix.clock_in(level); }));
    auto W_level_b = this->add_element(std::move(level_b));
    W_level_b->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavLeft, MAIN_ENCODER_INDEX});
    W_level_b->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavRight, MAIN_ENCODER_INDEX});
    W_level_b->set_cursor_offset(0);
    W_level_b->hover_to_edit = true;

    auto level_c = std::make_unique<ValueField<uint8_t>>("C", voice[2].mix.clock(), 0, 200, 1);
    level_c->register_getter(std::move([this]()
                                       { return voice[2].mix.in; }));
    level_c->register_setter(std::move([this](uint8_t level)
                                       { voice[2].mix.clock_in(level); }));
    auto W_level_c = this->add_element(std::move(level_c));
    W_level_c->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavLeft, MAIN_ENCODER_INDEX});
    W_level_c->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavRight, MAIN_ENCODER_INDEX});
    W_level_c->set_cursor_offset(0);
    W_level_c->hover_to_edit = true;

    auto level_d = std::make_unique<ValueField<uint8_t>>("D", voice[3].mix.clock(), 0, 200, 1);
    level_d->register_getter(std::move([this]()
                                       { return voice[3].mix.in; }));
    level_d->register_setter(std::move([this](uint8_t level)
                                       { voice[3].mix.clock_in(level); }));
    auto W_level_d = this->add_element(std::move(level_d));
    W_level_d->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavLeft, MAIN_ENCODER_INDEX});
    W_level_d->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::NavRight, MAIN_ENCODER_INDEX});
    W_level_d->set_cursor_offset(0);
    W_level_d->hover_to_edit = true;
  }

  virtual ~Mixer() = default;

  void handle_draw(Display *d) const override
  {
    if (header)
    {
      header->handle_draw(d);
    }

    size_t num_widgets = widgets.size();
    for (size_t n = 0; n < num_widgets; ++n)
    {
      uint8_t x = 0;
      uint8_t y = (n % 2) * 12 + 12;
      if (n > 1)
      {
        x += d->half_width();
      }
      d->setCursor(x, y);
      auto &child = widgets[n];
      if (child)
      {
        auto *el = child.get()->c_selected_element();
        if (el)
        {
          el->print_label(d);
          d->print(":");
          if (n == selected_index())
          {
            d->print("[");
          }
          else
          {
            d->print(" ");
          }
          el->print_value(d);
          if (n == selected_index())
          {
            d->print("]");
          }
        }
      }
    }
  }

  virtual bool handle_event(const MenuEvent &ev) override
  {
    if ((ev.type == MenuEvent::Type::NavUp) || (ev.type == MenuEvent::Type::NavDown))
    {
      active_widget()->handle_lose_focus();
      move_cursor(ev);
      active_widget()->handle_get_focus();
      return true;
    }

    if ((ev.type == MenuEvent::Type::NavLeft) || (ev.type == MenuEvent::Type::NavRight))
    {
      return active_widget()->handle_nav_delta(ev);
    }

    return Canvas::handle_event(ev);
  }
};
