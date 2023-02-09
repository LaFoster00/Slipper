#pragma once

#include <chrono>

namespace Slipper
{
class Time
{
 public:
    static void Tick(uint64_t FrameCount);

    [[nodiscard]] static float DeltaTime();

    [[nodiscard]] static float DeltaTimeSmooth();

    [[nodiscard]] static float TimeSinceStartup();

 private:
    static float m_deltaTime;

    static float m_deltaSmooth;
    static std::array<float, 16> m_lastDeltaTimes;

    static float m_timeSinceStartup;

    static std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTime;
    static std::chrono::time_point<std::chrono::steady_clock> m_startupTime;
};
}  // namespace Slipper