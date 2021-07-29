//
//   Project Name:        KratosFluidDynamicsApplication $
//   Last modified by:    $Author:               AFranci $
//   Date:                $Date:               June 2021 $
//   Revision:            $Revision:                 0.0 $
//
//   Implementation of the Gauss-Seidel two step Updated Lagrangian Velocity-Pressure element
//     ( There is a ScalingConstant to multiply the mass balance equation for a number because i read it somewhere)
//

// System includes

// External includes

// Project includes
#include "custom_elements/three_step_updated_lagrangian_element.h"
#include "includes/cfd_variables.h"

namespace Kratos
{

  template <unsigned int TDim>
  Element::Pointer ThreeStepUpdatedLagrangianElement<TDim>::Clone(IndexType NewId, NodesArrayType const &rThisNodes) const
  {
    KRATOS_TRY;

    ThreeStepUpdatedLagrangianElement NewElement(NewId, this->GetGeometry().Create(rThisNodes), this->pGetProperties());
    return Element::Pointer(new ThreeStepUpdatedLagrangianElement(NewElement));

    KRATOS_CATCH("");
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateLocalSystem(MatrixType &rLeftHandSideMatrix,
                                                                     VectorType &rRightHandSideVector,
                                                                     const ProcessInfo &rCurrentProcessInfo)
  {
    KRATOS_TRY;

    switch (rCurrentProcessInfo[FRACTIONAL_STEP])
    {
    case 1:
    {
      this->CalculateFirstVelocitySystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      break;
    }
    case 5:
    {
      CalculateStandardFSPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      //CalculatePSPGPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      //CalculateFSplusPSPGPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      //CalculateFICPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      break;
    }
    case 6:
    {
      this->CalculateLastVelocitySystem(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
      break;
    }

    default:
    {
      KRATOS_THROW_ERROR(std::logic_error, "Unexpected value for THREE_STEP_UPDATED_LAGRANGIAN_V_P_ELEMENT index: ", rCurrentProcessInfo[FRACTIONAL_STEP]);
    }
    }

    KRATOS_CATCH("");
  }
  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::EquationIdVector(EquationIdVectorType &rResult,
                                                                 const ProcessInfo &rCurrentProcessInfo) const
  {
    KRATOS_TRY;
    switch (rCurrentProcessInfo[FRACTIONAL_STEP])
    {
    case 1:
    {
      this->VelocityEquationIdVector(rResult, rCurrentProcessInfo);
      break;
    }
    case 5:
    {
      this->PressureEquationIdVector(rResult, rCurrentProcessInfo);
      break;
    }
    case 6:
    {
      this->VelocityEquationIdVector(rResult, rCurrentProcessInfo);
      break;
    }
    default:
    {
      KRATOS_THROW_ERROR(std::logic_error, "Unexpected value for FRACTIONAL_STEP index: ", rCurrentProcessInfo[FRACTIONAL_STEP]);
    }
    }

    KRATOS_CATCH("");
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::GetDofList(DofsVectorType &rElementalDofList,
                                                           const ProcessInfo &rCurrentProcessInfo) const
  {
    KRATOS_TRY;
    switch (rCurrentProcessInfo[FRACTIONAL_STEP])
    {
    case 1:
    {
      this->GetVelocityDofList(rElementalDofList, rCurrentProcessInfo);
      break;
    }
    case 5:
    {
      this->GetPressureDofList(rElementalDofList, rCurrentProcessInfo);
      break;
    }
    case 6:
    {
      this->GetVelocityDofList(rElementalDofList, rCurrentProcessInfo);
      break;
    }
    default:
    {
      KRATOS_THROW_ERROR(std::logic_error, "Unexpected value for FRACTIONAL_STEP index: ", rCurrentProcessInfo[FRACTIONAL_STEP]);
    }
    }

    KRATOS_CATCH("");
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateFirstVelocitySystem(MatrixType &rLeftHandSideMatrix,
                                                                             VectorType &rRightHandSideVector,
                                                                             const ProcessInfo &rCurrentProcessInfo)
  {
    const GeometryType &rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();
    const SizeType LocalSize = TDim * NumNodes;

    // Check sizes and initialize
    if (rLeftHandSideMatrix.size1() != LocalSize)
      rLeftHandSideMatrix.resize(LocalSize, LocalSize, false);

    noalias(rLeftHandSideMatrix) = ZeroMatrix(LocalSize, LocalSize);

    if (rRightHandSideVector.size() != LocalSize)
      rRightHandSideVector.resize(LocalSize, false);

    noalias(rRightHandSideVector) = ZeroVector(LocalSize);

    // Shape functions and integration points
    ShapeFunctionDerivativesArrayType DN_DX;
    Matrix NContainer;
    VectorType GaussWeights;
    this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
    const unsigned int NumGauss = GaussWeights.size();

    MatrixType MassMatrix = ZeroMatrix(LocalSize, LocalSize);
    MatrixType ViscousMatrix = ZeroMatrix(LocalSize, LocalSize);

    const unsigned int schemeOrder = 2;
    double theta_velocity = 1.0;
    double gamma_pressure = 0.0;
    if (schemeOrder == 2)
    {
      theta_velocity = 0.5;
      gamma_pressure = 1.0;
    }

    // Loop on integration points
    for (unsigned int g = 0; g < NumGauss; g++)
    {
      const double GaussWeight = GaussWeights[g];
      const ShapeFunctionsType &rN = row(NContainer, g);
      const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

      // Evaluate required variables at the integration point
      double Density = 0.0;
      double Viscosity = 0.0;
      array_1d<double, 3> BodyForce = ZeroVector(3);

      this->EvaluateInPoint(Density, DENSITY, rN);
      this->EvaluateInPoint(BodyForce, VOLUME_ACCELERATION, rN);

      this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, rN);

      // Add integration point contribution to the local mass matrix
      const double MassCoeff = Density * GaussWeight;
      //AddMomentumMassTerm(MassMatrix, rN, MassCoeff);
      ComputeLumpedMassMatrix(MassMatrix, MassCoeff);

      // // Evaluate the pressure and pressure gradient at this point (for the G * P_n term)
      double OldPressure = 0.0;
      this->EvaluateInPoint(OldPressure, PRESSURE, rN, 1);
      double oldPressureContribution = OldPressure * gamma_pressure;
      // array_1d<double, TDim> OldPressureGradient = ZeroVector(TDim);
      // this->EvaluateGradientInPoint(OldPressureGradient, PRESSURE, rDN_DX);
      // double oldPressureGradientContribution = OldPressureGradient * gamma_pressure;

      // Add RHS contributions to the local system equation
      AddMomentumRHSTerms(rRightHandSideVector, Density, BodyForce, oldPressureContribution, rN, rDN_DX, GaussWeight);
      // AddExternalForces(rRightHandSideVector, Density, rN, GaussWeight);

      // Add viscous term
      const double ViscousCoeff = Viscosity * GaussWeight;
      this->AddViscousTerm(rLeftHandSideMatrix, rDN_DX, ViscousCoeff, theta_velocity);

      this->AddViscousTerm(ViscousMatrix, rDN_DX, ViscousCoeff, 1.0);
    }

    // Add residual of previous iteration to RHS
    VectorType VelocityValues = ZeroVector(LocalSize);
    this->GetVelocityValues(VelocityValues, 0);
    // noalias(rRightHandSideVector) -= prod(rLeftHandSideMatrix, VelocityValues);

    VectorType VelocityValuesTheta = ZeroVector(LocalSize);
    this->GetVelocityValues(VelocityValuesTheta, 1);
    VelocityValuesTheta *= (1.0 - theta_velocity);
    noalias(VelocityValuesTheta) += VelocityValues * theta_velocity;
    noalias(rRightHandSideVector) -= prod(ViscousMatrix, VelocityValuesTheta);

    // Add dynamic term
    const double TimeStep = rCurrentProcessInfo[DELTA_TIME];
    const unsigned int accelerationOrder = 1;

    if (accelerationOrder == 2)
    {
      // second order
      VectorType AccelerationValues = ZeroVector(LocalSize);
      this->GetAccelerationValues(AccelerationValues, 0);
      noalias(AccelerationValues) += -2.0 * VelocityValues / TimeStep;
      this->GetVelocityValues(VelocityValues, 1);
      noalias(AccelerationValues) += 2.0 * VelocityValues / TimeStep; //these are negative accelerations
      noalias(rRightHandSideVector) += prod(MassMatrix, AccelerationValues);
      noalias(rLeftHandSideMatrix) += MassMatrix * 2 / TimeStep;
    }
    else if (accelerationOrder == 1)
    {
      // // first order
      VectorType AccelerationValues = ZeroVector(LocalSize);
      noalias(AccelerationValues) += -VelocityValues / TimeStep;
      this->GetVelocityValues(VelocityValues, 1);
      noalias(AccelerationValues) += VelocityValues / TimeStep; //these are negative accelerations
      noalias(rRightHandSideVector) += prod(MassMatrix, AccelerationValues);
      noalias(rLeftHandSideMatrix) += MassMatrix / TimeStep;
    }

    // // using BDF coefficients
    // const Vector &rBDFCoeffs = rCurrentProcessInfo[BDF_COEFFICIENTS];
    // noalias(rLeftHandSideMatrix) += rBDFCoeffs[0] * MassMatrix;
    // VectorType TimeTerm = rBDFCoeffs[0] * LastValues;
    // for (SizeType i = 1; i < rBDFCoeffs.size(); i++)
    // {
    //   this->GetVelocityValues(LastValues, i);
    //   noalias(TimeTerm) += rBDFCoeffs[i] * LastValues;
    // }
    // noalias(rRightHandSideVector) -= prod(MassMatrix, TimeTerm);
  }

  // this is a part of the last iteration of the fractional step
  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::Calculate(const Variable<array_1d<double, 3>> &rVariable,
                                                          array_1d<double, 3> &rOutput,
                                                          const ProcessInfo &rCurrentProcessInfo)
  {
    if (rVariable == VELOCITY)
    {
      GeometryType &rGeom = this->GetGeometry();
      const SizeType NumNodes = rGeom.PointsNumber();
      const SizeType LocalSize = TDim * NumNodes;

      // Shape functions and integration points
      ShapeFunctionDerivativesArrayType DN_DX;
      Matrix NContainer;
      VectorType GaussWeights;
      this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
      const unsigned int NumGauss = GaussWeights.size();

      VectorType NodalVelCorrection = ZeroVector(LocalSize);

      // Loop on integration points
      for (unsigned int g = 0; g < NumGauss; ++g)
      {
        const ShapeFunctionsType &rN = row(NContainer, g);
        const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

        double Density = 0;
        this->EvaluateInPoint(Density, DENSITY, rN);

        const double TimeStep = rCurrentProcessInfo[DELTA_TIME];
        // double timeFactor = 1.0 / rCurrentProcessInfo[BDF_COEFFICIENTS][0];
        // double timeFactor = 0.5 * TimeStep; //second order accelerations
        double timeFactor = TimeStep; //first order accelerations
        const double Coeff = GaussWeights[g] * timeFactor / Density;

        // Calculate contribution to the gradient term (RHS)
        // double DeltaPressure;
        // this->EvaluateInPoint(DeltaPressure, PRESSURE_OLD_IT, N);

        const unsigned int schemeOrder = 2;
        double elementalPressure = 0;
        if (schemeOrder == 2)
        {
          this->EvaluateDifferenceInPoint(elementalPressure, PRESSURE, rN);
        }
        else
        {
          this->EvaluateInPoint(elementalPressure, PRESSURE, rN);
        }

        SizeType RowIndex = 0;

        for (SizeType i = 0; i < NumNodes; ++i)
        {
          for (SizeType d = 0; d < TDim; ++d)
          {
            // NodalVelCorrection[RowIndex++] += Coeff * rDN_DX(i, d) * DeltaPressure;
            NodalVelCorrection[RowIndex++] += Coeff * rDN_DX(i, d) * elementalPressure;
          }
        }
      }

      SizeType Index = 0;

      for (SizeType i = 0; i < NumNodes; ++i)
      {
        rGeom[i].SetLock(); // So it is safe to write in the node in OpenMP
        array_1d<double, 3> &rTemp = rGeom[i].FastGetSolutionStepValue(FRACT_VEL);
        for (SizeType d = 0; d < TDim; ++d)
        {
          rTemp[d] += NodalVelCorrection[Index++];
        }
        rGeom[i].UnSetLock(); // Free the node for other threads
      }
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::AddMomentumRHSTerms(Vector &rRHSVector,
                                                                    const double Density,
                                                                    const array_1d<double, 3> &rBodyForce,
                                                                    const double OldPressure,
                                                                    const ShapeFunctionsType &rN,
                                                                    const ShapeFunctionDerivativesType &rDN_DX,
                                                                    const double Weight)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();
    SizeType FirstRow = 0;

    for (SizeType i = 0; i < NumNodes; ++i)
    {
      // Build RHS

      for (SizeType d = 0; d < TDim; ++d)
      {
        // Body force
        double RHSi = Density * rN[i] * rBodyForce[d];
        // RHSi = 0;
        // Pressure gradient (integrated by parts)
        RHSi += rDN_DX(i, d) * OldPressure;
        rRHSVector[FirstRow + d] += Weight * RHSi;
      }
      FirstRow += TDim;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::AddExternalForces(Vector &rRHSVector,
                                                                  const double Density,
                                                                  const ShapeFunctionsType &rN,
                                                                  const double Weight)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    SizeType FirstRow = 0;

    array_1d<double, 3> VolumeAcceleration(3, 0.0);

    this->EvaluateInPoint(VolumeAcceleration, VOLUME_ACCELERATION, rN);

    for (SizeType i = 0; i < NumNodes; ++i)
    {
      if (this->GetGeometry()[i].SolutionStepsDataHas(VOLUME_ACCELERATION))
      { // it must be checked once at the begining only

        // // stored body force information for the closed domain test with analytical solution
        // double posX = (this->GetGeometry()[0].X() + this->GetGeometry()[1].X() + this->GetGeometry()[2].X()) / 3.0;
        // double posY = (this->GetGeometry()[0].Y() + this->GetGeometry()[1].Y() + this->GetGeometry()[2].Y()) / 3.0;
        // double coeffX = (12.0 - 24.0 * posY) * pow(posX, 4);
        // coeffX += (-24.0 + 48.0 * posY) * pow(posX, 3);
        // coeffX += (-48.0 * posY + 72.0 * pow(posY, 2) - 48.0 * pow(posY, 3) + 12.0) * pow(posX, 2);
        // coeffX += (-2.0 + 24.0 * posY - 72.0 * pow(posY, 2) + 48.0 * pow(posY, 3)) * posX;
        // coeffX += 1.0 - 4.0 * posY + 12.0 * pow(posY, 2) - 8.0 * pow(posY, 3);
        // double coeffY = (8.0 - 48.0 * posY + 48.0 * pow(posY, 2)) * pow(posX, 3);
        // coeffY += (-12.0 + 72.0 * posY - 72.0 * pow(posY, 2)) * pow(posX, 2);
        // coeffY += (4.0 - 24.0 * posY + 48.0 * pow(posY, 2) - 48.0 * pow(posY, 3) + 24.0 * pow(posY, 4)) * posX;
        // coeffY += -12.0 * pow(posY, 2) + 24.0 * pow(posY, 3) - 12.0 * pow(posY, 4);
        // rRHSVector[FirstRow] += Weight * Density * rN[i] * VolumeAcceleration[0] * coeffX;
        // rRHSVector[FirstRow + 1] += Weight * Density * rN[i] * VolumeAcceleration[1] * coeffY;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (SizeType d = 0; d < TDim; ++d)
        {
          // Volume Acceleration
          rRHSVector[FirstRow + d] += Weight * Density * rN[i] * VolumeAcceleration[d];
        }
      }
      FirstRow += TDim;
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::AddViscousTerm(MatrixType &rDampingMatrix,
                                                            const ShapeFunctionDerivativesType &rShapeDeriv,
                                                            const double Weight,
                                                            const double theta)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    const double FourThirds = 4.0 / 3.0;
    const double nTwoThirds = -2.0 / 3.0;

    SizeType FirstRow(0), FirstCol(0);

    for (SizeType j = 0; j < NumNodes; ++j)
    {
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        // First Row
        rDampingMatrix(FirstRow, FirstCol) += Weight * (FourThirds * rShapeDeriv(i, 0) * rShapeDeriv(j, 0) + rShapeDeriv(i, 1) * rShapeDeriv(j, 1)) * theta;
        rDampingMatrix(FirstRow, FirstCol + 1) += Weight * (nTwoThirds * rShapeDeriv(i, 0) * rShapeDeriv(j, 1) + rShapeDeriv(i, 1) * rShapeDeriv(j, 0)) * theta;

        // Second Row
        rDampingMatrix(FirstRow + 1, FirstCol) += Weight * (nTwoThirds * rShapeDeriv(i, 1) * rShapeDeriv(j, 0) + rShapeDeriv(i, 0) * rShapeDeriv(j, 1)) * theta;
        rDampingMatrix(FirstRow + 1, FirstCol + 1) += Weight * (FourThirds * rShapeDeriv(i, 1) * rShapeDeriv(j, 1) + rShapeDeriv(i, 0) * rShapeDeriv(j, 0)) * theta;

        // Update Counter
        FirstRow += 2;
      }
      FirstRow = 0;
      FirstCol += 2;
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::AddViscousTerm(MatrixType &rDampingMatrix,
                                                            const ShapeFunctionDerivativesType &rShapeDeriv,
                                                            const double Weight,
                                                            const double theta)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    const double OneThird = 1.0 / 3.0;
    const double nTwoThirds = -2.0 / 3.0;

    unsigned int FirstRow(0), FirstCol(0);

    for (SizeType j = 0; j < NumNodes; ++j)
    {
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        // (dN_i/dx_k dN_j/dx_k)
        const double Diag = rShapeDeriv(i, 0) * rShapeDeriv(j, 0) + rShapeDeriv(i, 1) * rShapeDeriv(j, 1) + rShapeDeriv(i, 2) * rShapeDeriv(j, 2);

        // First Row
        rDampingMatrix(FirstRow, FirstCol) += Weight * (OneThird * rShapeDeriv(i, 0) * rShapeDeriv(j, 0) + Diag) * theta;
        rDampingMatrix(FirstRow, FirstCol + 1) += Weight * (nTwoThirds * rShapeDeriv(i, 0) * rShapeDeriv(j, 1) + rShapeDeriv(i, 1) * rShapeDeriv(j, 0)) * theta;
        rDampingMatrix(FirstRow, FirstCol + 2) += Weight * (nTwoThirds * rShapeDeriv(i, 0) * rShapeDeriv(j, 2) + rShapeDeriv(i, 2) * rShapeDeriv(j, 0)) * theta;

        // Second Row
        rDampingMatrix(FirstRow + 1, FirstCol) += Weight * (nTwoThirds * rShapeDeriv(i, 1) * rShapeDeriv(j, 0) + rShapeDeriv(i, 0) * rShapeDeriv(j, 1)) * theta;
        rDampingMatrix(FirstRow + 1, FirstCol + 1) += Weight * (OneThird * rShapeDeriv(i, 1) * rShapeDeriv(j, 1) + Diag) * theta;
        rDampingMatrix(FirstRow + 1, FirstCol + 2) += Weight * (nTwoThirds * rShapeDeriv(i, 1) * rShapeDeriv(j, 2) + rShapeDeriv(i, 2) * rShapeDeriv(j, 1)) * theta;

        // Third Row
        rDampingMatrix(FirstRow + 2, FirstCol) += Weight * (nTwoThirds * rShapeDeriv(i, 2) * rShapeDeriv(j, 0) + rShapeDeriv(i, 0) * rShapeDeriv(j, 2)) * theta;
        rDampingMatrix(FirstRow + 2, FirstCol + 1) += Weight * (nTwoThirds * rShapeDeriv(i, 2) * rShapeDeriv(j, 1) + rShapeDeriv(i, 1) * rShapeDeriv(j, 2)) * theta;
        rDampingMatrix(FirstRow + 2, FirstCol + 2) += Weight * (OneThird * rShapeDeriv(i, 2) * rShapeDeriv(j, 2) + Diag) * theta;

        // Update Counter
        FirstRow += 3;
      }
      FirstRow = 0;
      FirstCol += 3;
    }
  }

  template <unsigned int TDim>
  double ThreeStepUpdatedLagrangianElement<TDim>::ElementSize()
  {
    const GeometryType &rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();

    // calculate minimum element length (used in stabilization Tau)
    array_1d<double, 3> Edge = rGeom[1].Coordinates() - rGeom[0].Coordinates();
    double ElemSize = Edge[0] * Edge[0];
    for (SizeType d = 1; d < TDim; d++)
      ElemSize += Edge[d] * Edge[d];

    for (SizeType i = 2; i < NumNodes; i++)
      for (SizeType j = 0; j < i; j++)
      {
        Edge = rGeom[i].Coordinates() - rGeom[j].Coordinates();
        double Length = Edge[0] * Edge[0];
        for (SizeType d = 1; d < TDim; d++)
          Length += Edge[d] * Edge[d];
        if (Length < ElemSize)
          ElemSize = Length;
      }
    return sqrt(ElemSize);
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::ComputeLumpedMassMatrix(Matrix &rMassMatrix,
                                                                        const double Weight)
  {

    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    if ((NumNodes == 3 && TDim == 2) || (NumNodes == 4 && TDim == 3))
    {
      double Coeff = 1.0 + TDim;
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        const double Mij = Weight / Coeff;

        for (unsigned int j = 0; j < TDim; j++)
        {
          unsigned int index = i * TDim + j;
          rMassMatrix(index, index) += Mij;
        }
      }
    }
    else if (NumNodes == 6 && TDim == 2)
    {
      const double Mij = Weight / 57.0;
      double consistent = 1.0;
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        if (i < 3)
        {
          consistent = 3.0;
        }
        else
        {
          consistent = 16.0;
        }
        for (unsigned int j = 0; j < TDim; j++)
        {
          unsigned int index = i * TDim + j;
          rMassMatrix(index, index) += Mij * consistent;
        }
      }
    }
    else
    {
      std::cout << "ComputeLumpedMassMatrix 3D quadratic not yet implemented!" << std::endl;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::AddMomentumMassTerm(Matrix &rMassMatrix,
                                                                    const ShapeFunctionsType &rN,
                                                                    const double Weight)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    IndexType FirstRow = 0;
    IndexType FirstCol = 0;

    for (SizeType i = 0; i < NumNodes; ++i)
    {
      for (SizeType j = 0; j < NumNodes; ++j)
      {
        const double Mij = Weight * rN[i] * rN[j];
        for (SizeType d = 0; d < TDim; ++d)
          rMassMatrix(FirstRow + d, FirstCol + d) += Mij;
        FirstCol += TDim;
      }
      FirstRow += TDim;
      FirstCol = 0;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateStandardFSPressureSystem(MatrixType &rLeftHandSideMatrix,
                                                                                  VectorType &rRightHandSideVector,
                                                                                  const ProcessInfo &rCurrentProcessInfo)
  {
    GeometryType &rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();
    const double TimeStep = rCurrentProcessInfo[DELTA_TIME];

    // Check sizes and initialize
    if (rLeftHandSideMatrix.size1() != NumNodes)
      rLeftHandSideMatrix.resize(NumNodes, NumNodes, false);

    noalias(rLeftHandSideMatrix) = ZeroMatrix(NumNodes, NumNodes);

    if (rRightHandSideVector.size() != NumNodes)
      rRightHandSideVector.resize(NumNodes, false);

    noalias(rRightHandSideVector) = ZeroVector(NumNodes);

    // Shape functions and integration points
    ShapeFunctionDerivativesArrayType DN_DX;
    Matrix NContainer;
    VectorType GaussWeights;
    this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
    const unsigned int NumGauss = GaussWeights.size();

    // Loop on integration points
    for (unsigned int g = 0; g < NumGauss; g++)
    {
      const double GaussWeight = GaussWeights[g];
      const ShapeFunctionsType &rN = row(NContainer, g);
      const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

      // Evaluate required variables at the integration point
      double Density;
      array_1d<double, 3> BodyForce = ZeroVector(3);

      this->EvaluateInPoint(Density, DENSITY, rN);
      this->EvaluateInPoint(BodyForce, VOLUME_ACCELERATION, rN);

      const unsigned int schemeOrder = 2;
      array_1d<double, TDim> OldPressureGradient = ZeroVector(TDim);
      if (schemeOrder == 2)
      {
        this->EvaluateGradientDifferenceInPoint(OldPressureGradient, PRESSURE, rDN_DX);
      }
      else
      {
        this->EvaluateGradientInPoint(OldPressureGradient, PRESSURE, rDN_DX);
      }

      double Viscosity = 0;
      this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, rN);

      double DivU = 0;
      this->EvaluateDivergenceInPoint(DivU, VELOCITY, rDN_DX);

      const double LaplacianCoeff = TimeStep / Density;

      // Add convection, stabilization and RHS contributions to the local system equation
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        // LHS contribution
        for (SizeType j = 0; j < NumNodes; ++j)
        {
          double Lij = 0.0;
          for (SizeType d = 0; d < TDim; ++d)
            Lij += rDN_DX(i, d) * rDN_DX(j, d);

          Lij *= LaplacianCoeff;
          rLeftHandSideMatrix(i, j) += GaussWeight * Lij;
        }

        // RHS contribution
        // Velocity divergence
        double RHSi = -rN[i] * DivU;

        for (SizeType d = 0; d < TDim; ++d)
        {
          RHSi += -LaplacianCoeff * rDN_DX(i, d) * OldPressureGradient[d];
        }
        rRightHandSideVector[i] += GaussWeight * RHSi;
      }
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateFSplusPSPGPressureSystem(MatrixType &rLeftHandSideMatrix,
                                                                                  VectorType &rRightHandSideVector,
                                                                                  const ProcessInfo &rCurrentProcessInfo)
  {

    GeometryType &rGeom = this->GetGeometry();
    const unsigned int NumNodes = rGeom.PointsNumber();

    // Check sizes and initialize
    if (rLeftHandSideMatrix.size1() != NumNodes)
      rLeftHandSideMatrix.resize(NumNodes, NumNodes, false);

    noalias(rLeftHandSideMatrix) = ZeroMatrix(NumNodes, NumNodes);

    if (rRightHandSideVector.size() != NumNodes)
      rRightHandSideVector.resize(NumNodes, false);

    noalias(rRightHandSideVector) = ZeroVector(NumNodes);

    // Shape functions and integration points
    ShapeFunctionDerivativesArrayType DN_DX;
    Matrix NContainer;
    VectorType GaussWeights;
    this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
    const unsigned int NumGauss = GaussWeights.size();

    double TimeStep = rCurrentProcessInfo[DELTA_TIME];
    double ElemSize = this->ElementSize();

    double Viscosity = 0;
    double Density = 0;
    double totalVolume = 0;

    MatrixType DynamicStabilizationMatrix = ZeroMatrix(NumNodes, NumNodes);

    // ElementalVariables rElementalVariables;
    // this->InitializeElementalVariables(rElementalVariables);

    // Loop on integration points
    for (unsigned int g = 0; g < NumGauss; ++g)
    {
      const double GaussWeight = GaussWeights[g];
      totalVolume += GaussWeight;
      const ShapeFunctionsType &rN = row(NContainer, g);
      const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

      this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, rN);
      this->EvaluateInPoint(Density, DENSITY, rN);
      double PSPGweight = 0.1;
      double Tau = 0;
      this->CalculateTauPSPG(Tau, ElemSize, Density, Viscosity, rCurrentProcessInfo);

      double StabilizedWeight = Tau * GaussWeight;

      const unsigned int schemeOrder = 2;
      array_1d<double, TDim> OldPressureGradient = ZeroVector(TDim);
      if (schemeOrder == 2)
      {
        this->EvaluateGradientDifferenceInPoint(OldPressureGradient, PRESSURE, rDN_DX, PSPGweight);
      }
      else
      {
        this->EvaluateGradientInPoint(OldPressureGradient, PRESSURE, rDN_DX, 0);
      }

      double DivU = 0;
      this->EvaluateDivergenceInPoint(DivU, VELOCITY, rDN_DX);

      for (SizeType i = 0; i < NumNodes; ++i)
      {
        // LHS contribution
        double dynamicRHSi = 0;
        unsigned int freeSurfaceNodes = 0;
        for (SizeType j = 0; j < NumNodes; ++j)
        {
          double Lij = 0.0;
          for (SizeType d = 0; d < TDim; ++d)
            Lij += (1.0 + PSPGweight) * rDN_DX(i, d) * rDN_DX(j, d);

          Lij *= StabilizedWeight;
          rLeftHandSideMatrix(i, j) += Lij;

          dynamicRHSi += rDN_DX(i, 0) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 1)) / TimeStep;
          dynamicRHSi += rDN_DX(i, 1) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 1)) / TimeStep;
          if (TDim == 3)
          {
            dynamicRHSi += rDN_DX(i, 2) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Z, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Z, 1)) / TimeStep;
          }
          if (rGeom[j].Is(FREE_SURFACE) && rGeom[j].IsNot(INLET))
          {
            freeSurfaceNodes++;
          }
        }

        if (freeSurfaceNodes >= TDim)
        {
          // bool computeElement = this->CalcCompleteStrainRate(rElementalVariables, rCurrentProcessInfo, rDN_DX, 1.0);
          // VectorType deviatoricSpatialDefRate = rElementalVariables.SpatialDefRate;
          // deviatoricSpatialDefRate[0] -= DivU / 3.0;
          // deviatoricSpatialDefRate[1] -= DivU / 3.0;
          const double lagMultiplier = TimeStep / (ElemSize * ElemSize * Density);
          ComputeBoundaryTermsForPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rN, lagMultiplier);
        }

        // RHS contribution
        rRightHandSideVector[i] += -PSPGweight * GaussWeight * Tau * Density * dynamicRHSi;

        rRightHandSideVector[i] += -(1.0 + PSPGweight) * GaussWeight * rN[i] * DivU;

        double laplacianRHSi = 0;
        double bodyForceStabilizedRHSi = 0;
        array_1d<double, 3> VolumeAcceleration = this->GetGeometry()[i].FastGetSolutionStepValue(VOLUME_ACCELERATION);

        // // stored body force information for the closed domain test with analytical solution
        // array_1d<double, 3> VolumeAcceleration = this->GetGeometry()[i].FastGetSolutionStepValue(VOLUME_ACCELERATION);
        // double posX = (this->GetGeometry()[0].X() + this->GetGeometry()[1].X() + this->GetGeometry()[2].X()) / 3.0;
        // double posY = (this->GetGeometry()[0].Y() + this->GetGeometry()[1].Y() + this->GetGeometry()[2].Y()) / 3.0;
        // double coeffX = (12.0 - 24.0 * posY) * pow(posX, 4);
        // coeffX += (-24.0 + 48.0 * posY) * pow(posX, 3);
        // coeffX += (-48.0 * posY + 72.0 * pow(posY, 2) - 48.0 * pow(posY, 3) + 12.0) * pow(posX, 2);
        // coeffX += (-2.0 + 24.0 * posY - 72.0 * pow(posY, 2) + 48.0 * pow(posY, 3)) * posX;
        // coeffX += 1.0 - 4.0 * posY + 12.0 * pow(posY, 2) - 8.0 * pow(posY, 3);
        // double coeffY = (8.0 - 48.0 * posY + 48.0 * pow(posY, 2)) * pow(posX, 3);
        // coeffY += (-12.0 + 72.0 * posY - 72.0 * pow(posY, 2)) * pow(posX, 2);
        // coeffY += (4.0 - 24.0 * posY + 48.0 * pow(posY, 2) - 48.0 * pow(posY, 3) + 24.0 * pow(posY, 4)) * posX;
        // coeffY += -12.0 * pow(posY, 2) + 24.0 * pow(posY, 3) - 12.0 * pow(posY, 4);
        // VolumeAcceleration[0] *= coeffX;
        // VolumeAcceleration[1] *= coeffY;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (SizeType d = 0; d < TDim; ++d)
        {
          laplacianRHSi += -StabilizedWeight * rDN_DX(i, d) * OldPressureGradient[d];

          bodyForceStabilizedRHSi += PSPGweight * StabilizedWeight * rDN_DX(i, d) * (Density * VolumeAcceleration[d]);
        }
        rRightHandSideVector[i] += laplacianRHSi + bodyForceStabilizedRHSi;
      }
    }
  }

  // template <unsigned int TDim>
  // void ThreeStepUpdatedLagrangianElement<TDim>::CalculatePSPGPressureSystem(MatrixType &rLeftHandSideMatrix,
  //                                                                           VectorType &rRightHandSideVector,
  //                                                                           const ProcessInfo &rCurrentProcessInfo)
  // {

  //   GeometryType &rGeom = this->GetGeometry();
  //   const unsigned int NumNodes = rGeom.PointsNumber();

  //   // Check sizes and initialize
  //   if (rLeftHandSideMatrix.size1() != NumNodes)
  //     rLeftHandSideMatrix.resize(NumNodes, NumNodes, false);

  //   noalias(rLeftHandSideMatrix) = ZeroMatrix(NumNodes, NumNodes);

  //   if (rRightHandSideVector.size() != NumNodes)
  //     rRightHandSideVector.resize(NumNodes, false);

  //   noalias(rRightHandSideVector) = ZeroVector(NumNodes);

  //   // Shape functions and integration points
  //   ShapeFunctionDerivativesArrayType DN_DX;
  //   Matrix NContainer;
  //   VectorType GaussWeights;
  //   this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
  //   const unsigned int NumGauss = GaussWeights.size();

  //   double TimeStep = rCurrentProcessInfo[DELTA_TIME];
  //   double ElemSize = this->ElementSize();

  //   double Viscosity = 0;
  //   double Density = 0;
  //   double totalVolume = 0;

  //   MatrixType DynamicStabilizationMatrix = ZeroMatrix(NumNodes, NumNodes);

  //   ElementalVariables rElementalVariables;
  //   this->InitializeElementalVariables(rElementalVariables);

  //   // Loop on integration points
  //   for (unsigned int g = 0; g < NumGauss; ++g)
  //   {
  //     const double GaussWeight = GaussWeights[g];
  //     totalVolume += GaussWeight;
  //     const ShapeFunctionsType &rN = row(NContainer, g);
  //     const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

  //     this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, rN);
  //     this->EvaluateInPoint(Density, DENSITY, rN);
  //     double PSPGweight = 1.0;
  //     double Tau = 0;
  //     this->CalculateTauPSPG(Tau, ElemSize, Density, Viscosity, rCurrentProcessInfo);

  //     double StabilizedWeight = Tau * GaussWeight;

  //     array_1d<double, TDim> OldPressureGradient = ZeroVector(TDim);
  //     this->EvaluateGradientInPoint(OldPressureGradient, PRESSURE, rDN_DX, 0);

  //     double DivU = 0;
  //     this->EvaluateDivergenceInPoint(DivU, VELOCITY, rDN_DX);

  //     for (SizeType i = 0; i < NumNodes; ++i)
  //     {
  //       // LHS contribution
  //       double dynamicRHSi = 0;
  //       unsigned int freeSurfaceNodes = 0;
  //       for (SizeType j = 0; j < NumNodes; ++j)
  //       {
  //         double Lij = 0.0;
  //         for (SizeType d = 0; d < TDim; ++d)
  //           Lij += PSPGweight * rDN_DX(i, d) * rDN_DX(j, d);

  //         Lij *= StabilizedWeight;
  //         rLeftHandSideMatrix(i, j) += Lij;

  //         dynamicRHSi += rDN_DX(i, 0) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 1)) / TimeStep;
  //         dynamicRHSi += rDN_DX(i, 1) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 1)) / TimeStep;
  //         if (rGeom[j].Is(FREE_SURFACE) && rGeom[j].IsNot(INLET))
  //         {
  //           freeSurfaceNodes++;
  //         }
  //       }

  //       if (freeSurfaceNodes >= TDim)
  //       {
  //         // bool computeElement = this->CalcCompleteStrainRate(rElementalVariables, rCurrentProcessInfo, rDN_DX, 1.0);
  //         // VectorType deviatoricSpatialDefRate = rElementalVariables.SpatialDefRate;
  //         // deviatoricSpatialDefRate[0] -= DivU / 3.0;
  //         // deviatoricSpatialDefRate[1] -= DivU / 3.0;
  //         const double lagMultiplier = TimeStep / (ElemSize * ElemSize * Density);
  //         ComputeBoundaryTermsForPressureSystem(rLeftHandSideMatrix, rRightHandSideVector, rN, lagMultiplier);
  //       }

  //       // RHS contribution
  //       rRightHandSideVector[i] += -PSPGweight * GaussWeight * Tau * Density * dynamicRHSi;

  //       rRightHandSideVector[i] += -PSPGweight * GaussWeight * rN[i] * DivU;

  //       // this->AddPspgDynamicPartStabilization(rRightHandSideVector, Tau, Density, GaussWeight, TimeStep, rDN_DX, rN, i);

  //       double laplacianRHSi = 0;
  //       double bodyForceStabilizedRHSi = 0;
  //       array_1d<double, 3> VolumeAcceleration = this->GetGeometry()[i].FastGetSolutionStepValue(VOLUME_ACCELERATION);

  //       for (SizeType d = 0; d < TDim; ++d)
  //       {
  //         laplacianRHSi += -StabilizedWeight * rDN_DX(i, d) * OldPressureGradient[d];

  //         bodyForceStabilizedRHSi += PSPGweight * StabilizedWeight * rDN_DX(i, d) * (Density * VolumeAcceleration[d]);
  //       }
  //       rRightHandSideVector[i] += laplacianRHSi + bodyForceStabilizedRHSi;
  //     }
  //   }
  // }

  // template <unsigned int TDim>
  // void ThreeStepUpdatedLagrangianElement<TDim>::CalculateFICPressureSystem(MatrixType &rLeftHandSideMatrix,
  //                                                                          VectorType &rRightHandSideVector,
  //                                                                          const ProcessInfo &rCurrentProcessInfo)
  // {

  //   GeometryType &rGeom = this->GetGeometry();
  //   const unsigned int NumNodes = rGeom.PointsNumber();

  //   // Check sizes and initialize
  //   if (rLeftHandSideMatrix.size1() != NumNodes)
  //     rLeftHandSideMatrix.resize(NumNodes, NumNodes, false);

  //   noalias(rLeftHandSideMatrix) = ZeroMatrix(NumNodes, NumNodes);
  //   MatrixType LaplacianMatrix = ZeroMatrix(NumNodes, NumNodes);

  //   if (rRightHandSideVector.size() != NumNodes)
  //     rRightHandSideVector.resize(NumNodes, false);

  //   noalias(rRightHandSideVector) = ZeroVector(NumNodes);

  //   // Shape functions and integration points
  //   ShapeFunctionDerivativesArrayType DN_DX;
  //   Matrix NContainer;
  //   VectorType GaussWeights;
  //   this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
  //   const unsigned int NumGauss = GaussWeights.size();

  //   double TimeStep = rCurrentProcessInfo[DELTA_TIME];
  //   double ElemSize = this->ElementSize();

  //   double Viscosity = 0;
  //   double Density = 0;
  //   double totalVolume = 0;

  //   double maxViscousValueForStabilization = 0.001;

  //   ElementalVariables rElementalVariables;
  //   this->InitializeElementalVariables(rElementalVariables);

  //   VectorType NewRhsLaplacian = ZeroVector(NumNodes);

  //   // Loop on integration points
  //   for (unsigned int g = 0; g < NumGauss; ++g)
  //   {
  //     const double GaussWeight = GaussWeights[g];
  //     totalVolume += GaussWeight;
  //     const ShapeFunctionsType &rN = row(NContainer, g);
  //     const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

  //     this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, rN);
  //     this->EvaluateInPoint(Density, DENSITY, rN);

  //     if (Viscosity > maxViscousValueForStabilization)
  //     {
  //       Viscosity = maxViscousValueForStabilization;
  //     }

  //     double Tau = 0;
  //     this->CalculateTauFIC(Tau, ElemSize, Density, Viscosity, rCurrentProcessInfo);

  //     double StabLaplacianWeight = Tau * GaussWeight;

  //     array_1d<double, TDim> OldPressureGradient = ZeroVector(TDim);
  //     this->EvaluateGradientInPoint(OldPressureGradient, PRESSURE, rDN_DX, 0);

  //     double DivU = 0;
  //     this->EvaluateDivergenceInPoint(DivU, VELOCITY, rDN_DX);

  //     bool computeElement = this->CalcCompleteStrainRate(rElementalVariables, rCurrentProcessInfo, rDN_DX, 1.0);

  //     double BoundLHSCoeff = 4.0 * StabLaplacianWeight / (ElemSize * ElemSize);
  //     this->ComputeBoundLHSMatrix(rLeftHandSideMatrix, rN, BoundLHSCoeff);

  //     double BoundRHSCoeffAcc = -2.0 * StabLaplacianWeight * Density / ElemSize;
  //     double BoundRHSCoeffDev = -8.0 * StabLaplacianWeight * Viscosity / (ElemSize * ElemSize);

  //     this->ComputeBoundRHSVectorComplete(rRightHandSideVector, TimeStep, BoundRHSCoeffAcc, BoundRHSCoeffDev, rElementalVariables.SpatialDefRate);

  //     this->ComputeStabLaplacianMatrix(LaplacianMatrix, rDN_DX, StabLaplacianWeight);

  //     for (SizeType i = 0; i < NumNodes; ++i)
  //     {
  //       // RHS contribution
  //       // Velocity divergence
  //       rRightHandSideVector[i] += -GaussWeight * rN[i] * DivU;

  //       double laplacianRHSi = 0;
  //       double bodyForceStabilizedRHSi = 0;
  //       array_1d<double, 3> VolumeAcceleration = this->GetGeometry()[i].FastGetSolutionStepValue(VOLUME_ACCELERATION);

  //       for (SizeType d = 0; d < TDim; ++d)
  //       {
  //         laplacianRHSi += -StabLaplacianWeight * rDN_DX(i, d) * OldPressureGradient[d];

  //         bodyForceStabilizedRHSi += StabLaplacianWeight * rDN_DX(i, d) * (Density * VolumeAcceleration[d]);
  //       }
  //       rRightHandSideVector[i] += laplacianRHSi + bodyForceStabilizedRHSi;
  //     }
  //   }

  //   VectorType PressureValues = ZeroVector(NumNodes);
  //   VectorType PressureValuesForRHS = ZeroVector(NumNodes);
  //   this->GetPressureValues(PressureValuesForRHS, 0);
  //   //the LHS matrix up to now just contains the laplacian term and the bound term
  //   noalias(rRightHandSideVector) -= prod(rLeftHandSideMatrix, PressureValuesForRHS);
  //   noalias(rLeftHandSideMatrix) += LaplacianMatrix;
  // }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::AddStabilizationNodalTermsRHS(VectorType &rRightHandSideVector,
                                                                              const double Tau,
                                                                              const double Density,
                                                                              const double Weight,
                                                                              const ShapeFunctionDerivativesType &rDN_DX,
                                                                              const SizeType i)
  {

    double RHSi = 0;
    if (this->GetGeometry()[i].SolutionStepsDataHas(VOLUME_ACCELERATION))
    { // it must be checked once at the begining only
      array_1d<double, 3> &VolumeAcceleration = this->GetGeometry()[i].FastGetSolutionStepValue(VOLUME_ACCELERATION);

      for (SizeType d = 0; d < TDim; ++d)
      {
        RHSi += +rDN_DX(i, d) * Tau * (Density * VolumeAcceleration[d]);
      }
    }
    rRightHandSideVector[i] += Weight * RHSi;
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::ComputeBoundaryTermForPressure(VectorType &BoundRHSVector,
                                                                            const double TimeStep,
                                                                            const double BoundRHSCoeff,
                                                                            const VectorType SpatialDefRate)
  {
    GeometryType &rGeom = this->GetGeometry();
    const double coeff = 1.0 / 3.0;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE))
    {
      // array_1d<double, 3> NormalVector(3, 0.0);
      // this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 1, 2);
      // double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);
      // BoundRHSVector[0] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;
      // BoundRHSVector[1] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;

      if (rGeom[0].IsNot(INLET)) //to change into moving wall!!!!!
        BoundRHSVector[0] += coeff * BoundRHSCoeff * rGeom[0].FastGetSolutionStepValue(PRESSURE, 0);

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * BoundRHSCoeff * rGeom[1].FastGetSolutionStepValue(PRESSURE, 0);
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {
      // array_1d<double, 3> NormalVector(3, 0.0);
      // this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 2, 1);
      // double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);
      // BoundRHSVector[0] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;
      // BoundRHSVector[2] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;

      if (rGeom[0].IsNot(INLET)) //to change into moving wall!!!!!
        BoundRHSVector[0] += coeff * BoundRHSCoeff * rGeom[0].FastGetSolutionStepValue(PRESSURE, 0);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * BoundRHSCoeff * rGeom[2].FastGetSolutionStepValue(PRESSURE, 0);
    }

    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {
      // array_1d<double, 3> NormalVector(3, 0.0);
      // this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 1, 2, 0);
      // double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);
      // BoundRHSVector[1] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;
      // BoundRHSVector[2] += coeff * BoundRHSCoeff * SpatialDefRateNormalProjection * 0.5;

      if (rGeom[1].IsNot(INLET)) //to change into moving wall!!!!!
        BoundRHSVector[1] += coeff * BoundRHSCoeff * rGeom[1].FastGetSolutionStepValue(PRESSURE, 0);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * BoundRHSCoeff * rGeom[2].FastGetSolutionStepValue(PRESSURE, 0);
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::ComputeBoundaryTermForPressure(VectorType &BoundRHSVector,
                                                                            const double TimeStep,
                                                                            const double BoundRHSCoeff,
                                                                            const VectorType SpatialDefRate)
  {
    std::cout << "!!! ComputeBoundaryTermForPressure in 3D TO DO " << std::endl;
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::ComputeBoundRHSVectorComplete(VectorType &BoundRHSVector,
                                                                           const double TimeStep,
                                                                           const double BoundRHSCoeffAcc,
                                                                           const double BoundRHSCoeffDev,
                                                                           const VectorType SpatialDefRate)
  {
    GeometryType &rGeom = this->GetGeometry();
    const double coeff = 1.0 / 3.0;
    // const double timeFactor = 0.5 / TimeStep;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE))
    {
      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 1, 2);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      // noalias(AccA) = (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      // noalias(AccB) = (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      noalias(AccA) = 0.0 * rGeom[0].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = 0.0 * rGeom[1].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccA) = timeFactor * (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccB) = timeFactor * (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(MeanAcc) = -0.5 * (AccA + AccB);

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1];

      if (rGeom[0].IsNot(INLET)) //to change into moving wall!!!!!
        BoundRHSVector[0] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 2, 1);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      // noalias(AccA) = (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      // noalias(AccB) = (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      noalias(AccA) = 0.0 * rGeom[0].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = 0.0 * rGeom[2].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccA) = timeFactor * (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccB) = timeFactor * (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(MeanAcc) = -0.5 * (AccA + AccB);

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1];

      if (rGeom[0].IsNot(INLET)) //to change into moving wall!!!!!
        BoundRHSVector[0] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }

    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 1, 2, 0);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      // noalias(AccA) = (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      // noalias(AccB) = (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) / TimeStep;
      noalias(AccA) = 0.0 * rGeom[1].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = 0.0 * rGeom[2].FastGetSolutionStepValue(ACCELERATION, 0) + 1.0 * rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccA) = timeFactor * (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      // noalias(AccB) = timeFactor * (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(MeanAcc) = -0.5 * (AccA + AccB);

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1];

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::ComputeBoundRHSVectorComplete(VectorType &BoundRHSVector,
                                                                           const double TimeStep,
                                                                           const double BoundRHSCoeffAcc,
                                                                           const double BoundRHSCoeffDev,
                                                                           const VectorType SpatialDefRate)
  {
    GeometryType &rGeom = this->GetGeometry();
    const double coeff = 0.25;
    const double timeFactor = 0.5 / TimeStep;
    const double one_third = 1.0 / 3.0;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> AccC(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 1, 2, 3);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      noalias(AccA) = timeFactor * (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = timeFactor * (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccC) = timeFactor * (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);

      noalias(MeanAcc) = (AccA + AccB + AccC) * one_third;

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1] + MeanAcc[2] * NormalVector[2];

      if (rGeom[0].IsNot(INLET))
        BoundRHSVector[0] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> AccC(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 1, 3, 2);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      noalias(AccA) = timeFactor * (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = timeFactor * (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccC) = timeFactor * (rGeom[3].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[3].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[3].FastGetSolutionStepValue(ACCELERATION, 1);

      noalias(MeanAcc) = (AccA + AccB + AccC) * one_third;

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1] + MeanAcc[2] * NormalVector[2];

      if (rGeom[0].IsNot(INLET))
        BoundRHSVector[0] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[3].IsNot(INLET))
        BoundRHSVector[3] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> AccC(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 2, 3, 1);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      noalias(AccA) = timeFactor * (rGeom[0].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[0].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[0].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = timeFactor * (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccC) = timeFactor * (rGeom[3].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[3].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[3].FastGetSolutionStepValue(ACCELERATION, 1);

      noalias(MeanAcc) = (AccA + AccB + AccC) * one_third;

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1] + MeanAcc[2] * NormalVector[2];

      if (rGeom[0].IsNot(INLET))
        BoundRHSVector[0] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[3].IsNot(INLET))
        BoundRHSVector[3] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }

    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {

      array_1d<double, 3> AccA(3, 0.0);
      array_1d<double, 3> AccB(3, 0.0);
      array_1d<double, 3> AccC(3, 0.0);
      array_1d<double, 3> MeanAcc(3, 0.0);
      array_1d<double, 3> NormalVector(3, 0.0);

      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 1, 2, 3, 0);

      double SpatialDefRateNormalProjection = this->CalcNormalProjectionDefRate(SpatialDefRate, NormalVector);

      noalias(AccA) = timeFactor * (rGeom[1].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[1].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[1].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccB) = timeFactor * (rGeom[2].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[2].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[2].FastGetSolutionStepValue(ACCELERATION, 1);
      noalias(AccC) = timeFactor * (rGeom[3].FastGetSolutionStepValue(VELOCITY, 0) - rGeom[3].FastGetSolutionStepValue(VELOCITY, 1)) - rGeom[3].FastGetSolutionStepValue(ACCELERATION, 1);

      noalias(MeanAcc) = (AccA + AccB + AccC) * one_third;

      const double accelerationsNormalProjection = MeanAcc[0] * NormalVector[0] + MeanAcc[1] * NormalVector[1] + MeanAcc[2] * NormalVector[2];

      if (rGeom[1].IsNot(INLET))
        BoundRHSVector[1] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[2].IsNot(INLET))
        BoundRHSVector[2] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);

      if (rGeom[3].IsNot(INLET))
        BoundRHSVector[3] += coeff * (BoundRHSCoeffAcc * accelerationsNormalProjection + BoundRHSCoeffDev * SpatialDefRateNormalProjection);
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::ComputeBoundLHSMatrix(Matrix &BoundLHSMatrix,
                                                                   const ShapeFunctionsType &rN,
                                                                   const double Weight)

  {
    GeometryType &rGeom = this->GetGeometry();
    double coeff = 1.0 / 3.0;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE))
    {
      if (rGeom[0].IsNot(INLET))
        BoundLHSMatrix(0, 0) += Weight * coeff;
      if (rGeom[1].IsNot(INLET))
        BoundLHSMatrix(1, 1) += Weight * coeff;
    }
    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {
      if (rGeom[0].IsNot(INLET))
        BoundLHSMatrix(0, 0) += Weight * coeff;
      if (rGeom[2].IsNot(INLET))
        BoundLHSMatrix(2, 2) += Weight * coeff;
    }
    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {
      if (rGeom[1].IsNot(INLET))
        BoundLHSMatrix(1, 1) += Weight * coeff;
      if (rGeom[2].IsNot(INLET))
        BoundLHSMatrix(2, 2) += Weight * coeff;
    }
    // }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::ComputeBoundLHSMatrix(Matrix &BoundLHSMatrix,
                                                                   const ShapeFunctionsType &rN,
                                                                   const double Weight)
  {
    GeometryType &rGeom = this->GetGeometry();
    double coeff = 0.25;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE))
    {
      if (rGeom[0].IsNot(INLET))
        BoundLHSMatrix(0, 0) += Weight * coeff;
      if (rGeom[1].IsNot(INLET))
        BoundLHSMatrix(1, 1) += Weight * coeff;
      if (rGeom[2].IsNot(INLET))
        BoundLHSMatrix(2, 2) += Weight * coeff;
    }
    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {
      if (rGeom[0].IsNot(INLET))
        BoundLHSMatrix(0, 0) += Weight * coeff;
      if (rGeom[1].IsNot(INLET))
        BoundLHSMatrix(1, 1) += Weight * coeff;
      if (rGeom[3].IsNot(INLET))
        BoundLHSMatrix(3, 3) += Weight * coeff;
    }
    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {
      if (rGeom[0].IsNot(INLET))
        BoundLHSMatrix(0, 0) += Weight * coeff;
      if (rGeom[2].IsNot(INLET))
        BoundLHSMatrix(2, 2) += Weight * coeff;
      if (rGeom[3].IsNot(INLET))
        BoundLHSMatrix(3, 3) += Weight * coeff;
    }
    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE))
    {
      if (rGeom[1].IsNot(INLET))
        BoundLHSMatrix(1, 1) += Weight * coeff;
      if (rGeom[2].IsNot(INLET))
        BoundLHSMatrix(2, 2) += Weight * coeff;
      if (rGeom[3].IsNot(INLET))
        BoundLHSMatrix(3, 3) += Weight * coeff;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateTauFIC(double &Tau,
                                                                double ElemSize,
                                                                const double Density,
                                                                const double Viscosity,
                                                                const ProcessInfo &rCurrentProcessInfo)
  {
    double DeltaTime = rCurrentProcessInfo.GetValue(DELTA_TIME);
    if (rCurrentProcessInfo.GetValue(DELTA_TIME) < rCurrentProcessInfo.GetValue(PREVIOUS_DELTA_TIME))
    {
      DeltaTime = 0.5 * rCurrentProcessInfo.GetValue(DELTA_TIME) + 0.5 * rCurrentProcessInfo.GetValue(PREVIOUS_DELTA_TIME);
    }

    double MeanVelocity = 0;
    this->CalcMeanVelocityNorm(MeanVelocity, 0);

    // Tau = 1.0 / (2.0 * Density *(0.5 * MeanVelocity / ElemSize + 0.5/DeltaTime) +  8.0 * Viscosity / (ElemSize * ElemSize) );
    Tau = (ElemSize * ElemSize * DeltaTime) / (Density * MeanVelocity * DeltaTime * ElemSize + Density * ElemSize * ElemSize + 8.0 * Viscosity * DeltaTime);

    const double tolerance = 1.0e-13;
    if (MeanVelocity < tolerance)
    {
      Tau = 0;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::ComputeLumpedMatrixForPressure(Matrix &BulkMatrix,
                                                                               const double Weight)
  {
    const SizeType NumNodes = this->GetGeometry().PointsNumber();

    if ((NumNodes == 3 && TDim == 2) || (NumNodes == 4 && TDim == 3))
    {
      double coeff = 1.0 + TDim;
      for (SizeType i = 0; i < NumNodes; ++i)
      {
        // LHS contribution
        double Mij = Weight / coeff;
        // if (this->GetGeometry()[i].Is(FREE_SURFACE) && this->GetGeometry()[i].IsNot(INLET))
        BulkMatrix(i, i) += Mij;
      }
    }
    else
    {
      KRATOS_ERROR << "... ComputeLumpedMatrixForPressure TO IMPLEMENT" << std::endl;
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::ComputeBoundaryTermsForPressureSystem(Matrix &rBoundLHSMatrix,
                                                                                   VectorType &rBoundRHSVector,
                                                                                   const ShapeFunctionsType &rN,
                                                                                   const double lagMultiplier)
  {
    GeometryType &rGeom = this->GetGeometry();
    double meanPressure = 0;
    double edgeLength = 0;

    // double viscousCoeff = 0;
    // this->EvaluateInPoint(viscousCoeff, DYNAMIC_VISCOSITY, rN);
    // viscousCoeff *= 2.0;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[0].IsNot(INLET) && rGeom[1].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 1, 2, edgeLength);

      // double SpatialDefRateNormalProjection = viscousCoeff * (deviatoricSpatialDefRate[0] * NormalVector[0] * NormalVector[0] + deviatoricSpatialDefRate[2] * NormalVector[1] * NormalVector[0]);
      // SpatialDefRateNormalProjection += viscousCoeff * (deviatoricSpatialDefRate[1] * NormalVector[1] * NormalVector[1] + deviatoricSpatialDefRate[2] * NormalVector[0] * NormalVector[1]);

      rBoundLHSMatrix(0, 0) += edgeLength * rN[0] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(1, 1) += edgeLength * rN[1] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 0) += edgeLength * rN[1] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 1) += edgeLength * rN[0] * rN[1] * lagMultiplier;

      meanPressure = rGeom[0].FastGetSolutionStepValue(PRESSURE, 0) * rN[0] + rGeom[1].FastGetSolutionStepValue(PRESSURE, 0) * rN[1];

      rBoundRHSVector[0] += -edgeLength * rN[0] * lagMultiplier * meanPressure;
      rBoundRHSVector[1] += -edgeLength * rN[1] * lagMultiplier * meanPressure;
      // rBoundRHSVector[0] += edgeLength * rN[0] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
      // rBoundRHSVector[1] += edgeLength * rN[1] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[0].IsNot(INLET) && rGeom[2].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 0, 2, 1, edgeLength);

      // double SpatialDefRateNormalProjection = viscousCoeff * (deviatoricSpatialDefRate[0] * NormalVector[0] * NormalVector[0] + deviatoricSpatialDefRate[2] * NormalVector[1] * NormalVector[0]);
      // SpatialDefRateNormalProjection += viscousCoeff * (deviatoricSpatialDefRate[1] * NormalVector[1] * NormalVector[1] + deviatoricSpatialDefRate[2] * NormalVector[0] * NormalVector[1]);

      rBoundLHSMatrix(0, 0) += edgeLength * rN[0] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(2, 2) += edgeLength * rN[2] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(2, 0) += edgeLength * rN[2] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 2) += edgeLength * rN[0] * rN[2] * lagMultiplier;

      meanPressure = rGeom[0].FastGetSolutionStepValue(PRESSURE, 0) * rN[0] + rGeom[2].FastGetSolutionStepValue(PRESSURE, 0) * rN[2];

      rBoundRHSVector[0] += -edgeLength * rN[0] * lagMultiplier * meanPressure;
      rBoundRHSVector[2] += -edgeLength * rN[2] * lagMultiplier * meanPressure;
      // rBoundRHSVector[0] += edgeLength * rN[0] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
      // rBoundRHSVector[2] += edgeLength * rN[2] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
    }

    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[1].IsNot(INLET) && rGeom[2].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForTwoPoints(NormalVector, 1, 2, 0, edgeLength);

      // double SpatialDefRateNormalProjection = viscousCoeff * (deviatoricSpatialDefRate[0] * NormalVector[0] * NormalVector[0] + deviatoricSpatialDefRate[2] * NormalVector[1] * NormalVector[0]);
      // SpatialDefRateNormalProjection += viscousCoeff * (deviatoricSpatialDefRate[1] * NormalVector[1] * NormalVector[1] + deviatoricSpatialDefRate[2] * NormalVector[0] * NormalVector[1]);

      rBoundLHSMatrix(1, 1) += edgeLength * rN[1] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(2, 2) += edgeLength * rN[2] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(2, 1) += edgeLength * rN[2] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 2) += edgeLength * rN[1] * rN[2] * lagMultiplier;

      meanPressure = rGeom[1].FastGetSolutionStepValue(PRESSURE, 0) * rN[1] + rGeom[2].FastGetSolutionStepValue(PRESSURE, 0) * rN[2];

      rBoundRHSVector[1] += -edgeLength * rN[1] * lagMultiplier * meanPressure;
      rBoundRHSVector[2] += -edgeLength * rN[2] * lagMultiplier * meanPressure;
      // rBoundRHSVector[1] += edgeLength * rN[1] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
      // rBoundRHSVector[2] += edgeLength * rN[2] * lagMultiplier * (SpatialDefRateNormalProjection - meanPressure);
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::ComputeBoundaryTermsForPressureSystem(Matrix &rBoundLHSMatrix,
                                                                                   VectorType &rBoundRHSVector,
                                                                                   const ShapeFunctionsType &rN,
                                                                                   const double lagMultiplier)
  {
    GeometryType &rGeom = this->GetGeometry();
    double meanPressure = 0;
    double surfaceArea = 0;

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) &&
        rGeom[0].IsNot(INLET) && rGeom[1].IsNot(INLET) && rGeom[2].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 1, 2, 3, surfaceArea);

      rBoundLHSMatrix(0, 0) += surfaceArea * rN[0] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(1, 1) += surfaceArea * rN[1] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(2, 2) += surfaceArea * rN[2] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(1, 0) += surfaceArea * rN[1] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 1) += surfaceArea * rN[0] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(2, 0) += surfaceArea * rN[2] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 2) += surfaceArea * rN[0] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(2, 1) += surfaceArea * rN[2] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 2) += surfaceArea * rN[1] * rN[2] * lagMultiplier;

      meanPressure = rN[0] * rGeom[0].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[1] * rGeom[1].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[2] * rGeom[2].FastGetSolutionStepValue(PRESSURE, 0);

      rBoundRHSVector[0] += -surfaceArea * rN[0] * lagMultiplier * meanPressure;
      rBoundRHSVector[1] += -surfaceArea * rN[1] * lagMultiplier * meanPressure;
      rBoundRHSVector[2] += -surfaceArea * rN[2] * lagMultiplier * meanPressure;
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[1].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE) &&
        rGeom[0].IsNot(INLET) && rGeom[1].IsNot(INLET) && rGeom[3].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 1, 3, 2, surfaceArea);

      rBoundLHSMatrix(0, 0) += surfaceArea * rN[0] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(1, 1) += surfaceArea * rN[1] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(3, 3) += surfaceArea * rN[3] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(1, 0) += surfaceArea * rN[1] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 1) += surfaceArea * rN[0] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(3, 0) += surfaceArea * rN[3] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 3) += surfaceArea * rN[0] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(3, 1) += surfaceArea * rN[3] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 3) += surfaceArea * rN[1] * rN[3] * lagMultiplier;

      meanPressure = rN[0] * rGeom[0].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[1] * rGeom[1].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[3] * rGeom[3].FastGetSolutionStepValue(PRESSURE, 0);

      rBoundRHSVector[0] += -surfaceArea * rN[0] * lagMultiplier * meanPressure;
      rBoundRHSVector[1] += -surfaceArea * rN[1] * lagMultiplier * meanPressure;
      rBoundRHSVector[3] += -surfaceArea * rN[3] * lagMultiplier * meanPressure;
    }

    if (rGeom[0].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE) &&
        rGeom[0].IsNot(INLET) && rGeom[2].IsNot(INLET) && rGeom[3].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 0, 2, 3, 1, surfaceArea);

      rBoundLHSMatrix(0, 0) += surfaceArea * rN[0] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(2, 2) += surfaceArea * rN[2] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(3, 3) += surfaceArea * rN[3] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(2, 0) += surfaceArea * rN[2] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 2) += surfaceArea * rN[0] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(3, 0) += surfaceArea * rN[3] * rN[0] * lagMultiplier;
      rBoundLHSMatrix(0, 3) += surfaceArea * rN[0] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(3, 2) += surfaceArea * rN[3] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(2, 3) += surfaceArea * rN[2] * rN[3] * lagMultiplier;

      meanPressure = rN[0] * rGeom[0].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[2] * rGeom[2].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[3] * rGeom[3].FastGetSolutionStepValue(PRESSURE, 0);

      rBoundRHSVector[0] += -surfaceArea * rN[0] * lagMultiplier * meanPressure;
      rBoundRHSVector[2] += -surfaceArea * rN[2] * lagMultiplier * meanPressure;
      rBoundRHSVector[3] += -surfaceArea * rN[3] * lagMultiplier * meanPressure;
    }

    if (rGeom[1].Is(FREE_SURFACE) && rGeom[2].Is(FREE_SURFACE) && rGeom[3].Is(FREE_SURFACE) &&
        rGeom[1].IsNot(INLET) && rGeom[2].IsNot(INLET) && rGeom[3].IsNot(INLET))
    {
      array_1d<double, 3> NormalVector(3, 0.0);
      this->GetOutwardsUnitNormalForThreePoints(NormalVector, 1, 2, 3, 0, surfaceArea);

      rBoundLHSMatrix(1, 1) += surfaceArea * rN[1] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(2, 2) += surfaceArea * rN[2] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(3, 3) += surfaceArea * rN[3] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(2, 1) += surfaceArea * rN[2] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 2) += surfaceArea * rN[1] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(3, 1) += surfaceArea * rN[3] * rN[1] * lagMultiplier;
      rBoundLHSMatrix(1, 3) += surfaceArea * rN[1] * rN[3] * lagMultiplier;
      rBoundLHSMatrix(3, 2) += surfaceArea * rN[3] * rN[2] * lagMultiplier;
      rBoundLHSMatrix(2, 3) += surfaceArea * rN[2] * rN[3] * lagMultiplier;

      meanPressure = rN[1] * rGeom[1].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[2] * rGeom[2].FastGetSolutionStepValue(PRESSURE, 0) +
                     rN[3] * rGeom[3].FastGetSolutionStepValue(PRESSURE, 0);

      rBoundRHSVector[1] += -surfaceArea * rN[1] * lagMultiplier * meanPressure;
      rBoundRHSVector[2] += -surfaceArea * rN[2] * lagMultiplier * meanPressure;
      rBoundRHSVector[3] += -surfaceArea * rN[3] * lagMultiplier * meanPressure;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateTauPSPG(double &Tau,
                                                                 double ElemSize,
                                                                 const double Density,
                                                                 const double Viscosity,
                                                                 const ProcessInfo &rCurrentProcessInfo)
  {
    double DeltaTime = rCurrentProcessInfo.GetValue(DELTA_TIME);
    if (rCurrentProcessInfo.GetValue(DELTA_TIME) < rCurrentProcessInfo.GetValue(PREVIOUS_DELTA_TIME))
    {
      DeltaTime = 0.5 * rCurrentProcessInfo.GetValue(DELTA_TIME) + 0.5 * rCurrentProcessInfo.GetValue(PREVIOUS_DELTA_TIME);
    }

    double MeanVelocity = 0;
    this->CalcMeanVelocityNorm(MeanVelocity, 0);

    // // Tau Fic
    //Tau = (ElemSize * ElemSize * DeltaTime) / (Density * MeanVelocity * DeltaTime * ElemSize + Density * ElemSize * ElemSize + 8.0 * Viscosity * DeltaTime);

    // // Tau Tezduyar first proposal
    // double timeScale = 0.5 * (ElemSize / (2 * MeanVelocity) + DeltaTime);
    // double Reynolds = MeanVelocity * ElemSize * Density / (2.0 * Viscosity);
    // double ReynoldsFactor = 1.0;
    // if (Reynolds < 3)
    // {
    //   ReynoldsFactor = (ElemSize * ReynoldsFactor / (2 * MeanVelocity)) / 3.0;
    // }
    // Tau = timeScale * ReynoldsFactor / Density;

    // // Tau Tezduyar second proposal
    // double timeStepTerm = 2.0 / DeltaTime;
    // double timeScaleTerm = 2.0 * MeanVelocity / ElemSize;
    // double viscousTerm = 3.0 * (4 * Viscosity / (Density * pow(ElemSize, 2)));
    // // std::cout << "timeStepTerm= "<<timeStepTerm<< " timeScaleTerm= "<<timeScaleTerm<< " viscousTerm= "<<viscousTerm << std::endl;
    // double alternativeTau = pow(timeStepTerm, 2) + pow(timeScaleTerm, 2) + pow(viscousTerm, 2);
    // Tau = 1.0 / (std::sqrt(alternativeTau) * Density);

    // // Simplest Tau
    Tau = DeltaTime / Density;

    // std::cout << "Tau= " << Tau << std::endl;

    const double tolerance = 1.0e-13;
    if (MeanVelocity < tolerance)
    {
      Tau = 0;
    }
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::ComputeStabLaplacianMatrix(MatrixType &StabLaplacianMatrix,
                                                                           const ShapeFunctionDerivativesType &rDN_DX,
                                                                           const double Weight)

  {
    // LHS contribution
    const SizeType NumNodes = this->GetGeometry().PointsNumber();
    for (SizeType i = 0; i < NumNodes; ++i)
    {
      for (SizeType j = 0; j < NumNodes; ++j)
      {
        double Lij = 0.0;
        for (SizeType d = 0; d < TDim; ++d)
        {
          Lij += rDN_DX(i, d) * rDN_DX(j, d);
        }
        StabLaplacianMatrix(i, j) += Weight * Lij;
      }
    }
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<2>::AddPspgDynamicPartStabilization(VectorType &rRightHandSideVector,
                                                                             const double Tau,
                                                                             const double Density,
                                                                             const double Weight,
                                                                             const double TimeStep,
                                                                             const ShapeFunctionDerivativesType &rDN_DX,
                                                                             const ShapeFunctionsType &rN,
                                                                             const SizeType i)
  {

    double RHSi = 0;
    // LHS contribution
    const SizeType NumNodes = this->GetGeometry().PointsNumber();
    for (SizeType j = 0; j < NumNodes; ++j)
    {
      //RHSi += rDN_DX(i, 0) * rN[j] * this->GetGeometry()[j].FastGetSolutionStepValue(ACCELERATION_X, 0) + rDN_DX(i, 1) * rN[j] * this->GetGeometry()[j].FastGetSolutionStepValue(ACCELERATION_Y, 0);
      RHSi += rDN_DX(i, 0) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_X, 1)) / TimeStep;
      RHSi += rDN_DX(i, 1) * rN[j] * (this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 0) - this->GetGeometry()[j].FastGetSolutionStepValue(VELOCITY_Y, 1)) / TimeStep;
    }
    rRightHandSideVector[i] += -Weight * Tau * Density * RHSi;
  }

  template <>
  void ThreeStepUpdatedLagrangianElement<3>::AddPspgDynamicPartStabilization(VectorType &rRightHandSideVector,
                                                                             const double Tau,
                                                                             const double Density,
                                                                             const double Weight,
                                                                             const double TimeStep,
                                                                             const ShapeFunctionDerivativesType &rDN_DX,
                                                                             const ShapeFunctionsType &rN,
                                                                             const SizeType i)
  {

    double RHSi = 0;
    //////////////////////////// TO CHEEEEEEEEEEEEEEEEEEEEEEECCCCCCCCCCCCCCCCCCCCKKKKKKKKKKKKKKK///////////////////////

    // LHS contribution
    const SizeType NumNodes = this->GetGeometry().PointsNumber();
    for (SizeType j = 0; j < NumNodes; ++j)
    {
      double termX = rDN_DX(i, 0) * rN[j] * this->GetGeometry()[j].FastGetSolutionStepValue(ACCELERATION_X, 0);
      double termY = rDN_DX(i, 1) * rN[j] * this->GetGeometry()[j].FastGetSolutionStepValue(ACCELERATION_Y, 0);
      double termZ = rDN_DX(i, 2) * rN[j] * this->GetGeometry()[j].FastGetSolutionStepValue(ACCELERATION_Z, 0);
      RHSi += -Tau * Density * (termX + termY + termZ);
    }
    rRightHandSideVector[i] += Weight * RHSi;
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::InitializeElementalVariables(ElementalVariables &rElementalVariables)
  {
    KRATOS_TRY;

    unsigned int voigtsize = 3;
    if (TDim == 3)
    {
      voigtsize = 6;
    }
    rElementalVariables.voigtsize = voigtsize;

    rElementalVariables.ConstitutiveMatrix = ZeroMatrix(voigtsize, voigtsize);

    rElementalVariables.DetFgrad = 1.0;

    rElementalVariables.DetFgradVel = 1.0;

    rElementalVariables.DeviatoricInvariant = 1.0;

    rElementalVariables.EquivalentStrainRate = 1.0;

    rElementalVariables.VolumetricDefRate = 1.0;

    rElementalVariables.SpatialDefRate = ZeroVector(voigtsize);

    rElementalVariables.MDGreenLagrangeMaterial.resize(voigtsize, false);

    noalias(rElementalVariables.MDGreenLagrangeMaterial) = ZeroVector(voigtsize);

    rElementalVariables.Fgrad = ZeroMatrix(TDim, TDim);

    rElementalVariables.InvFgrad = ZeroMatrix(TDim, TDim);

    rElementalVariables.FgradVel = ZeroMatrix(TDim, TDim);

    rElementalVariables.InvFgradVel = ZeroMatrix(TDim, TDim);

    rElementalVariables.SpatialVelocityGrad = ZeroMatrix(TDim, TDim);

    rElementalVariables.MeanPressure = 0;

    rElementalVariables.CurrentTotalCauchyStress = ZeroVector(voigtsize);

    rElementalVariables.UpdatedTotalCauchyStress = ZeroVector(voigtsize);

    rElementalVariables.CurrentDeviatoricCauchyStress = ZeroVector(voigtsize);

    rElementalVariables.UpdatedDeviatoricCauchyStress = ZeroVector(voigtsize);

    KRATOS_CATCH("");
  }

  template <unsigned int TDim>
  void ThreeStepUpdatedLagrangianElement<TDim>::CalculateLastVelocitySystem(MatrixType &rLeftHandSideMatrix,
                                                                            VectorType &rRightHandSideVector,
                                                                            const ProcessInfo &rCurrentProcessInfo) //useless, it is done in Calculate called from the strategy
  {
    // const GeometryType &rGeom = this->GetGeometry();
    // const SizeType NumNodes = rGeom.PointsNumber();
    // const SizeType LocalSize = TDim * NumNodes;

    // // Check sizes and initialize
    // if (rLeftHandSideMatrix.size1() != LocalSize)
    //   rLeftHandSideMatrix.resize(LocalSize, LocalSize);

    // rLeftHandSideMatrix = ZeroMatrix(LocalSize, LocalSize);

    // if (rRightHandSideVector.size() != LocalSize)
    //   rRightHandSideVector.resize(LocalSize);

    // rRightHandSideVector = ZeroVector(LocalSize);

    // // Shape functions and integration points
    // ShapeFunctionDerivativesArrayType DN_DX;
    // Matrix NContainer;
    // VectorType GaussWeights;
    // this->CalculateGeometryData(DN_DX, NContainer, GaussWeights);
    // const unsigned int NumGauss = GaussWeights.size();

    // // Stabilization parameters
    // double ElemSize = this->ElementSize();

    // double Bdf0 = rCurrentProcessInfo[BDF_COEFFICIENTS][0]; // for BDF2, 3/(2dt) /// !!!!!!!! play attention to this!

    // // Loop on integration points
    // for (unsigned int g = 0; g < NumGauss; g++)
    // {
    //   const double GaussWeight = GaussWeights[g];
    //   const ShapeFunctionsType &N = row(NContainer, g);
    //   const ShapeFunctionDerivativesType &rDN_DX = DN_DX[g];

    //   // Evaluate required variables at the integration point
    //   double Density;
    //   this->EvaluateInPoint(Density, DENSITY, N);

    //   const double Coeff = GaussWeight * Density * Bdf0;

    //   // Calculate contribution to the gradient term (RHS)
    //   double DeltaPressure;
    //   this->EvaluateInPoint(DeltaPressure, PRESSURE_OLD_IT, N); /// !!!!!!!! play attention to this!

    //   SizeType RowIndex = 0;

    //   for (SizeType i = 0; i < NumNodes; ++i)
    //   {
    //     for (SizeType d = 0; d < TDim; ++d)
    //     {
    //       rRightHandSideVector[RowIndex++] += GaussWeight * rDN_DX(i, d) * DeltaPressure;
    //     }
    //   }

    //   // LHS matrix includes mass and viscous terms
    //   double Viscosity = 0;
    //   this->EvaluateInPoint(Viscosity, DYNAMIC_VISCOSITY, N);

    //   this->AddMomentumMassTerm(rLeftHandSideMatrix, N, Coeff);

    //   // Add viscous term
    //   const double ViscousCoeff = Viscosity * GaussWeight;
    //   this->AddViscousTerm(rLeftHandSideMatrix, rDN_DX, ViscousCoeff);
    // }
  }

  /*
   * Template class definition (this should allow us to compile the desired template instantiations)
   */

  template class ThreeStepUpdatedLagrangianElement<2>;
  template class ThreeStepUpdatedLagrangianElement<3>;

} // namespace Kratos
