#include <optional>
#include <vector>

#include "sms/auxiliary/odd_closed_walk.hpp"

std::optional<OddClosedWalk>
violatedOddSelectionClosedWalk(const std::vector<node> &nodes, const std::vector<double> &weights)
{
    assert(nodes.size() == weights.size());
    assert(std::all_of(weights.begin(), weights.end(), [](auto w)
    {
        return (w >= 0) && (w <= 1);
    }));

    const uint64_t kUnassigned = std::numeric_limits<uint64_t>::max();

    double curLength = 0;
    std::vector<bool> crossEdges(nodes.size(), false);

    double lightestCross = 1;
    uint64_t lightestCrossIndex = kUnassigned;
    double heaviestStay = 0;
    uint64_t heaviestStayIndex = kUnassigned;

    int numCrossSelected = 0;

    for (uint64_t i = 0; i < nodes.size(); i++)
    {
        if (weights[i] > 0.5)
        {
            curLength += 1 - weights[i];
            crossEdges[i] = true;
            numCrossSelected += 1;
            if (weights[i] < lightestCross)
            {
                lightestCross = weights[i];
                lightestCrossIndex = i;
            }
        } else
        {
            curLength += weights[i];
            crossEdges[i] = false;
            if (weights[i] > heaviestStay)
            {
                heaviestStay = weights[i];
                heaviestStayIndex = i;
            }
        }
        if (curLength >= 1)
        {
            return {};
        }
    }

    if (numCrossSelected % 2 == 0)
    {
        if ((lightestCrossIndex != kUnassigned) &&
            (curLength + 2 * lightestCross - 1 < 1))
        {
            crossEdges[lightestCrossIndex] = false;
        } else if ((heaviestStayIndex != kUnassigned) &&
                   (curLength - 2 * heaviestStay + 1 < 1))
        {
            crossEdges[heaviestStayIndex] = true;
        } else
        {
            return {};
        }
    }

    auto oscw = OddClosedWalk(nodes, crossEdges);
    assert(oscw.isValid());
    return oscw;
}
