//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Riccardo Rossi
//

#if !defined(KRATOS_COMPRESSIBLE_POTENTIAL_FLOW_ELEMENT_H_INCLUDED )
#define KRATOS_COMPRESSIBLE_POTENTIAL_FLOW_ELEMENT_H_INCLUDED


// System includes
#include <string>
#include <iostream>
#include <algorithm>

// External includes


// Project includes
#include "includes/element.h"
#include "includes/kratos_flags.h"
#include "compressible_potential_flow_application_variables.h"
#include "utilities/geometry_utilities.h"

namespace Kratos
{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

template< int Dim, int NumNodes >
class CompressiblePotentialFlowElement : public Element
{
public:

    template <unsigned int TNumNodes, unsigned int TDim>
    struct ElementalData
    {
        array_1d<double,TNumNodes> phis, distances;
        double rho;
        double vol;

        bounded_matrix<double, TNumNodes, TDim > DN_DX;
        array_1d<double, TNumNodes > N;
    };



    ///@name Type Definitions
    ///@{

    typedef Element BaseType;

    ///@}
    ///@name Pointer Definitions
    /// Pointer definition of CompressiblePotentialFlowElement
    KRATOS_CLASS_POINTER_DEFINITION(CompressiblePotentialFlowElement);

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * Constructor.
     */
    CompressiblePotentialFlowElement(IndexType NewId = 0) {};

    /**
     * Constructor using an array of nodes
     */
    CompressiblePotentialFlowElement(IndexType NewId, const NodesArrayType& ThisNodes):Element(NewId, ThisNodes) {};

    /**
     * Constructor using Geometry
     */
    CompressiblePotentialFlowElement(IndexType NewId, GeometryType::Pointer pGeometry):Element(NewId, pGeometry) {};

    /**
     * Constructor using Properties
     */
    CompressiblePotentialFlowElement(IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties):Element(NewId, pGeometry, pProperties) {};

    /**
     * Copy Constructor
     */
    CompressiblePotentialFlowElement(CompressiblePotentialFlowElement const& rOther) {};

    /**
     * Destructor
     */
    virtual ~CompressiblePotentialFlowElement() {};

    ///@}
    ///@name Operators
    ///@{

    /// Assignment operator.
    CompressiblePotentialFlowElement & operator=(CompressiblePotentialFlowElement const& rOther)
    {
        BaseType::operator=(rOther);
        Flags::operator =(rOther);
        return *this;
    }

    ///@}
    ///@name Operations
    ///@{

    /**
     * creates a new element pointer
     * @param NewId: the ID of the new element
     * @param ThisNodes: the nodes of the new element
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Create(IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties) const override
    {
        KRATOS_TRY
        return Element::Pointer(new CompressiblePotentialFlowElement(NewId, GetGeometry().Create(ThisNodes), pProperties));
        KRATOS_CATCH("");
    }

    /**
     * creates a new element pointer
     * @param NewId: the ID of the new element
     * @param pGeom: the geometry to be employed
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Create(IndexType NewId, GeometryType::Pointer pGeom, PropertiesType::Pointer pProperties) const override
    {
        KRATOS_TRY
        return Element::Pointer(new CompressiblePotentialFlowElement(NewId, pGeom, pProperties));
        KRATOS_CATCH("");
    }

    /**
     * creates a new element pointer and clones the previous element data
     * @param NewId: the ID of the new element
     * @param ThisNodes: the nodes of the new element
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Clone(IndexType NewId, NodesArrayType const& ThisNodes) const override
    {
        KRATOS_TRY
        return Element::Pointer(new CompressiblePotentialFlowElement(NewId, GetGeometry().Create(ThisNodes), pGetProperties()));
        KRATOS_CATCH("");
    }

    /**
     * this determines the elemental equation ID vector for all elemental
     * DOFs
     * @param rResult: the elemental equation ID vector
     * @param rCurrentProcessInfo: the current process info instance
     */
    virtual void EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& CurrentProcessInfo) override
    {
        if(this->IsNot(MARKER)) //normal element
        {
            if (rResult.size() != NumNodes)
                rResult.resize(NumNodes, false);

            for (unsigned int i = 0; i < NumNodes; i++)
                rResult[i] = GetGeometry()[i].GetDof(POSITIVE_FACE_PRESSURE).EquationId();

        }
        else//wake element
        {
            if (rResult.size() != 2*NumNodes)
                rResult.resize(2*NumNodes, false);

//     array_1d<double,NumNodes> distances = this->GetValue(ELEMENTAL_DISTANCES);
            array_1d<double,NumNodes> distances;
            for(unsigned int i = 0; i<NumNodes; i++)
                distances[i] = GetGeometry()[i].FastGetSolutionStepValue(DISTANCE);

            //positive part
            for (unsigned int i = 0; i < NumNodes; i++)
            {
                if(distances[i] > 0)
                    rResult[i] = GetGeometry()[i].GetDof(POSITIVE_FACE_PRESSURE).EquationId();
                else
                    rResult[i] = GetGeometry()[i].GetDof(NEGATIVE_FACE_PRESSURE,0).EquationId();
            }

            //negative part - sign is opposite to the previous case
            for (unsigned int i = 0; i < NumNodes; i++)
            {
                if(distances[i] < 0)
                    rResult[NumNodes+i] = GetGeometry()[i].GetDof(POSITIVE_FACE_PRESSURE).EquationId();
                else
                    rResult[NumNodes+i] = GetGeometry()[i].GetDof(NEGATIVE_FACE_PRESSURE,0).EquationId();
            }
        }


    }


    /**
     * determines the elemental list of DOFs
     * @param ElementalDofList: the list of DOFs
     * @param rCurrentProcessInfo: the current process info instance
     */
    virtual void GetDofList(DofsVectorType& rElementalDofList, ProcessInfo& CurrentProcessInfo) override
    {
        if(this->IsNot(MARKER)) //normal element
        {
            if (rElementalDofList.size() != NumNodes)
                rElementalDofList.resize(NumNodes);

            for (unsigned int i = 0; i < NumNodes; i++)
                rElementalDofList[i] = GetGeometry()[i].pGetDof(POSITIVE_FACE_PRESSURE);
        }
        else//wake element
        {
            if (rElementalDofList.size() != 2*NumNodes)
                rElementalDofList.resize(2*NumNodes);

//     const array_1d<double,NumNodes>& distances = this->GetValue(ELEMENTAL_DISTANCES);
            array_1d<double,NumNodes> distances;
            for(unsigned int i = 0; i<NumNodes; i++)
                distances[i] = GetGeometry()[i].FastGetSolutionStepValue(DISTANCE);

            //positive part
            for (unsigned int i = 0; i < NumNodes; i++)
            {
                if(distances[i] > 0)
                    rElementalDofList[i] = GetGeometry()[i].pGetDof(POSITIVE_FACE_PRESSURE);
                else
                    rElementalDofList[i] = GetGeometry()[i].pGetDof(NEGATIVE_FACE_PRESSURE);
            }

            //negative part - sign is opposite to the previous case
            for (unsigned int i = 0; i < NumNodes; i++)
            {
                if(distances[i] < 0)
                    rElementalDofList[NumNodes+i] = GetGeometry()[i].pGetDof(POSITIVE_FACE_PRESSURE);
                else
                    rElementalDofList[NumNodes+i] = GetGeometry()[i].pGetDof(NEGATIVE_FACE_PRESSURE);
            }
        }
    }


    /**
     * ELEMENTS inherited from this class have to implement next
     * CalculateLocalSystem, CalculateLeftHandSide and CalculateRightHandSide methods
     * they can be managed internally with a private method to do the same calculations
     * only once: MANDATORY
     */

    /**
     * this is called during the assembling process in order
     * to calculate all elemental contributions to the global system
     * matrix and the right hand side
     * @param rLeftHandSideMatrix: the elemental left hand side matrix
     * @param rRightHandSideVector: the elemental right hand side
     * @param rCurrentProcessInfo: the current process info instance
     */
    virtual void CalculateLocalSystem(
        MatrixType& rLeftHandSideMatrix,
        VectorType& rRightHandSideVector,
        ProcessInfo& rCurrentProcessInfo) override
    {
        ElementalData<NumNodes,Dim> data;

        //calculate shape functions
        GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

        //gather nodal data
        for(unsigned int i=0; i<NumNodes; i++)
        {
            data.phis[i] = GetGeometry()[i].FastGetSolutionStepValue(POSITIVE_FACE_PRESSURE);
            data.distances[i] = GetGeometry()[i].FastGetSolutionStepValue(DISTANCE);
        }

        if(this->IsNot(MARKER))//normal element (non-wake)
        {
            if(rLeftHandSideMatrix.size1() != NumNodes || rLeftHandSideMatrix.size2() != NumNodes)
                rLeftHandSideMatrix.resize(NumNodes,NumNodes,false);
            if(rRightHandSideVector.size() != NumNodes)
                rRightHandSideVector.resize(NumNodes,false);
            rLeftHandSideMatrix.clear();
            rRightHandSideVector.clear();

            ComputeLHSGaussPointContribution(data.vol,rLeftHandSideMatrix,data);
            ComputeRHSGaussPointContribution(data.vol,rRightHandSideVector,data);
        }
        else //it is a wake element
        {
            //note that the lhs and rhs have double the size!!
            if(rLeftHandSideMatrix.size1() != 2*NumNodes || rLeftHandSideMatrix.size2() != 2*NumNodes)
                rLeftHandSideMatrix.resize(2*NumNodes,2*NumNodes,false);
            if(rRightHandSideVector.size() != 2*NumNodes)
                rRightHandSideVector.resize(2*NumNodes,false);
            rLeftHandSideMatrix.clear();

            //compute the lhs and rhs that would correspond to it not being divided
            Matrix lhs = ZeroMatrix(NumNodes,NumNodes);

            double weight = data.vol;
            ComputeLHSGaussPointContribution(weight,lhs,data);

            //here impose the wake constraint
            const double k=1.0e4; //a value of 1000 here should be good enough
            for(unsigned int i=0; i<NumNodes; ++i)
            {
                for(unsigned int j=0; j<NumNodes; ++j)
                {
                    rLeftHandSideMatrix(i,j)                   =  (1.0+k)*lhs(i,j);
                    rLeftHandSideMatrix(i,j+NumNodes)          = -k*lhs(i,j);
                    rLeftHandSideMatrix(i+NumNodes,j)          = -k*lhs(i,j);
                    rLeftHandSideMatrix(i+NumNodes,j+NumNodes) =  (1.0+k)*lhs(i,j);
                }
            }
            Vector split_element_values(NumNodes*2);
            GetValuesOnSplitElement(split_element_values);
            noalias(rRightHandSideVector) = -prod(rLeftHandSideMatrix,split_element_values);
        }

//      KRATOS_WATCH(rLeftHandSideMatrix)
    }


    /**
     * this is called during the assembling process in order
     * to calculate the elemental right hand side vector only
     * @param rRightHandSideVector: the elemental right hand side vector
     * @param rCurrentProcessInfo: the current process info instance
     */
    virtual void CalculateRightHandSide(VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo) override
    {
        //TODO: improve speed
        Matrix tmp;
        CalculateLocalSystem(tmp, rRightHandSideVector, rCurrentProcessInfo);
    }


    /**
     * This method provides the place to perform checks on the completeness of the input
     * and the compatibility with the problem options as well as the contitutive laws selected
     * It is designed to be called only once (or anyway, not often) typically at the beginning
     * of the calculations, so to verify that nothing is missing from the input
     * or that no common error is found.
     * @param rCurrentProcessInfo
     * this method is: MANDATORY
     */
    virtual int Check(const ProcessInfo& rCurrentProcessInfo) override
    {

        KRATOS_TRY

        if (this->Id() < 1)
        {
            KRATOS_THROW_ERROR(std::logic_error, "CompressiblePotentialFlowElement found with Id 0 or negative","")
        }

        if (this->GetGeometry().Area() <= 0)
        {
            std::cout << "error on CompressiblePotentialFlowElement -> " << this->Id() << std::endl;
            KRATOS_THROW_ERROR(std::logic_error, "Area cannot be less than or equal to 0","")
        }

        for ( unsigned int i = 0; i < this->GetGeometry().size(); i++ )
        {
            if ( this->GetGeometry()[i].SolutionStepsDataHas( POSITIVE_FACE_PRESSURE ) == false )
                KRATOS_THROW_ERROR( std::invalid_argument, "missing variable POSITIVE_FACE_PRESSURE on node ", this->GetGeometry()[i].Id() )
            }


        return 0;

        KRATOS_CATCH("");
    }

    virtual void GetValueOnIntegrationPoints(const Variable<double>& rVariable,
            std::vector<double>& rValues,
            const ProcessInfo& rCurrentProcessInfo) override
    {
        if(rValues.size() != 1) rValues.resize(1);

        if (rVariable == PRESSURE)
        {
            double p = 0.0;

            bool active = true;
            if ((this)->IsDefined(ACTIVE))
                active = (this)->Is(ACTIVE);

            if(active && !this->Is(MARKER))
            {
                const array_1d<double,3> vinfinity = rCurrentProcessInfo[VELOCITY];
                const double vinfinity_norm = norm_2(vinfinity);

                ElementalData<NumNodes,Dim> data;

                //calculate shape functions
                GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

                //gather nodal data
                for(unsigned int i=0; i<NumNodes; i++)
                {
                    data.phis[i] = GetGeometry()[i].FastGetSolutionStepValue(POSITIVE_FACE_PRESSURE);
                }

                const array_1d<double,Dim> v = prod(trans(data.DN_DX), data.phis);


                p = (vinfinity_norm - norm_2(v))/vinfinity_norm; //0.5*(norm_2(vinfinity) - norm_2(v));
            }


            rValues[0] = p;
        }
    }

    virtual void GetValueOnIntegrationPoints(const Variable<array_1d<double,3> >& rVariable,
            std::vector< array_1d<double,3> >& rValues,
            const ProcessInfo& rCurrentProcessInfo) override
    {
        if(rValues.size() != 1) rValues.resize(1);

        if (rVariable == VELOCITY)
        {
            bool active = true;
            if ((this)->IsDefined(ACTIVE))
                active = (this)->Is(ACTIVE);

            array_1d<double,3> v;
            if(this->Is(MARKER) || active==false)
                v.clear();
            else
            {
                ElementalData<NumNodes,Dim> data;

                //calculate shape functions
                GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

                //gather nodal data
                for(unsigned int i=0; i<NumNodes; i++)
                {
                    data.phis[i] = GetGeometry()[i].FastGetSolutionStepValue(POSITIVE_FACE_PRESSURE);
                }

                array_1d<double,Dim> vaux = -prod(trans(data.DN_DX), data.phis);
                v[0] = vaux[0];
                v[1] = vaux[1];
                v[2] = 0.0;
            }



            rValues[0] = v;
        }
    }


    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const
    {
        std::stringstream buffer;
        buffer << "CompressiblePotentialFlowElement #" << Id();
        return buffer.str();
    }

/// Print information about this object.

    void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << "CompressiblePotentialFlowElement #" << Id();
    }

/// Print object's data.

    void PrintData(std::ostream& rOStream) const
    {
        pGetGeometry()->PrintData(rOStream);
    }



    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:

    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{
    void ComputeLHSGaussPointContribution(
        const double weight,
        Matrix& lhs,
        const ElementalData<NumNodes,Dim>& data)
    {
        noalias(lhs) += weight*prod(data.DN_DX, trans(data.DN_DX));
    }

    void ComputeRHSGaussPointContribution(
        const double weight,
        Vector& rhs,
        const ElementalData<NumNodes,Dim>& data)
    {
        array_1d<double,Dim> grad = prod(trans(data.DN_DX), data.phis);
        noalias(rhs) -= weight*prod(data.DN_DX, grad);
    }


    void GetValuesOnSplitElement(Vector& split_element_values )
    {
        array_1d<double,NumNodes> distances;
        for(unsigned int i = 0; i<NumNodes; i++)
            distances[i] = GetGeometry()[i].FastGetSolutionStepValue(DISTANCE);

        for (unsigned int i = 0; i < NumNodes; i++)
        {
            if(distances[i] > 0)
                split_element_values[i] = GetGeometry()[i].FastGetSolutionStepValue(POSITIVE_FACE_PRESSURE);
            else
                split_element_values[i] = GetGeometry()[i].FastGetSolutionStepValue(NEGATIVE_FACE_PRESSURE);
        }

        //negative part - sign is opposite to the previous case
        for (unsigned int i = 0; i < NumNodes; i++)
        {
            if(distances[i] < 0)
                split_element_values[NumNodes+i] = GetGeometry()[i].FastGetSolutionStepValue(POSITIVE_FACE_PRESSURE);
            else
                split_element_values[NumNodes+i] = GetGeometry()[i].FastGetSolutionStepValue(NEGATIVE_FACE_PRESSURE);
        }
    }




    ///@}
    ///@name Protected Operations
    ///@{

    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{

    ///@}

private:

    ///@name Static Member Variables
    ///@{



    ///@}
    ///@name Member Variables
    ///@{



    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    ///@}
    ///@name Serialization
    ///@{

    friend class Serializer;

    virtual void save(Serializer& rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Element );
    }

    virtual void load(Serializer& rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Element );
    }

    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

}; // Class CompressiblePotentialFlowElement

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

///@}

} // namespace Kratos.

#endif // KRATOS_COMPRESSIBLE_POTENTIAL_FLOW_ELEMENT_H_INCLUDED  defined
