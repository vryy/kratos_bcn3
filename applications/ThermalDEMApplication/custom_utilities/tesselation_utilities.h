//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics ThermalDEM Application
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Rafael Rangel (rrangel@cimne.upc.edu)
//

#ifndef TESSELATION_UTILITIES_H
#define	TESSELATION_UTILITIES_H

// System includes

// External includes
#ifndef TETLIBRARY
#define TETLIBRARY
#endif

#if !defined(KRATOS_TETGEN_EXTERNAL_H_INCLUDED)
#define KRATOS_TETGEN_EXTERNAL_H_INCLUDED
#include "tetgen.h"
#endif

#ifndef TRILIBRARY
#define TRILIBRARY
#endif

#include "triangle.h"
#include "includes/model_part.h"

// Project includes
#include "custom_elements/thermal_spheric_particle.h"

namespace Kratos {

  extern "C" {
    void triangulate(char*, struct triangulateio*, struct triangulateio*, struct triangulateio*);
    void trifree(void*);
  }

  class KRATOS_API(THERMAL_DEM_APPLICATION) TesselationUtilities {

  public:

    KRATOS_CLASS_POINTER_DEFINITION(TesselationUtilities);

    // Constructor / destructor methods
    TesselationUtilities();
    ~TesselationUtilities();

    // Public methods
    void ExecuteInitialize(ModelPart & rModelPart, bool update_voronoi, bool update_porosity);
    void ExecuteInitializeSolutionStep(ModelPart& rModelPart);

  protected:
    // Protected methods
    void Triangulation         (ModelPart& rModelPart);
    void Tetrahedralization    (ModelPart& rModelPart);
    void UpdateVoronoi2D       (ModelPart& rModelPart, struct triangulateio& rOut, struct triangulateio& rVorOut);
    void UpdateVoronoi3D       (ModelPart& rModelPart, struct tetgenio& rOut);
    void UpdatePorosity2D      (ModelPart& rModelPart, struct triangulateio& rOut, struct triangulateio& rVorOut);
    void UpdatePorosity3D      (ModelPart& rModelPart, struct tetgenio& rOut);
    void ComputeAlphaRadius2D  (ModelPart& rModelPart, struct triangulateio& rOut);
    void ComputeAlphaRadius3D  (ModelPart& rModelPart, struct tetgenio& rOut);
    bool AlphaShape2D          (std::vector<double>& coords);
    bool AlphaShape3D          (std::vector<double>& coords);
    void AddParticleArea       (ModelPart& rModelPart, std::vector<int>& addedParticle, double& particle_area,   const int id);
    void AddParticleVolume     (ModelPart& rModelPart, std::vector<int>& addedParticle, double& particle_volume, const int id);
    void ClearTriangle         (struct triangulateio& rTr);
    void FreeTriangle          (struct triangulateio& rTr);

    // Protected attributes
    bool        mUpdateVoronoi;
    bool        mUpdatePorosiy;
    double      mAlphaRadius;
    std::string mSwitches;

  private:
    // Assignment operator
    TesselationUtilities& operator=(TesselationUtilities const& rOther);
  };

} // namespace Kratos

#endif  // TESSELATION_UTILITIES_H