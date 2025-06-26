#pragma once

#include "field.h"
#include "sampler_voice.h"

#include "canvas.h"
#include "toggle_element.h"

static constexpr const char *channel_labels[] = {
    "Channel A", "Channel B", "Channel C", "Channel D"};

class ChannelSettings : public Canvas
{
  std::unique_ptr<Header> uhdr;
  uint8_t channel;

public:
  ChannelSettings(uint8_t channel)
      : Canvas(channel_labels[channel]),
        channel(channel)
  {
    uhdr = std::make_unique<Header>(channel_labels[channel]);
    header = uhdr.get();
    build_canvas();
  }

  virtual ~ChannelSettings() = default;

  void build_canvas()
  {
    auto sample = std::make_unique<ValueField<int16_t>>("Sample", voice[channel].sample.clock(), 0, NUM_SAMPLES - 1, 1);
    sample->register_getter(std::move([this]()
                                      { return voice[channel].sample.in; }));
    sample->register_setter(std::move([this](int16_t idx)
                                      { voice[channel].sample.clock_in(idx); }));
    this->add_element(std::move(sample));

    auto level = std::make_unique<ValueField<int16_t>>("Level", voice[channel].mix.clock(), 0, 200, 1);
    level->register_getter(std::move([this]()
                                     { return voice[channel].mix.in; }));
    level->register_setter(std::move([this](int16_t level)
                                     { voice[channel].mix.clock_in(level); }));
    level->set_big_step(10);
    this->add_element(std::move(level));

    auto decay = std::make_unique<ValueField<int16_t>>("Decay", voice[channel].decay.clock(), 1, 1000, 1);
    decay->register_getter(std::move([this]()
                                     { return voice[channel].decay.in; }));
    decay->register_setter(std::move([this](int16_t decay)
                                     { voice[channel].decay.clock_in(decay); }));
    decay->set_big_step(100);
    this->add_element(std::move(decay));

    auto pitch = std::make_unique<ValueField<int16_t>>("Pitch", voice[channel].pitch.clock(), -96, 96, 1);
    pitch->register_getter(std::move([this]()
                                     { return voice[channel].pitch.in; }));
    pitch->register_setter(std::move([this](int16_t pitch)
                                     { voice[channel].pitch.clock_in(pitch); }));
    pitch->set_big_step(12);
    this->add_element(std::move(pitch));

    if (!(channel % 2))
    {
      auto choke = std::make_unique<ToggleElement>((channel == 0) ? "Choke Ch. B" : "Choke Ch. D", "YES", "NO", voice[channel].choke.in);
      choke->register_event_listener(MenuEvent{MenuEvent::Source::Encoder, MenuEvent::Type::Select, MAIN_ENCODER_INDEX});

      // This is an on_change() callback; the value toggles and THEN the callback fires
      choke->register_handler([this]
                              { voice[channel].choke.clock_in(!voice[channel].choke.in); });
      this->add_element(std::move(choke));
    }
  }
};
