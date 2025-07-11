#include "utils/Timer.hpp"

Timer::Timer(float target_seconds)
    : elapsed_seconds_(0), target_seconds_(target_seconds), is_paused_(false) {}

bool Timer::Update(float dt) {
    if (IsPaused()) return false;

    elapsed_seconds_ += dt;
    if (elapsed_seconds_ >= target_seconds_) {
        Reset();
        return true;
    }

    return false;
}

void Timer::Pause() {
    is_paused_ = true;
}

bool Timer::IsPaused() const {
    return is_paused_;
}

void Timer::Reset() {
    elapsed_seconds_ = 0;
}

float Timer::GetElapsedSeconds() const {
    return elapsed_seconds_;
}
