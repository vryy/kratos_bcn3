//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya
//

// System includes
#include <cmath>

// External includes

// Project includes
#include "includes/cfd_variables.h"
#include "includes/checks.h"
#include "includes/define.h"
#include "includes/element.h"
#include "utilities/geometry_utilities.h"
#include "utilities/time_discretization.h"

// Application includes
#include "convection_diffusion_reaction_stabilization_utilities.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_epsilon/epsilon_element_data.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_epsilon/k_element_data.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_omega/k_element_data.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_omega/omega_element_data.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_omega_sst/k_element_data.h"
#include "custom_elements/convection_diffusion_reaction_element_data/k_omega_sst/omega_element_data.h"
#include "custom_utilities/rans_calculation_utilities.h"

// Include base h
#include "convection_diffusion_reaction_cross_wind_stabilized_element.h"

namespace Kratos
{
///@name Kratos Classes
///@{

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateRightHandSide(
    VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    if (rRightHandSideVector.size() != TNumNodes) {
        rRightHandSideVector.resize(TNumNodes, false);
    }

    noalias(rRightHandSideVector) = ZeroVector(TNumNodes);

    // Get Shape function data
    Vector gauss_weights;
    Matrix shape_functions;
    ShapeFunctionDerivativesArrayType shape_derivatives;
    this->CalculateGeometryData(gauss_weights, shape_functions, shape_derivatives);
    const ShapeFunctionDerivativesArrayType& r_parameter_derivatives =
        this->GetGeometryParameterDerivatives();
    const IndexType num_gauss_points = gauss_weights.size();

    const double delta_time = this->GetDeltaTime(rCurrentProcessInfo);
    const double bossak_alpha = rCurrentProcessInfo[BOSSAK_ALPHA];
    const double bossak_gamma =
        TimeDiscretization::Bossak(bossak_alpha, 0.25, 0.5).GetGamma();
    const double dynamic_tau = rCurrentProcessInfo[DYNAMIC_TAU];

    BoundedMatrix<double, TDim, TDim> contravariant_metric_tensor;

    const auto& r_geometry = this->GetGeometry();
    TConvectionDiffusionReactionData element_data(r_geometry);

    element_data.CalculateConstants(rCurrentProcessInfo);

    for (IndexType g = 0; g < num_gauss_points; ++g) {
        const Matrix& r_shape_derivatives = shape_derivatives[g];
        const Vector gauss_shape_functions = row(shape_functions, g);

        this->CalculateContravariantMetricTensor(contravariant_metric_tensor,
                                                 r_parameter_derivatives[g]);

        element_data.CalculateGaussPointData(gauss_shape_functions, r_shape_derivatives);
        const array_1d<double, 3>& velocity = element_data.CalculateEffectiveVelocity(
            gauss_shape_functions, r_shape_derivatives);
        const double effective_kinematic_viscosity =
            element_data.CalculateEffectiveKinematicViscosity(
                gauss_shape_functions, r_shape_derivatives);

        const double reaction = element_data.CalculateReactionTerm(
            gauss_shape_functions, r_shape_derivatives);
        const double source = element_data.CalculateSourceTerm(
            gauss_shape_functions, r_shape_derivatives);

        double tau, element_length;
        ConvectionDiffusionReactionStabilizationUtilities::CalculateStabilizationTau(
            tau, element_length, velocity, contravariant_metric_tensor,
            reaction, effective_kinematic_viscosity, bossak_alpha, bossak_gamma,
            delta_time, dynamic_tau);

        BoundedVector<double, TNumNodes> velocity_convective_terms;
        this->GetConvectionOperator(velocity_convective_terms, velocity, r_shape_derivatives);

        const double s = std::abs(reaction);

        for (IndexType a = 0; a < TNumNodes; ++a) {
            double value = 0.0;

            value += gauss_shape_functions[a] * source;

            // Add supg stabilization terms
            value += (velocity_convective_terms[a] + s * gauss_shape_functions[a]) *
                     tau * source;

            rRightHandSideVector[a] += gauss_weights[g] * value;
        }
    }

    KRATOS_CATCH("");
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateLocalVelocityContribution(
    MatrixType& rDampingMatrix, VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo)
{
    CalculateDampingMatrix(rDampingMatrix, rCurrentProcessInfo);

    // Now calculate an additional contribution to the residual: r -= rDampingMatrix * (u,p)
    VectorType U;
    this->GetValuesVector(U);
    noalias(rRightHandSideVector) -= prod(rDampingMatrix, U);
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateMassMatrix(
    MatrixType& rMassMatrix, const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    if (rMassMatrix.size1() != TNumNodes || rMassMatrix.size2() != TNumNodes) {
        rMassMatrix.resize(TNumNodes, TNumNodes, false);
    }

    rMassMatrix.clear();

    // Get Shape function data
    Vector gauss_weights;
    Matrix shape_functions;
    ShapeFunctionDerivativesArrayType shape_derivatives;
    this->CalculateGeometryData(gauss_weights, shape_functions, shape_derivatives);
    const ShapeFunctionDerivativesArrayType& r_parameter_derivatives =
        this->GetGeometryParameterDerivatives();
    const IndexType num_gauss_points = gauss_weights.size();

    const double delta_time = this->GetDeltaTime(rCurrentProcessInfo);
    const double bossak_alpha = rCurrentProcessInfo[BOSSAK_ALPHA];
    const double bossak_gamma =
        TimeDiscretization::Bossak(bossak_alpha, 0.25, 0.5).GetGamma();
    const double dynamic_tau = rCurrentProcessInfo[DYNAMIC_TAU];

    BoundedMatrix<double, TDim, TDim> contravariant_metric_tensor;

    const auto& r_geometry = this->GetGeometry();
    TConvectionDiffusionReactionData element_data(r_geometry);

    element_data.CalculateConstants(rCurrentProcessInfo);

    for (IndexType g = 0; g < num_gauss_points; ++g) {
        const Matrix& r_shape_derivatives = shape_derivatives[g];
        const Vector& gauss_shape_functions = row(shape_functions, g);

        this->CalculateContravariantMetricTensor(contravariant_metric_tensor,
                                                 r_parameter_derivatives[g]);

        const double mass = gauss_weights[g] * (1.0 / TNumNodes);
        this->AddLumpedMassMatrix(rMassMatrix, mass);

        element_data.CalculateGaussPointData(gauss_shape_functions, r_shape_derivatives);
        const array_1d<double, 3>& velocity = element_data.CalculateEffectiveVelocity(
            gauss_shape_functions, r_shape_derivatives);
        BoundedVector<double, TNumNodes> velocity_convective_terms;
        this->GetConvectionOperator(velocity_convective_terms, velocity, r_shape_derivatives);

        const double effective_kinematic_viscosity =
            element_data.CalculateEffectiveKinematicViscosity(
                gauss_shape_functions, r_shape_derivatives);

        const double reaction = element_data.CalculateReactionTerm(
            gauss_shape_functions, r_shape_derivatives);

        double tau, element_length;
        ConvectionDiffusionReactionStabilizationUtilities::CalculateStabilizationTau(
            tau, element_length, velocity, contravariant_metric_tensor,
            reaction, effective_kinematic_viscosity, bossak_alpha, bossak_gamma,
            delta_time, dynamic_tau);

        const double s = std::abs(reaction);

        // Add mass stabilization terms
        for (IndexType i = 0; i < TNumNodes; ++i) {
            for (IndexType j = 0; j < TNumNodes; ++j) {
                rMassMatrix(i, j) +=
                    gauss_weights[g] * tau *
                    (velocity_convective_terms[i] + s * gauss_shape_functions[i]) *
                    gauss_shape_functions[j];
            }
        }
    }

    KRATOS_CATCH("");
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateDampingMatrix(
    MatrixType& rDampingMatrix, const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    if (rDampingMatrix.size1() != TNumNodes || rDampingMatrix.size2() != TNumNodes) {
        rDampingMatrix.resize(TNumNodes, TNumNodes, false);
    }

    rDampingMatrix.clear();

    // Get Shape function data
    Vector gauss_weights;
    Matrix shape_functions;
    ShapeFunctionDerivativesArrayType shape_derivatives;
    this->CalculateGeometryData(gauss_weights, shape_functions, shape_derivatives);
    const ShapeFunctionDerivativesArrayType& r_parameter_derivatives =
        this->GetGeometryParameterDerivatives();
    const IndexType num_gauss_points = gauss_weights.size();

    const double delta_time = this->GetDeltaTime(rCurrentProcessInfo);
    const double bossak_alpha = rCurrentProcessInfo[BOSSAK_ALPHA];
    const double bossak_gamma =
        TimeDiscretization::Bossak(bossak_alpha, 0.25, 0.5).GetGamma();
    const double dynamic_tau = rCurrentProcessInfo[DYNAMIC_TAU];
    const double eps = std::numeric_limits<double>::epsilon();

    array_1d<double, 3> variable_gradient;
    const Variable<double>& primal_variable =
        TConvectionDiffusionReactionData::GetScalarVariable();

    const auto& r_geometry = this->GetGeometry();
    TConvectionDiffusionReactionData element_data(r_geometry);

    element_data.CalculateConstants(rCurrentProcessInfo);

    BoundedMatrix<double, TDim, TDim> contravariant_metric_tensor;

    for (IndexType g = 0; g < num_gauss_points; ++g) {
        const Matrix& r_shape_derivatives = shape_derivatives[g];
        const Vector gauss_shape_functions = row(shape_functions, g);

        this->CalculateContravariantMetricTensor(contravariant_metric_tensor,
                                                 r_parameter_derivatives[g]);

        element_data.CalculateGaussPointData(gauss_shape_functions, r_shape_derivatives);
        const array_1d<double, 3>& velocity = element_data.CalculateEffectiveVelocity(
            gauss_shape_functions, r_shape_derivatives);
        BoundedVector<double, TNumNodes> velocity_convective_terms;
        this->GetConvectionOperator(velocity_convective_terms, velocity, r_shape_derivatives);
        const double velocity_magnitude = norm_2(velocity);

        const double effective_kinematic_viscosity =
            element_data.CalculateEffectiveKinematicViscosity(
                gauss_shape_functions, r_shape_derivatives);
        const double variable_gradient_norm =
            this->GetScalarVariableGradientNorm(r_shape_derivatives);
        const double relaxed_variable_acceleration =
            this->GetScalarVariableRelaxedAcceleration(gauss_shape_functions);
        this->CalculateGradient(variable_gradient, primal_variable, r_shape_derivatives);

        const double reaction = element_data.CalculateReactionTerm(
            gauss_shape_functions, r_shape_derivatives);

        double tau, element_length;
        ConvectionDiffusionReactionStabilizationUtilities::CalculateStabilizationTau(
            tau, element_length, velocity, contravariant_metric_tensor,
            reaction, effective_kinematic_viscosity, bossak_alpha, bossak_gamma,
            delta_time, dynamic_tau);

        // Calculate residual for cross wind dissipation coefficient
        double positivity_preserving_coefficient{0.0}, k1{0.0}, k2{0.0}, chi{0.0};
        const double velocity_magnitude_square = std::pow(velocity_magnitude, 2);

        const double velocity_dot_variable_gradient =
            inner_prod(velocity, variable_gradient);
        const double variable_value =
            this->EvaluateInPoint(primal_variable, gauss_shape_functions);

        if (variable_gradient_norm > eps && velocity_magnitude_square > eps) {
            const double source = element_data.CalculateSourceTerm(
                gauss_shape_functions, r_shape_derivatives);

            double residual = relaxed_variable_acceleration;
            residual += velocity_dot_variable_gradient;
            residual += reaction * variable_value;
            residual -= source;
            residual = std::abs(residual);

            ConvectionDiffusionReactionStabilizationUtilities::CalculateCrossWindDiffusionParameters(
                chi, k1, k2, velocity_magnitude, tau,
                effective_kinematic_viscosity, reaction, bossak_alpha,
                bossak_gamma, delta_time, element_length, dynamic_tau);

            positivity_preserving_coefficient =
                residual * chi / (variable_gradient_norm * velocity_magnitude_square);
        }

        const double s = std::abs(reaction);

        for (IndexType a = 0; a < TNumNodes; ++a) {
            for (IndexType b = 0; b < TNumNodes; ++b) {
                double dNa_dNb = 0.0;
                for (IndexType i = 0; i < TDim; ++i)
                    dNa_dNb += r_shape_derivatives(a, i) * r_shape_derivatives(b, i);

                double value = 0.0;

                value += gauss_shape_functions[a] * velocity_convective_terms[b];
                value += gauss_shape_functions[a] * reaction * gauss_shape_functions[b];
                value += effective_kinematic_viscosity * dNa_dNb;

                // Adding SUPG stabilization terms
                value += tau *
                         (velocity_convective_terms[a] + s * gauss_shape_functions[a]) *
                         velocity_convective_terms[b];
                value += tau *
                         (velocity_convective_terms[a] + s * gauss_shape_functions[a]) *
                         reaction * gauss_shape_functions[b];

                // Adding cross wind dissipation
                value += positivity_preserving_coefficient * k2 * dNa_dNb * velocity_magnitude_square;
                value -= positivity_preserving_coefficient * k2 *
                         velocity_convective_terms[a] * velocity_convective_terms[b];

                // Adding stream line dissipation
                value += positivity_preserving_coefficient * k1 *
                         velocity_convective_terms[a] * velocity_convective_terms[b];

                rDampingMatrix(a, b) += gauss_weights[g] * value;
            }
        }
    }

    element_data.UpdateElementDataValueContainer(*this);

    KRATOS_CATCH("");
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateGradient(
    BoundedMatrix<double, TDim, TDim>& rOutput,
    const Variable<array_1d<double, 3>>& rVariable,
    const Matrix& rShapeDerivatives,
    const int Step) const
{
    const auto& r_geometry = this->GetGeometry();

    RansCalculationUtilities::CalculateGradient<TDim>(
        rOutput, r_geometry, rVariable, rShapeDerivatives, Step);
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateGradient(
    array_1d<double, 3>& rOutput,
    const Variable<double>& rVariable,
    const Matrix& rShapeDerivatives,
    const int Step) const
{
    const auto& r_geometry = this->GetGeometry();
    RansCalculationUtilities::CalculateGradient(rOutput, r_geometry, rVariable,
                                                rShapeDerivatives, Step);
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
void ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::CalculateContravariantMetricTensor(
    BoundedMatrix<double, TDim, TDim>& rOutput, const Matrix& rParameterDerivatives) const
{
    noalias(rOutput) = prod(trans(rParameterDerivatives), rParameterDerivatives);
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
double ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::GetDeltaTime(
    const ProcessInfo& rProcessInfo) const
{
    return rProcessInfo[DELTA_TIME];
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
double ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::GetScalarVariableGradientNorm(
    const Matrix& rShapeFunctionDerivatives, const int Step) const
{
    KRATOS_TRY;

    array_1d<double, 3> scalar_variable_gradient;
    this->CalculateGradient(scalar_variable_gradient,
                            TConvectionDiffusionReactionData::GetScalarVariable(),
                            rShapeFunctionDerivatives, Step);
    return norm_2(scalar_variable_gradient);

    KRATOS_CATCH("");
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
double ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::GetScalarVariableRelaxedAcceleration(
    const Vector& rShapeFunctions, const int Step) const
{
    KRATOS_TRY;

    return this->EvaluateInPoint(
        TConvectionDiffusionReactionData::GetScalarRelaxedRateVariable(),
        rShapeFunctions, Step);

    KRATOS_CATCH("");
}

template <unsigned int TDim, unsigned int TNumNodes, class TConvectionDiffusionReactionData>
typename ConvectionDiffusionReactionCrossWindStabilizedElement<TDim, TNumNodes, TConvectionDiffusionReactionData>::ShapeFunctionDerivativesArrayType ConvectionDiffusionReactionCrossWindStabilizedElement<
    TDim,
    TNumNodes,
    TConvectionDiffusionReactionData>::GetGeometryParameterDerivatives() const
{
    const auto& r_geometry = this->GetGeometry();
    return RansCalculationUtilities::CalculateGeometryParameterDerivatives(
        r_geometry, this->GetIntegrationMethod());
}

// template instantiations
template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KEpsilonElementData::KElementData<2>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KEpsilonElementData::EpsilonElementData<2>>;

template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KOmegaElementData::KElementData<2>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KOmegaElementData::OmegaElementData<2>>;

template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KOmegaSSTElementData::KElementData<2>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<2, 3, KOmegaSSTElementData::OmegaElementData<2>>;

template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KEpsilonElementData::KElementData<3>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KEpsilonElementData::EpsilonElementData<3>>;

template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KOmegaElementData::KElementData<3>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KOmegaElementData::OmegaElementData<3>>;

template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KOmegaSSTElementData::KElementData<3>>;
template class ConvectionDiffusionReactionCrossWindStabilizedElement<3, 4, KOmegaSSTElementData::OmegaElementData<3>>;

} // namespace Kratos.
