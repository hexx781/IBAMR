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

#include "ibtk/IBTKInit.h"
#include "ibtk/IBTK_MPI.h"
#include "ibtk/app_namespaces.h"

namespace IBTK
{
/////////////////////////////// STATIC //////////////////////////////////////
bool IBTKInit::s_initialized = false;

IBTKInit&
IBTKInit::initialize(int argc, char** argv, IBTK_MPI::comm communicator, char* petsc_file, char* petsc_help)
{
    if (s_initialized)
    {
        TBOX_WARNING("IBAMR already initialized. Multiple initialize() calls will be ignored.");
    }
    static IBTKInit instance(argc, argv, communicator, petsc_file, petsc_help);
    return instance;
}

/////////////////////////////// PRIVATE //////////////////////////////////////

IBTKInit::IBTKInit(int argc, char** argv, IBTK_MPI::comm communicator, char* petsc_file, char* petsc_help)
#ifdef IBTK_HAVE_LIBMESH
    : d_libmesh_init(argc, argv, communicator)
#endif
{
#ifdef IBTK_HAVE_LIBMESH
    libMesh::ReferenceCounter::disable_print_counter_info();
    NULL_USE(petsc_file);
    NULL_USE(petsc_help);
#else
    // We need to initialize PETSc.
    PetscInitialize(&argc, &argv, petsc_file, petsc_help);
#endif
#if SAMRAI_VERSION_MAJOR > 2
    SAMRAIManager::initialize();
    SAMRAI_MPI::init(comm);
    SAMRAIManager::setMaxNumberPatchDataEntries(std::max(2048, SAMRAIManager::getMaxNumberPatchDataEntries()));
#else
    SAMRAIManager::setMaxNumberPatchDataEntries(2048);
#endif
    SAMRAI_MPI::setCommunicator(communicator);
    SAMRAI_MPI::setCallAbortInSerialInsteadOfExit();
    SAMRAIManager::startup();
    s_initialized = true;
}

IBTKInit::~IBTKInit()
{
    pout << "IBTKInit destructor called. Shutting down libraries.\n";
    SAMRAIManager::shutdown();
#if SAMRAI_VERSION_MAJOR > 2
    SAMRAIManager::finalize();
#endif
#ifndef IBTK_HAVE_LIBMESH
    PetscFinalize();
#endif
    s_initialized = false;
}
} // namespace IBTK
