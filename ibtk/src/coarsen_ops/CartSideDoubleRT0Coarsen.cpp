// ---------------------------------------------------------------------
//
// Copyright (c) 2015 - 2019 by the IBAMR developers
// All rights reserved.
//
// This file is part of IBAMR.
//
// IBAMR is free software and is distributed under the 3-clause BSD
// license. The full text of the license can be found in the file
// COPYRIGHT at the top level directory of IBAMR.
//
// ---------------------------------------------------------------------

/////////////////////////////// INCLUDES /////////////////////////////////////

#include "IBTK_config.h"

#include "ibtk/CartSideDoubleRT0Coarsen.h"
#include "ibtk/ibtk_utilities.h"
#include "ibtk/namespaces.h" // IWYU pragma: keep

#include "Box.h"
#include "Index.h"
#include "IntVector.h"
#include "Patch.h"
#include "SideData.h"
#include "SideVariable.h"
#include "tbox/Pointer.h"
#include "tbox/Utilities.h"

#include <ostream>
#include <string>

namespace SAMRAI
{
namespace hier
{
template <int DIM>
class Variable;
} // namespace hier
} // namespace SAMRAI

// FORTRAN ROUTINES
#if (NDIM == 2)
#define SC_RT0_COARSEN_FC IBTK_FC_FUNC(scrt0coarsen2d, SCRT0COARSEN2D)
#endif
#if (NDIM == 3)
#define SC_RT0_COARSEN_FC IBTK_FC_FUNC(scrt0coarsen3d, SCRT0COARSEN3D)
#endif

// Function interfaces
extern "C"
{
    void SC_RT0_COARSEN_FC(double* U_coarse0,
                           double* U_coarse1,
#if (NDIM == 3)
                           double* U_coarse2,
#endif
                           const int& U_crse_gcw,
                           const double* U_fine0,
                           const double* U_fine1,
#if (NDIM == 3)
                           const double* U_fine2,
#endif
                           const int& U_fine_gcw,
                           const int& ilowerc0,
                           const int& iupperc0,
                           const int& ilowerc1,
                           const int& iupperc1,
#if (NDIM == 3)
                           const int& ilowerc2,
                           const int& iupperc2,
#endif
                           const int& ilowerf0,
                           const int& iupperf0,
                           const int& ilowerf1,
                           const int& iupperf1,
#if (NDIM == 3)
                           const int& ilowerf2,
                           const int& iupperf2,
#endif
                           const int* ratio_to_coarser,
                           const int* fblower,
                           const int* fbupper);
}

/////////////////////////////// NAMESPACE ////////////////////////////////////

namespace IBTK
{
/////////////////////////////// STATIC ///////////////////////////////////////

const std::string CartSideDoubleRT0Coarsen::s_op_name = "RT0_COARSEN";

namespace
{
static const int COARSEN_OP_PRIORITY = 0;
}

/////////////////////////////// PUBLIC ///////////////////////////////////////

CartSideDoubleRT0Coarsen::CartSideDoubleRT0Coarsen(IntVector<NDIM> gcw) : d_gcw(std::move(gcw))
{
    // intentionally blank
    return;
} // CartSideDoubleRT0Coarsen

bool
CartSideDoubleRT0Coarsen::findCoarsenOperator(const Pointer<Variable<NDIM> >& var, const std::string& op_name) const
{
    Pointer<SideVariable<NDIM, double> > sc_var = var;
    return (sc_var && op_name == s_op_name);
} // findCoarsenOperator

const std::string&
CartSideDoubleRT0Coarsen::getOperatorName() const
{
    return s_op_name;
} // getOperatorName

int
CartSideDoubleRT0Coarsen::getOperatorPriority() const
{
    return COARSEN_OP_PRIORITY;
} // getOperatorPriority

IntVector<NDIM>
CartSideDoubleRT0Coarsen::getStencilWidth() const
{
    return d_gcw;
} // getStencilWidth

void
CartSideDoubleRT0Coarsen::coarsen(Patch<NDIM>& coarse,
                                  const Patch<NDIM>& fine,
                                  const int dst_component,
                                  const int src_component,
                                  const Box<NDIM>& coarse_box,
                                  const IntVector<NDIM>& ratio) const
{
    Pointer<SideData<NDIM, double> > cdata = coarse.getPatchData(dst_component);
    Pointer<SideData<NDIM, double> > fdata = fine.getPatchData(src_component);
    const int U_fine_ghosts = (fdata->getGhostCellWidth()).max();
    const int U_crse_ghosts = (cdata->getGhostCellWidth()).max();
#if !defined(NDEBUG)
    if (U_fine_ghosts != (fdata->getGhostCellWidth()).min())
    {
        TBOX_ERROR("CartSideDoubleRT0Coarsen::coarsen():\n"
                   << "   fine patch data does not have uniform ghost cell widths" << std::endl);
    }
    if (U_crse_ghosts != (cdata->getGhostCellWidth()).min())
    {
        TBOX_ERROR("CartSideDoubleRT0Coarsen::coarsen():\n"
                   << "   coarse patch data does not have uniform ghost cell widths" << std::endl);
    }
    for (unsigned int d = 0; d < NDIM; ++d)
    {
        if (d_gcw(d) + 1 < ratio(d))
        {
            TBOX_ERROR(
                "CartSideDoubleRT0Coarsen::coarsen():\n"
                << "   invalid refinement ratio between coarse and fine index spaces for specified ghost cell width"
                << std::endl);
        }
    }
#endif
    const int data_depth = cdata->getDepth();
#if !defined(NDEBUG)
    TBOX_ASSERT(data_depth == fdata->getDepth());
#endif
    const Box<NDIM>& patch_box_fine = fine.getBox();
    const Box<NDIM>& patch_box_crse = coarse.getBox();
    for (int depth = 0; depth < data_depth; ++depth)
    {
        double* const U_crse0 = cdata->getPointer(0, depth);
        double* const U_crse1 = cdata->getPointer(1, depth);
#if (NDIM == 3)
        double* const U_crse2 = cdata->getPointer(2, depth);
#endif
        const double* const U_fine0 = fdata->getPointer(0, depth);
        const double* const U_fine1 = fdata->getPointer(1, depth);
#if (NDIM == 3)
        const double* const U_fine2 = fdata->getPointer(2, depth);
#endif
        SC_RT0_COARSEN_FC(U_crse0,
                          U_crse1,
#if (NDIM == 3)
                          U_crse2,
#endif
                          U_crse_ghosts,
                          U_fine0,
                          U_fine1,
#if (NDIM == 3)
                          U_fine2,
#endif
                          U_fine_ghosts,
                          patch_box_crse.lower(0),
                          patch_box_crse.upper(0),
                          patch_box_crse.lower(1),
                          patch_box_crse.upper(1),
#if (NDIM == 3)
                          patch_box_crse.lower(2),
                          patch_box_crse.upper(2),
#endif
                          patch_box_fine.lower(0),
                          patch_box_fine.upper(0),
                          patch_box_fine.lower(1),
                          patch_box_fine.upper(1),
#if (NDIM == 3)
                          patch_box_fine.lower(2),
                          patch_box_fine.upper(2),
#endif
                          ratio,
                          coarse_box.lower(),
                          coarse_box.upper());
    }
    return;
} // coarsen

/////////////////////////////// PROTECTED ////////////////////////////////////

/////////////////////////////// PRIVATE //////////////////////////////////////

/////////////////////////////// NAMESPACE ////////////////////////////////////

} // namespace IBTK

//////////////////////////////////////////////////////////////////////////////
