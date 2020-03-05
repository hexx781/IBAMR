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

#ifndef included_IBTKInit
#define included_IBTKInit

/////////////////////////////// INCLUDES /////////////////////////////////////

#include <IBTK_config.h>

#include "SAMRAI_config.h"
#include "tbox/SAMRAIManager.h"
#include "tbox/SAMRAI_MPI.h"

#include <petscsys.h>
#ifdef IBTK_HAVE_LIBMESH
#include "libmesh/libmesh.h"
#include "libmesh/reference_counter.h"
#endif

namespace IBTK
{
/**
 * @brief Initialization for IBAMR programs.
 *
 * The singleton IBTKInit class handles the initializations for PETSc, LibMesh, and SAMRAI. This object should be
 * created using the initialize() function at the start of the main() function. The destruction of the object correctly
 * closes the libraries.
 *
 */

class IBTKInit
{
public:
    /*!
     * \brief Default constructor. This function is not implemented and should not be used.
     */
    IBTKInit() = delete;

    /*!
     * \brief Copy constructor. This function is not implemented and should not be used.
     */
    IBTKInit(const IBTKInit& from) = delete;

    /*!
     * \brief Assignment operator. This function is not implemented and should not be used.
     */
    IBTKInit& operator=(const IBTKInit& that) = delete;

    using comm = MPI_Comm;

    /**
     * The initialize function creates and returns a static IBTKInit object which initializes all pertinent libraries.
     * Repeated calls to initialize() return the IBTKInit object.
     */
    static IBTKInit& initialize(int argc,
                                char** argv,
                                comm communicator = MPI_COMM_WORLD,
                                char* petsc_file = nullptr,
                                char* petsc_help = nullptr);

    /**
     * Destructor. Closes libraries appropriately.
     */
    ~IBTKInit();

#ifdef IBTK_HAVE_LIBMESH
    /**
     * Get libMesh initialization object.
     */
    libMesh::LibMeshInit& getLibMeshInit()
    {
        return d_libmesh_init;
    }
#endif

    /*!
     * \brief Check if the library has been initialized. Throw an error if it has not been initialized.
     */
    inline static bool check_initialized()
    {
        if (!s_initialized)
        {
            TBOX_ERROR(
                "IBAMR is not initialized! IBAMR must be initialized by an appropriate call to IBTKInit::initialize() "
                "prior to using any of the library.");
        }
        return s_initialized;
    }

private:
    /**
     * Constructor for IBTKInit. Initializes libraries and sets the SAMRAI world communicator.
     */
    IBTKInit(int argc,
             char** argv,
             comm communicator = MPI_COMM_WORLD,
             char* petsc_file = nullptr,
             char* petsc_help = nullptr);

#ifdef IBTK_HAVE_LIBMESH
    libMesh::LibMeshInit d_libmesh_init;
#endif
    static bool s_initialized;
};

} // namespace IBTK

#endif
