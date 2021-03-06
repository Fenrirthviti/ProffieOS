#ifndef TRANSITION_COLOR_CYCLE_H
#define TRANSITION_COLOR_CYCLE_H

#include <algorithm>

// Usage: ColorCycleTr<MILLIS, START_RPM, END_RPM>
// MILLS:  number
// START_RPM: a number (defaults to 0)
// END_RPM: a number (defaults to 6000)
// return value: COLOR
// Tron-like transition.
class BladeBase;
template<int MILLIS, int start_rpm=0, int end_rpm = 6000>
class TrColorCycle : public TransitionBaseX<Int<MILLIS>> {
public:
  void run(BladeBase* base) {
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (delta > 1000000) delta = 1;
    
    if (this->done()) {
      fade_ = 1.0;
    } else {
      fade_ = (millis() - this->start_millis_) / (float)(this->len_);
    }
    
    float current_rpm = start_rpm * (1 - fade_) + end_rpm * fade_;
    float current_percentage = 100.0 * fade_;
    pos_ = fract(pos_ + delta / 60000000.0 * current_rpm);
    num_leds_ = base->num_leds() * 16384;
    start_ = pos_ * num_leds_;
    if (current_percentage == 100.0) {
      start_ = 0;
      end_ = num_leds_;
    } else if (current_percentage == 0.0) {
      start_ = 0;
      end_ = 0;
    } else {
      end_ = fract(pos_ + current_percentage / 100.0) * num_leds_;
    }
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    Range led_range(led * 16384, led * 16384 + 16384);
    int mix = 0;
    if (start_ <= end_) {
      mix = (Range(start_, end_) & led_range).size();
    } else {
      mix = (Range(0, end_) & led_range).size() +
                  (Range(start_, num_leds_) & led_range).size();
    }
    OverDriveColor ret = a;
    ret.c = a.c.mix(b.c, mix);
    return ret;
  }
private:
  float fade_ = 0.0;
  float pos_ = 0.0;
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
  uint32_t last_micros_;
};

#endif
