#pragma once

class Timer {
public:
    Timer(float target_seconds);
    
    bool Update(float dt);
    void Pause();
    void Reset();
    
    float GetElapsedSeconds() const;
    bool IsPaused() const;

private:
    float elapsed_seconds_;
    float target_seconds_;
    bool is_paused_;
};
