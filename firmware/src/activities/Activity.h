#pragma once

#include <string>
#include <utility>
#include <Arduino.h>

class MappedInputManager;
class GfxRenderer;

class Activity {
protected:
  String name;
  GfxRenderer& renderer;
  MappedInputManager& input;

public:
  Activity(String name, GfxRenderer& renderer, MappedInputManager& input)
      : name(std::move(name)), renderer(renderer), input(input) {}
  virtual ~Activity() = default;
  virtual void onEnter() { Serial.printf("[%lu] [ACT] Entering activity: %s\n", millis(), name.c_str()); }
  virtual void onExit() { Serial.printf("[%lu] [ACT] Exiting activity: %s\n", millis(), name.c_str()); }
  virtual void loop() {}
};
