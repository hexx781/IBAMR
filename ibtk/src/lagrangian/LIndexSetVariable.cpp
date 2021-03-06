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

#include "ibtk/LIndexSetDataFactory.h"
#include "ibtk/LIndexSetVariable.h"
#include "ibtk/LSet.h"       // IWYU pragma: keep
#include "ibtk/namespaces.h" // IWYU pragma: keep

#include "IntVector.h"
#include "Variable.h"
#include "tbox/Pointer.h"

#include <string>

namespace IBTK
{
class LNode;
class LNodeIndex;
} // namespace IBTK

/////////////////////////////// NAMESPACE ////////////////////////////////////

namespace IBTK
{
/////////////////////////////// STATIC ///////////////////////////////////////

/////////////////////////////// PUBLIC ///////////////////////////////////////

template <class T>
LIndexSetVariable<T>::LIndexSetVariable(std::string name)
    : Variable<NDIM>(std::move(name), new LIndexSetDataFactory<T>(IntVector<NDIM>(0)))
{
    // intentionally blank
    return;
} // LIndexSetVariable

template <class T>
bool
LIndexSetVariable<T>::dataLivesOnPatchBorder() const
{
    return false;
} // dataLivesOnPatchBorder

template <class T>
bool
LIndexSetVariable<T>::fineBoundaryRepresentsVariable() const
{
    return true;
} // fineBoundaryRepresentsVariable

/////////////////////////////// PROTECTED ////////////////////////////////////

/////////////////////////////// PRIVATE //////////////////////////////////////

/////////////////////////////// NAMESPACE ////////////////////////////////////

} // namespace IBTK

/////////////////////////////// TEMPLATE INSTANTIATION ///////////////////////

template class IBTK::LIndexSetVariable<IBTK::LNode>;
template class IBTK::LIndexSetVariable<IBTK::LNodeIndex>;

//////////////////////////////////////////////////////////////////////////////
