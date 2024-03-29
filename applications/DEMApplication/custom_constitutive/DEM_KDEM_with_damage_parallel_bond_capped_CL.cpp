// System includes
#include <string>
#include <iostream>
#include <cmath>

// Project includes
#include "DEM_KDEM_with_damage_parallel_bond_capped_CL.h"
#include "custom_elements/spheric_continuum_particle.h"

namespace Kratos {

    DEMContinuumConstitutiveLaw::Pointer DEM_KDEM_with_damage_parallel_bond_capped::Clone() const {
        DEMContinuumConstitutiveLaw::Pointer p_clone(new DEM_KDEM_with_damage_parallel_bond_capped(*this));
        return p_clone;
    }

    void DEM_KDEM_with_damage_parallel_bond_capped::Check(Properties::Pointer pProp) const {

        DEM_KDEM_with_damage_parallel_bond::Check(pProp);

        if (!pProp->Has(CONTACT_SIGMA_MIN)) {
            KRATOS_WARNING("DEM") << std::endl;
            KRATOS_WARNING("DEM") << "WARNING: Variable CONTACT_SIGMA_MIN was not found in the Properties when using DEM_KDEM_with_damage_parallel_bond_capped. A default value of 0.0 was assigned." << std::endl;
            KRATOS_WARNING("DEM") << std::endl;
            pProp->GetValue(CONTACT_SIGMA_MIN) = 0.0;
        }
    }

    double DEM_KDEM_with_damage_parallel_bond_capped::GetContactSigmaMax() {

        KRATOS_TRY

        const double& sigma_max_capped = (*mpProperties)[CONTACT_SIGMA_MIN];
        return sigma_max_capped;

        KRATOS_CATCH("")
    }
} // namespace Kratos
