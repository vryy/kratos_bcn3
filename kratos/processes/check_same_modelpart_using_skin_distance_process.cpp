//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Vicente Mataix Ferrandiz
//
//

// System includes
#include <limits>

// External includes

// Project includes
#include "geometries/hexahedra_3d_8.h"
#include "processes/check_same_modelpart_using_skin_distance_process.h"
#include "utilities/auxiliar_model_part_utilities.h"
// #include "processes/calculate_distance_to_skin_process.h" // Continuous version
#include "processes/calculate_discontinuous_distance_to_skin_process.h" // Discontinuous version
#include "processes/structured_mesh_generator_process.h"

namespace Kratos
{
template<std::size_t TDim>
void CheckSameModelPartUsingSkinDistanceProcess<TDim>::Execute()
{
    KRATOS_TRY

    // We get the names of the model parts
    const std::string& r_skin_model_part_1_name = mThisParameters["skin_model_part_1_name"].GetString();
    const std::string& r_skin_model_part_2_name = mThisParameters["skin_model_part_2_name"].GetString();

    // We get the model parts
    ModelPart& r_skin_model_part_1 = mrModel.GetModelPart(r_skin_model_part_1_name);
    ModelPart& r_skin_model_part_2 = mrModel.GetModelPart(r_skin_model_part_2_name);
    
    // Checking that the model part contains conditions
    KRATOS_ERROR_IF(r_skin_model_part_1.NumberOfConditions() == 0) << "The first model part does not contain conditions" << std::endl;
    KRATOS_ERROR_IF(r_skin_model_part_2.NumberOfConditions() == 0) << "The second model part does not contain conditions" << std::endl;
    if constexpr (TDim == 2) { // 2D
        KRATOS_ERROR_IF_NOT(r_skin_model_part_1.Conditions().begin()->GetGeometry().GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Line2D2) << "Conditions from first model part must be lines in 2D space" << std::endl;
        KRATOS_ERROR_IF_NOT(r_skin_model_part_2.Conditions().begin()->GetGeometry().GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Line2D2) << "Conditions from second model part must be lines in 2D space" << std::endl;
    } else { // 3D
        KRATOS_ERROR_IF_NOT(r_skin_model_part_1.Conditions().begin()->GetGeometry().GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Triangle3D3) << "Conditions from first model part must be triangles in 3D space" << std::endl;
        KRATOS_ERROR_IF_NOT(r_skin_model_part_2.Conditions().begin()->GetGeometry().GetGeometryType() == GeometryData::KratosGeometryType::Kratos_Triangle3D3) << "Conditions from second model part must be triangles in 3D space" << std::endl;
    }

    // We get the coordinates of the bounding box
    using NodeType = Node<3>;
    using BBReduction = CombinedReduction<MaxReduction<double>, MinReduction<double>, MaxReduction<double>, MinReduction<double>, MaxReduction<double>, MinReduction<double>>;
    double max_x, min_x, max_y, min_y, max_z, min_z;
    std::tie(max_x, min_x, max_y, min_y, max_z, min_z) = block_for_each<BBReduction>(r_skin_model_part_1.Nodes(), [&](NodeType& rNode) {
        return std::make_tuple(rNode.X(),rNode.X(),rNode.Y(),rNode.Y(),rNode.Z(),rNode.Z());
    });
    const double bounding_box_scale_factor = mThisParameters["bounding_box_scale_factor"].GetDouble();
    max_x *= bounding_box_scale_factor;
    min_x *= bounding_box_scale_factor;
    max_y *= bounding_box_scale_factor;
    min_y *= bounding_box_scale_factor;
    max_z *= bounding_box_scale_factor;
    min_z *= bounding_box_scale_factor;

    // Generate background mesh
    auto p_point_1 = Kratos::make_intrusive<Node<3>>(1, min_x, min_y, min_z);
    auto p_point_2 = Kratos::make_intrusive<Node<3>>(2, max_x, min_y, min_z);
    auto p_point_3 = Kratos::make_intrusive<Node<3>>(3, max_x, max_y, min_z);
    auto p_point_4 = Kratos::make_intrusive<Node<3>>(4, min_x, max_y, min_z);
    auto p_point_5 = Kratos::make_intrusive<Node<3>>(5, min_x, min_y, max_z);
    auto p_point_6 = Kratos::make_intrusive<Node<3>>(6, max_x, min_y, max_z);
    auto p_point_7 = Kratos::make_intrusive<Node<3>>(7, max_x, max_y, max_z);
    auto p_point_8 = Kratos::make_intrusive<Node<3>>(8, min_x, max_y, max_z);
    Hexahedra3D8<Node<3>> geometry(p_point_1, p_point_2, p_point_3, p_point_4, p_point_5, p_point_6, p_point_7, p_point_8);

    Parameters mesher_parameters(R"({
        "number_of_divisions"        : -1,
        "element_name"               : "Element3D4N",
        "create_skin_sub_model_part" : false
    })");
    mesher_parameters["number_of_divisions"].SetInt(mThisParameters["number_of_divisions_background_mesh"].GetInt());
    ModelPart& r_model_part_1 = mrModel.CreateModelPart("BACKGROUND_MESH_1");
    StructuredMeshGeneratorProcess(geometry, r_model_part_1, mesher_parameters).Execute();

    // Using the same geometry, we create the second background mesh, but values are stored in a different model part
    ModelPart& r_model_part_2 = AuxiliarModelPartUtilities(r_model_part_1).DeepCopyModelPart("BACKGROUND_MESH_2", &mrModel);

    // Compute the distance to the skin
    Parameters distance_parameters = mThisParameters["discontinuous_distance_settings"];
    CalculateDiscontinuousDistanceToSkinProcess<TDim> distance_calculator_1(r_model_part_1, r_skin_model_part_1, distance_parameters);
    distance_calculator_1.Execute();
    CalculateDiscontinuousDistanceToSkinProcess<TDim> distance_calculator_2(r_model_part_2, r_skin_model_part_2, distance_parameters);
    distance_calculator_2.Execute();

    // Compute the average length of the elements in order to compute the tolerance
    auto it_cond_begin_1 = r_skin_model_part_1.ConditionsBegin();
    const std::size_t total_number_conditions = r_skin_model_part_1.NumberOfConditions();
    double average_length = IndexPartition<std::size_t>(total_number_conditions).for_each<SumReduction<double>>([&](std::size_t i) {
        auto it_cond_1 = it_cond_begin_1 + i;
        return it_cond_1->GetGeometry().Length();
    });
    average_length /= static_cast<double>(total_number_conditions);

    // Now we check that the difference is near a tolerance
    const double tolerance = average_length * mThisParameters["tolerance"].GetDouble();

    // Interate over the elements
    auto it_elem_begin_1 = r_model_part_1.ElementsBegin();
    auto it_elem_begin_2 = r_model_part_2.ElementsBegin();
    const auto& r_elem_dist_var = KratosComponents<Variable<Vector>>::Get(distance_parameters["elemental_distances_variable"].GetString());
    const double error = IndexPartition<std::size_t>(r_model_part_1.NumberOfElements()).for_each<SumReduction<double>>([&](std::size_t i) {
        auto it_elem_1 = it_elem_begin_1 + i;
        auto it_elem_2 = it_elem_begin_2 + i;
        const Vector& r_elem_dist_1 = it_elem_1->GetValue(r_elem_dist_var);
        const Vector& r_elem_dist_2 = it_elem_2->GetValue(r_elem_dist_var);
        return norm_2(r_elem_dist_1 - r_elem_dist_2);
    });

    if (error > tolerance) {
        KRATOS_ERROR << "The distance between the two model parts is " << error << " and the tolerance is " << tolerance << std::endl;
    } else {
        KRATOS_INFO("CheckSameModelPartUsingSkinDistanceProcess") << "The distance between the two model parts is " << error << " and the tolerance is " << tolerance << ". It would be assumed it is the same model part." << std::endl;
    }

    // TODO: Add MPI version

    // Cleaning up created model parts
    mrModel.DeleteModelPart("BACKGROUND_MESH_1");
    mrModel.DeleteModelPart("BACKGROUND_MESH_2");

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
const Parameters CheckSameModelPartUsingSkinDistanceProcess<TDim>::GetDefaultParameters() const
{
    KRATOS_TRY

    const Parameters default_parameters = Parameters(R"(
    {
        "skin_model_part_1_name"              : "PLEASE_SPECIFY_SKIN_MODEL_PART_2_NAME",
        "skin_model_part_2_name"              : "PLEASE_SPECIFY_SKIN_MODEL_PART_2_NAME",
        "tolerance"                           : 1.0e-3,
        //"continuous_distance"               : false, // TODO: Add continuous version if needed in the future
        "bounding_box_scale_factor"           : 1.5,
        "number_of_divisions_background_mesh" : 30,
        "discontinuous_distance_settings": {
        }
    })" );

    return default_parameters;

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

template class Kratos::CheckSameModelPartUsingSkinDistanceProcess<2>;
template class Kratos::CheckSameModelPartUsingSkinDistanceProcess<3>;

} // namespace Kratos
