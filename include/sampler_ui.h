#pragma once

#include <vector>
#include <memory>

#include "ui_manager.h"

#include "Latchable.h"
#include "MagicButton.h"
#include "ClickEncoder.h"
#include "pin_map.h"
#include "hw_inputs.h"
#include "ch_menu.h"

class SamplerUI : public UIManager
{
  HWInputs *inputs = nullptr;

public:
  SamplerUI();
  ~SamplerUI() = default;

  virtual void update() override { update_hw_state(); }
  void update_hw_state();
  void encoder_handler();
  void button_handler();
};
