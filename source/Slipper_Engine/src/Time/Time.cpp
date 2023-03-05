#include "Time.h"

#include <array>

namespace Slipper
{
std::chrono::time_point<std::chrono::system_clock> Time::m_lastFrameTime =
    std::chrono::high_resolution_clock::now();

std::chrono::time_point<std::chrono::system_clock> Time::m_startupTime =
    std::chrono::high_resolution_clock::now();

float Time::m_deltaTime = 0;

float Time::m_deltaSmooth = 0;
std::array<float, DELTA_SMOOTH_FRAMES> Time::m_lastDeltaTimes{};

float Time::m_timeSinceStartup = 0;

void Time::Tick(uint64_t FrameCount)
{
    const auto current_time = std::chrono::high_resolution_clock::now();

    m_timeSinceStartup = std::chrono::duration<float, std::chrono::seconds::period>(current_time -
                                                                                    m_startupTime)
                             .count();
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(current_time -
                                                                             m_lastFrameTime)
                      .count();

    m_lastDeltaTimes[FrameCount % m_lastDeltaTimes.size()] = m_deltaTime;
    {
        float total_smooth = 0;
        for (const float last_delta_time : m_lastDeltaTimes) {
            total_smooth += last_delta_time;
        }
        m_deltaSmooth = total_smooth / m_lastDeltaTimes.size();
    }

    m_lastFrameTime = current_time;
}

float Time::DeltaTime()
{
    return m_deltaTime;
}

float Time::DeltaTimeSmooth()
{
    return m_deltaSmooth;
}

float Time::TimeSinceStartup()
{
    return m_timeSinceStartup;
}
}  // namespace Slipper