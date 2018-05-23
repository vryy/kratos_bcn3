//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:
//

#if !defined(KRATOS_READ_MATERIALS_H_INCLUDED )
#define  KRATOS_READ_MATERIALS_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "processes/process.h"
#include "includes/kratos_parameters.h"
#include "includes/io.h"

namespace Kratos {

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

/// Process to read constitutive law and material properties from a json file
/** This process reads constitutive law and material properties from a json file
 * and assign them to elements and conditions.
 */
class KRATOS_API(STRUCTURAL_MECHANICS_APPLICATION) ReadMaterialProcess : public Process
{
  public:

    ///@name Type Definitions
    ///@{

    ///@}
    ///@name Pointer Definitions
    /// Pointer definition of ReadMaterialProcess
    KRATOS_CLASS_POINTER_DEFINITION(ReadMaterialProcess);

    //typedef std::size_t SizeType;

    //typedef ModelPart::NodeType::DofsContainerType DofsContainerType;
    
    ///@}
    ///@name Life Cycle
    ///@{

    ReadMaterialProcess(ModelPart &rModelPart,
                        Parameters parameters);

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

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
    virtual std::string Info() const override {
        return "ReadMaterialProcess";
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override {
        rOStream << "ReadMaterialProcess";
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override {
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

    ModelPart& mrModelPart;

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{
    
    void AssignPropertyBlock(Parameters);

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

}; // Class ReadMaterialProcess

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

///@}

}  // namespace Kratos.

#endif // KRATOS_READ;MATERIALS_H_INCLUDED defined
