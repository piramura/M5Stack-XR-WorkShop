#include "touch_input.h"

#include <cstdio>

#include <M5GFX.h>
#include <M5Unified.h>

#include "osc_transport.h"

namespace {
constexpr size_t MAX_DESTINATION_COUNT = 3;

enum class ScreenMode {
  DestinationSelection,
  Control,
};

int32_t screenWidth;
int32_t screenHeight;
bool lastPressed = false;
bool waitForTouchRelease = false;
bool clearSelectionOnNextUpdate = false;
size_t selectedDestinationIndex = 0;
ScreenMode screenMode = ScreenMode::Control;
LGFX_Button button;
LGFX_Button destinationButtons[MAX_DESTINATION_COUNT];
LGFX_Button changeDestinationButton;

#if defined(WORKSHOP_M5STICKS3)
constexpr int32_t PHYSICAL_BUTTON_X = 0;
constexpr int32_t PHYSICAL_BUTTON_Y = 0;

void drawStickDestinationSelection() {
  M5.Display.fillScreen(WHITE);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Select Target", screenWidth / 2, 8,
                        &fonts::FreeMonoBold9pt7b);
  M5.Display.drawString(getOscDestinationName(selectedDestinationIndex),
                        screenWidth / 2, 47,
                        &fonts::FreeMonoBold12pt7b);
  M5.Display.drawString("A: OK  B: Next", screenWidth / 2, 105,
                        &fonts::FreeMono9pt7b);
}

void drawStickControlStatus(const char* state) {
  constexpr int statusHeight = 38;
  char text[40];
  if (isOscEnabled()) {
    snprintf(text, sizeof(text), "%s: %s",
             getOscDestinationName(selectedDestinationIndex), state);
  } else {
    snprintf(text, sizeof(text), "Button: %s", state);
  }

  M5.Display.fillRect(0, 0, screenWidth, statusHeight, WHITE);
  M5.Display.setTextDatum(top_center);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Display.drawString(text, screenWidth / 2, 8,
                        &fonts::FreeMonoBold9pt7b);
}

void drawStickControlScreen() {
  M5.Display.fillScreen(WHITE);
  drawStickControlStatus("Ready");
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("A: SEND", screenWidth / 2, 52,
                        &fonts::FreeMonoBold12pt7b);
  if (isOscEnabled()) {
    M5.Display.drawString("B: Target", screenWidth / 2, 105,
                          &fonts::FreeMono9pt7b);
  }
}

TouchEvent updateStickControlButton() {
  const bool pressed = M5.BtnA.isPressed();
  const bool changed = pressed != lastPressed;

  if (changed) {
    lastPressed = pressed;
    drawStickControlStatus(pressed ? "Pressed" : "Ready");
    Serial.printf("button A %s\n", pressed ? "pressed" : "released");
  }

  return {changed, pressed, PHYSICAL_BUTTON_X, PHYSICAL_BUTTON_Y};
}

TouchEvent updateStickInput() {
  if (clearSelectionOnNextUpdate) {
    clearSelectionOnNextUpdate = false;
    clearOscDestinationSelection();
  }

  if (waitForTouchRelease) {
    if (!M5.BtnA.isPressed() && !M5.BtnB.isPressed()) {
      waitForTouchRelease = false;
      lastPressed = false;
    }
    return {false, false, PHYSICAL_BUTTON_X, PHYSICAL_BUTTON_Y};
  }

  if (screenMode == ScreenMode::DestinationSelection) {
    if (M5.BtnB.wasPressed()) {
      selectedDestinationIndex =
          (selectedDestinationIndex + 1) % getOscDestinationCount();
      drawStickDestinationSelection();
    } else if (M5.BtnA.wasPressed()) {
      selectOscDestination(selectedDestinationIndex);
      screenMode = ScreenMode::Control;
      waitForTouchRelease = true;
      drawStickControlScreen();
    }
    return {false, false, PHYSICAL_BUTTON_X, PHYSICAL_BUTTON_Y};
  }

  if (isOscEnabled() && M5.BtnB.wasPressed()) {
    const bool releaseNeeded = lastPressed;
    lastPressed = false;
    screenMode = ScreenMode::DestinationSelection;
    waitForTouchRelease = true;
    drawStickDestinationSelection();

    if (releaseNeeded) {
      clearSelectionOnNextUpdate = true;
      return {true, false, PHYSICAL_BUTTON_X, PHYSICAL_BUTTON_Y};
    }

    clearOscDestinationSelection();
    return {false, false, PHYSICAL_BUTTON_X, PHYSICAL_BUTTON_Y};
  }

  return updateStickControlButton();
}
#endif

void drawStatus(const char* text) {
  constexpr int statusAreaHeight = 72;
  constexpr int textY = 10;
  M5.Display.fillRect(0, 0, screenWidth, statusAreaHeight, WHITE);
  M5.Display.drawString(text, screenWidth / 2, textY,
                        &fonts::FreeMonoBold12pt7b);
}

void drawDestinationSelection() {
  constexpr int titleAreaHeight = 55;
  constexpr int buttonHeight = 48;
  constexpr int buttonGap = 10;
  constexpr int bottomMargin = 10;
  const size_t destinationCount = getOscDestinationCount();

  M5.Display.fillScreen(WHITE);
  M5.Display.setTextDatum(top_center);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Display.drawString("Select Target", screenWidth / 2, 12,
                        &fonts::FreeMonoBold12pt7b);

  const int groupHeight = destinationCount * buttonHeight +
                          (destinationCount - 1) * buttonGap;
  const int availableHeight = screenHeight - titleAreaHeight - bottomMargin;
  const int firstCenterY = titleAreaHeight + (availableHeight - groupHeight) / 2 +
                           buttonHeight / 2;

  for (size_t i = 0; i < destinationCount; ++i) {
    const int centerY = firstCenterY + i * (buttonHeight + buttonGap);
    destinationButtons[i].initButton(
        &M5.Display, screenWidth / 2, centerY, screenWidth - 40, buttonHeight,
        TFT_BLUE, TFT_CYAN, TFT_BLACK, getOscDestinationName(i), 2, 2);
    destinationButtons[i].drawButton();
  }
}

void drawControlStatus(const char* state) {
  constexpr int statusWidth = 220;
  constexpr int statusHeight = 56;
  char text[48];
  snprintf(text, sizeof(text), "%s: %s",
           getOscDestinationName(selectedDestinationIndex), state);

  M5.Display.fillRect(0, 0, statusWidth, statusHeight, WHITE);
  M5.Display.setTextDatum(middle_center);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Display.drawString(text, statusWidth / 2, statusHeight / 2,
                        &fonts::FreeMonoBold12pt7b);
}

void drawControlScreen() {
  M5.Display.fillScreen(WHITE);
  drawControlStatus("Ready");

  changeDestinationButton.initButton(
      &M5.Display, screenWidth - 48, 28, 88, 40, TFT_DARKGREY,
      TFT_LIGHTGREY, TFT_BLACK, "Target", 1, 1);
  changeDestinationButton.drawButton();

  button.initButton(&M5.Display, screenWidth / 2, 148, 190, 150, TFT_BLUE,
                    TFT_YELLOW, TFT_BLACK, "BTN", 4, 4);
  button.drawButton();
}

TouchEvent updateControlTouch(int32_t x, int32_t y, bool touched) {
  const bool pressed = touched && button.contains(x, y);
  const bool changed = pressed != lastPressed;

  if (changed) {
    lastPressed = pressed;
    if (isOscEnabled()) {
      drawControlStatus(pressed ? "Pressed" : "Ready");
    } else {
      drawStatus(pressed ? "Button Pressed" : "Button Released");
    }
    Serial.printf("touch %s x=%d y=%d\n",
                  pressed ? "pressed" : "released", x, y);
  }

  return {changed, pressed, x, y};
}
}  // namespace

void initializeTouchInput() {
  M5.Display.setRotation(1);
  screenWidth = M5.Display.width();
  screenHeight = M5.Display.height();

#if defined(WORKSHOP_M5STICKS3)
  if (isOscEnabled()) {
    clearOscDestinationSelection();
    screenMode = ScreenMode::DestinationSelection;
    drawStickDestinationSelection();
  } else {
    screenMode = ScreenMode::Control;
    drawStickControlScreen();
  }
  return;
#endif

  if (isOscEnabled()) {
    clearOscDestinationSelection();
    screenMode = ScreenMode::DestinationSelection;
    drawDestinationSelection();
    return;
  }

  M5.Display.fillScreen(WHITE);
  M5.Display.setTextDatum(top_center);
  drawStatus("Touch Ready");

  constexpr int buttonSize = 200;
  button.initButton(&M5.Display, screenWidth / 2, screenHeight / 2,
                    buttonSize, buttonSize, TFT_BLUE, TFT_YELLOW, TFT_BLACK,
                    "BTN", 4, 4);
  button.drawButton();
}

TouchEvent updateTouchInput() {
#if defined(WORKSHOP_M5STICKS3)
  return updateStickInput();
#else
  const auto touch = M5.Touch.getDetail();

  if (!isOscEnabled()) {
    return updateControlTouch(touch.x, touch.y, touch.isPressed());
  }

  if (clearSelectionOnNextUpdate) {
    clearSelectionOnNextUpdate = false;
    clearOscDestinationSelection();
  }

  if (waitForTouchRelease) {
    if (!touch.isPressed()) {
      waitForTouchRelease = false;
      lastPressed = false;
    }
    return {false, false, touch.x, touch.y};
  }

  if (screenMode == ScreenMode::DestinationSelection) {
    if (touch.isPressed()) {
      const size_t destinationCount = getOscDestinationCount();
      for (size_t i = 0; i < destinationCount; ++i) {
        if (destinationButtons[i].contains(touch.x, touch.y)) {
          selectedDestinationIndex = i;
          selectOscDestination(i);
          screenMode = ScreenMode::Control;
          waitForTouchRelease = true;
          drawControlScreen();
          break;
        }
      }
    }
    return {false, false, touch.x, touch.y};
  }

  if (touch.isPressed() &&
      changeDestinationButton.contains(touch.x, touch.y)) {
    const bool releaseNeeded = lastPressed;
    lastPressed = false;
    screenMode = ScreenMode::DestinationSelection;
    waitForTouchRelease = true;
    drawDestinationSelection();

    if (releaseNeeded) {
      clearSelectionOnNextUpdate = true;
      return {true, false, touch.x, touch.y};
    }

    clearOscDestinationSelection();
    return {false, false, touch.x, touch.y};
  }

  return updateControlTouch(touch.x, touch.y, touch.isPressed());
#endif
}
