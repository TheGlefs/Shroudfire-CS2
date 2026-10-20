#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace shroudfire {

// Intersect a player-body sphere with a cone capped by reach, rather than
// sampling a few rays that leave gaps when the reach or angle is increased.
// forward is a unit vector; cone_width is the full angle in degrees.
inline bool knife_cone_intersects(const std::array<float, 3> &offset,
                                  const std::array<float, 3> &forward,
                                  float radius, float reach, float cone_width) {
    if (!std::isfinite(reach) || !std::isfinite(cone_width) ||
        !std::isfinite(radius) || radius < 0.0f ||
        reach <= 0.0f || cone_width < 0.0f || cone_width > 360.0f)
        return false;
    const double distance_squared = static_cast<double>(offset[0]) * offset[0] +
        static_cast<double>(offset[1]) * offset[1] + static_cast<double>(offset[2]) * offset[2];
    if (!std::isfinite(distance_squared)) return false;
    const double distance = std::sqrt(distance_squared);
    if (distance > reach + radius) return false;
    if (distance <= radius) return true;
    const double along = static_cast<double>(offset[0]) * forward[0] +
        static_cast<double>(offset[1]) * forward[1] + static_cast<double>(offset[2]) * forward[2];
    if (!std::isfinite(along)) return false;
    const double angle = std::acos(std::clamp(along / distance, -1.0, 1.0));
    const double half_angle = cone_width * 0.00872664625997164788;
    if (angle <= half_angle) return true;

    // Outside the angle, the closest point lies on the cone's edge or its
    // range cap. Comparing that distance also handles a zero-width cone.
    const double edge_projection = distance * std::cos(angle - half_angle);
    const double edge_distance = std::clamp(edge_projection, 0.0, static_cast<double>(reach));
    const double perpendicular = distance * std::sin(angle - half_angle);
    const double beyond_edge = edge_projection - edge_distance;
    return perpendicular * perpendicular + beyond_edge * beyond_edge <=
           static_cast<double>(radius) * radius + 0.000001;
}

} // namespace shroudfire
