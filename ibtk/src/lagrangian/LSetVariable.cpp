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

#include "ibtk/LSet.h" // IWYU pragma: keep
#include "ibtk/LSetDataFactory.h"
#include "ibtk/LSetVariable.h"
#include "ibtk/namespaces.h" // IWYU pragma: keep

#include "IntVector.h"
#include "Variable.h"
#include "tbox/Pointer.h"

#include <string>

namespace IBTK
{
class LMarker;
class LNode;
class LNodeIndex;
} // namespace IBTK

/////////////////////////////// NAMESPACE ////////////////////////////////////

namespace IBTK
{
/////////////////////////////// STATIC ///////////////////////////////////////

/////////////////////////////// PUBLIC ///////////////////////////////////////

template <class T>
LSetVariable<T>::LSetVariable(std::string name)
    : Variable<NDIM>(std::move(name), new LSetDataFactory<T>(IntVector<NDIM>(0)))
{
    // intentionally blank
    return;
} // LSetVariable

template <class T>
bool
LSetVariable<T>::dataLivesOnPatchBorder() const
{
    return false;
} // dataLivesOnPatchBorder

template <class T>
bool
LSetVariable<T>::fineBoundaryRepresentsVariable() const
{
    return true;
} // fineBoundaryRepresentsVariable

/////////////////////////////// PROTECTED ////////////////////////////////////

/////////////////////////////// PRIVATE //////////////////////////////////////

/////////////////////////////// NAMESPACE ////////////////////////////////////

} // namespace IBTK

/////////////////////////////// TEMPLATE INSTANTIATION ///////////////////////

template class IBTK::LSetVariable<IBTK::LMarker>;
template class IBTK::LSetVariable<IBTK::LNode>;
template class IBTK::LSetVariable<IBTK::LNodeIndex>;

//////////////////////////////////////////////////////////////////////////////
