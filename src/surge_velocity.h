#pragma once

#include <array>
#include <cmath>

namespace shroudfire {

inline std::array<float, 3> surge_velocity(std::array<float, 3> current,
    float pitch_degrees, float yaw_degrees, float speed, bool add_momentum) {
    const float pitch = pitch_degrees * 0.01745329251994329577f;
    const float yaw = yaw_degrees * 0.01745329251994329577f;
    const std::array<float, 3> boost {
        speed * std::cos(pitch) * std::cos(yaw),
        speed * std::cos(pitch) * std::sin(yaw),
        -speed * std::sin(pitch)
    };
    for (std::size_t axis = 0; axis < current.size(); ++axis)
        current[axis] = (add_momentum ? current[axis] : 0.0f) + boost[axis];
    return current;
}

} // namespace shroudfire
