// ---------------------------------------------------------------------
//
// Copyright (c) 2014 - 2019 by the IBAMR developers
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

#include "ibtk/CopyToRootTransaction.h"
#include "ibtk/namespaces.h" // IWYU pragma: keep

#include "BoxArray.h"
#include "BoxGeometry.h"
#include "BoxOverlap.h"
#include "GridGeometry.h"
#include "IntVector.h"
#include "Patch.h"
#include "PatchData.h"
#include "PatchDataFactory.h"
#include "PatchDescriptor.h"
#include "PatchLevel.h"
#include "tbox/AbstractStream.h"
#include "tbox/Pointer.h"
#include "tbox/Utilities.h"

#include <ostream>

namespace SAMRAI
{
namespace hier
{
template <int DIM>
class Box;
} // namespace hier
} // namespace SAMRAI

/////////////////////////////// NAMESPACE ////////////////////////////////////

namespace IBTK
{
/////////////////////////////// STATIC ///////////////////////////////////////

/////////////////////////////// PUBLIC ///////////////////////////////////////

CopyToRootTransaction::CopyToRootTransaction(const int src_proc,
                                             const int dst_proc,
                                             Pointer<PatchLevel<NDIM> > patch_level,
                                             const int src_patch_data_idx,
                                             Pointer<PatchData<NDIM> > dst_patch_data)
    : d_src_proc(src_proc),
      d_dst_proc(dst_proc),
      d_patch_level(patch_level),
      d_src_patch_data_idx(src_patch_data_idx),
      d_dst_patch_data(dst_patch_data)
{
    // intentionally blank
    return;
} // CopyToRootTransaction

Pointer<PatchData<NDIM> >
CopyToRootTransaction::getRootPatchData() const
{
    return d_dst_patch_data;
} // getRootPatchData

bool
CopyToRootTransaction::canEstimateIncomingMessageSize()
{
    return false;
} // canEstimateIncomingMessageSize

int
CopyToRootTransaction::computeIncomingMessageSize()
{
    return 0;
} // computeIncomingMessageSize

int
CopyToRootTransaction::computeOutgoingMessageSize()
{
    Pointer<PatchDataFactory<NDIM> > pdat_factory =
        d_patch_level->getPatchDescriptor()->getPatchDataFactory(d_src_patch_data_idx);

    Pointer<GridGeometry<NDIM> > grid_geom = d_patch_level->getGridGeometry();
#if !defined(NDEBUG)
    TBOX_ASSERT(grid_geom->getDomainIsSingleBox());
#endif
    const Box<NDIM>& dst_box = grid_geom->getPhysicalDomain()[0];
    Pointer<BoxGeometry<NDIM> > dst_box_geometry = pdat_factory->getBoxGeometry(dst_box);

    int size = AbstractStream::sizeofInt();
    for (PatchLevel<NDIM>::Iterator p(d_patch_level); p; p++)
    {
        const int src_patch_num = p();
        size += AbstractStream::sizeofInt();
        Pointer<Patch<NDIM> > patch = d_patch_level->getPatch(src_patch_num);
        const Box<NDIM>& src_box = patch->getBox();
        Pointer<BoxGeometry<NDIM> > src_box_geometry = pdat_factory->getBoxGeometry(src_box);
        const Box<NDIM>& src_mask = dst_box;
        const bool overwrite_interior = true;
        const IntVector<NDIM> src_shift = 0;
        Pointer<BoxOverlap<NDIM> > box_overlap =
            dst_box_geometry->calculateOverlap(*src_box_geometry, src_mask, overwrite_interior, src_shift);
        size += patch->getPatchData(d_src_patch_data_idx)->getDataStreamSize(*box_overlap);
    }
    return size;
} // computeOutgoingMessageSize

int
CopyToRootTransaction::getSourceProcessor()
{
    return d_src_proc;
} // getSourceProcessor

int
CopyToRootTransaction::getDestinationProcessor()
{
    return d_dst_proc;
} // getDestinationProcessor

void
CopyToRootTransaction::packStream(AbstractStream& stream)
{
    Pointer<PatchDataFactory<NDIM> > pdat_factory =
        d_patch_level->getPatchDescriptor()->getPatchDataFactory(d_src_patch_data_idx);

    Pointer<GridGeometry<NDIM> > grid_geom = d_patch_level->getGridGeometry();
#if !defined(NDEBUG)
    TBOX_ASSERT(grid_geom->getDomainIsSingleBox());
#endif
    const Box<NDIM>& dst_box = grid_geom->getPhysicalDomain()[0];
    Pointer<BoxGeometry<NDIM> > dst_box_geometry = pdat_factory->getBoxGeometry(dst_box);

    int src_patch_count = 0;
    for (PatchLevel<NDIM>::Iterator p(d_patch_level); p; p++)
    {
        ++src_patch_count;
    }
    stream << src_patch_count;

    for (PatchLevel<NDIM>::Iterator p(d_patch_level); p; p++)
    {
        const int src_patch_num = p();
        stream << src_patch_num;
        Pointer<Patch<NDIM> > patch = d_patch_level->getPatch(src_patch_num);
        const Box<NDIM>& src_box = patch->getBox();
        Pointer<BoxGeometry<NDIM> > src_box_geometry = pdat_factory->getBoxGeometry(src_box);
        const Box<NDIM>& src_mask = dst_box;
        const bool overwrite_interior = true;
        const IntVector<NDIM> src_shift = 0;
        Pointer<BoxOverlap<NDIM> > box_overlap =
            dst_box_geometry->calculateOverlap(*src_box_geometry, src_mask, overwrite_interior, src_shift);
        patch->getPatchData(d_src_patch_data_idx)->packStream(stream, *box_overlap);
    }
    return;
} // packStream

void
CopyToRootTransaction::unpackStream(AbstractStream& stream)
{
    Pointer<PatchDataFactory<NDIM> > pdat_factory =
        d_patch_level->getPatchDescriptor()->getPatchDataFactory(d_src_patch_data_idx);

    Pointer<GridGeometry<NDIM> > grid_geom = d_patch_level->getGridGeometry();
#if !defined(NDEBUG)
    TBOX_ASSERT(grid_geom->getDomainIsSingleBox());
#endif
    const Box<NDIM>& dst_box = grid_geom->getPhysicalDomain()[0];
    Pointer<BoxGeometry<NDIM> > dst_box_geometry = pdat_factory->getBoxGeometry(dst_box);

    int src_patch_count;
    stream >> src_patch_count;
    for (int p = 0; p < src_patch_count; ++p)
    {
        int src_patch_num;
        stream >> src_patch_num;
        const Box<NDIM>& src_box = d_patch_level->getBoxes()[src_patch_num];
        Pointer<BoxGeometry<NDIM> > src_box_geometry = pdat_factory->getBoxGeometry(src_box);
        const Box<NDIM>& src_mask = dst_box;
        const bool overwrite_interior = true;
        const IntVector<NDIM> src_shift = 0;
        Pointer<BoxOverlap<NDIM> > box_overlap =
            dst_box_geometry->calculateOverlap(*src_box_geometry, src_mask, overwrite_interior, src_shift);
        d_dst_patch_data->unpackStream(stream, *box_overlap);
    }
    return;
} // unpackStream

void
CopyToRootTransaction::copyLocalData()
{
    Pointer<PatchDataFactory<NDIM> > pdat_factory =
        d_patch_level->getPatchDescriptor()->getPatchDataFactory(d_src_patch_data_idx);

    Pointer<GridGeometry<NDIM> > grid_geom = d_patch_level->getGridGeometry();
#if !defined(NDEBUG)
    TBOX_ASSERT(grid_geom->getDomainIsSingleBox());
#endif
    const Box<NDIM>& dst_box = grid_geom->getPhysicalDomain()[0];
    Pointer<BoxGeometry<NDIM> > dst_box_geometry = pdat_factory->getBoxGeometry(dst_box);

    for (PatchLevel<NDIM>::Iterator p(d_patch_level); p; p++)
    {
        int src_patch_num = p();
        Pointer<Patch<NDIM> > patch = d_patch_level->getPatch(src_patch_num);
        const Box<NDIM>& src_box = patch->getBox();
        Pointer<BoxGeometry<NDIM> > src_box_geometry = pdat_factory->getBoxGeometry(src_box);
        const Box<NDIM>& src_mask = dst_box;
        const bool overwrite_interior = true;
        const IntVector<NDIM> src_shift = 0;
        Pointer<BoxOverlap<NDIM> > box_overlap =
            dst_box_geometry->calculateOverlap(*src_box_geometry, src_mask, overwrite_interior, src_shift);
        d_dst_patch_data->copy(*patch->getPatchData(d_src_patch_data_idx), *box_overlap);
    }
    return;
} // copyLocalData

void
CopyToRootTransaction::printClassData(std::ostream& stream) const
{
    stream << "CopyToRootTransaction::printClassData() is not implemented\n";
    return;
} // printClassData

/////////////////////////////// PROTECTED ////////////////////////////////////

/////////////////////////////// PRIVATE //////////////////////////////////////

/////////////////////////////// NAMESPACE ////////////////////////////////////

} // namespace IBTK

//////////////////////////////////////////////////////////////////////////////
