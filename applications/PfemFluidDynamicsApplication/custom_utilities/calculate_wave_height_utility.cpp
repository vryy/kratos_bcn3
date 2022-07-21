//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//


// System includes


// External includes


// Project includes
#include "utilities/math_utils.h"
#include "utilities/parallel_utilities.h"
#include "utilities/reduction_utilities.h"
#include "geometries/line_2d_2.h"
#include "geometries/line_3d_2.h"

// Application utilities
#include "calculate_wave_height_utility.h"


namespace Kratos
{

CalculateWaveHeightUtility::CalculateWaveHeightUtility(
    ModelPart& rThisModelPart,
    Parameters ThisParameters
) : mrModelPart(rThisModelPart)
{
    Parameters default_parameters(R"({
        "model_part_name"        : "",
        "mean_water_level"       : 0.0,
        "relative_search_radius" : 2.0,
        "search_tolerance"       : 1e-6,
        "use_local_element_size" : false,
        "use_nearest_node"       : false
    })");

    ThisParameters.ValidateAndAssignDefaults(default_parameters);

    const array_1d<double,3> gravity = mrModelPart.GetProcessInfo()[GRAVITY];
    mDirection = -gravity / norm_2(gravity);
    mUseLocalElementSize = ThisParameters["use_local_element_size"].GetBool();
    mUseNearestNode = ThisParameters["use_nearest_node"].GetBool();
    mMeanWaterLevel = ThisParameters["mean_water_level"].GetDouble();
    mAbsoluteRadius = ThisParameters["search_tolerance"].GetDouble();
    mRelativeRadius = ThisParameters["relative_search_radius"].GetDouble();
}


double CalculateWaveHeightUtility::Calculate(const array_1d<double,3>& rCoordinates) const
{
    if (mUseNearestNode) {
        return CalculateNearest(rCoordinates);
    } else {
        if (mUseLocalElementSize) {
            return CalculateAverage(rCoordinates, mRelativeRadius);
        } else {
            return CalculateAverage(rCoordinates);
        }
    }
}


double CalculateWaveHeightUtility::CalculateAverage(const array_1d<double,3>& rCoordinates, double SearchRadius) const
{
    KRATOS_TRY

    using MultipleReduction = CombinedReduction<SumReduction<double>, SumReduction<double>>; 

    double count = 0.0;
    double wave_height = 0.0;

    std::tie(count, wave_height) = block_for_each<MultipleReduction>(
        mrModelPart.Nodes(), [&](NodeType& rNode)
    {
        double local_count = 0.0;
        double local_wave_height = 0.0;

        if (rNode.IsNot(ISOLATED) && rNode.IsNot(RIGID) && rNode.Is(FREE_SURFACE)) {
            const array_1d<double,3> relative_position = rNode.Coordinates() - rCoordinates;
            const array_1d<double,3> horizontal_position = MathUtils<double>::CrossProduct(mDirection, relative_position);
            const double distance = norm_2(horizontal_position);

            if (distance < SearchRadius) {
                local_count = 1.0;
                local_wave_height = inner_prod(mDirection, rNode) - mMeanWaterLevel;
            }
        }
        return std::make_tuple(local_count, local_wave_height);
    });

    wave_height /= count;
    return wave_height;

    KRATOS_CATCH("");
}


double CalculateWaveHeightUtility::CalculateAverage(const array_1d<double,3>& rCoordinates) const
{
    KRATOS_TRY

    using MultipleReduction = CombinedReduction<SumReduction<double>, SumReduction<double>>; 

    double count = 0.0;
    double wave_height = 0.0;

    std::tie(count, wave_height) = block_for_each<MultipleReduction>(
        mrModelPart.Elements(), [&](Element& rElement)
    {
        double local_count = 0.0;
        double local_wave_height = 0.0;

        const double radius = mRelativeRadius * rElement.GetGeometry().Length() + mAbsoluteRadius;

        for (auto& r_node : rElement.GetGeometry()) {
            if (r_node.IsNot(ISOLATED) && r_node.IsNot(RIGID) && r_node.Is(FREE_SURFACE)) {
                const array_1d<double,3> relative_position = r_node.Coordinates() - rCoordinates;
                const array_1d<double,3> horizontal_position = MathUtils<double>::CrossProduct(mDirection, relative_position);
                const double distance = norm_2(horizontal_position);

                if (distance < radius) {
                    local_count = 1.0;
                    local_wave_height = inner_prod(mDirection, r_node) - mMeanWaterLevel;
                }
            }
        }
        return std::make_tuple(local_count, local_wave_height);
    });

    wave_height /= count;
    return wave_height;

    KRATOS_CATCH("");
}


double CalculateWaveHeightUtility::CalculateNearest(const array_1d<double,3>& rCoordinates) const
{
    KRATOS_TRY

    static array_1d<double,3> coordinates = rCoordinates; // Workaround to make variables accessible to the local reducer
    static array_1d<double,3> direction = mDirection;
    static double mean_water_level = mMeanWaterLevel;

    struct CustomReducer{
        using return_type = double;
        double distance = 1e16;
        double wave_height = 0.0;

        return_type GetValue()
        {
            return wave_height;
        }

        void LocalReduce(NodeType& rNode)
        {
            if (rNode.IsNot(ISOLATED) && rNode.IsNot(RIGID) && rNode.Is(FREE_SURFACE))
            {
                const array_1d<double,3> relative_position = rNode.Coordinates() - coordinates;
                const array_1d<double,3> horizontal_position = MathUtils<double>::CrossProduct(direction, relative_position);
                const double new_distance = norm_2(horizontal_position);
                if (new_distance < this->distance) {
                    this->distance = new_distance;
                    this->wave_height = inner_prod(direction, coordinates) - mean_water_level;
                }
            }
        }

        void ThreadSafeReduce(CustomReducer& rOther)
        {
            #pragma omp critical
            {
                if (rOther.distance < this->distance) {
                    this->distance = rOther.distance;
                    this->wave_height = rOther.wave_height;
                }
            }
        }
    };

    return block_for_each<CustomReducer>(mrModelPart.Nodes(),[&](NodeType& rNode) -> NodeType& {return rNode;});

    KRATOS_CATCH("")
}

}  // namespace Kratos.
