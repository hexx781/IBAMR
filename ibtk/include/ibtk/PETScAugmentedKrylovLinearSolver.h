// Filename: PETScAugmentedKrylovLinearSolver.h
// Created on 04 Apr 2016 by Boyce Griffith
//
// Copyright (c) 2002-2014, Boyce Griffith
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of The University of North Carolina nor the names of
//      its contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef included_PETScAugmentedKrylovLinearSolver
#define included_PETScAugmentedKrylovLinearSolver

/////////////////////////////// INCLUDES /////////////////////////////////////

#include <iosfwd>
#include <stddef.h>
#include <string>
#include <vector>

#include "IntVector.h"
#include "MultiblockDataTranslator.h"
#include "SAMRAIVectorReal.h"
#include "ibtk/KrylovLinearSolver.h"
#include "ibtk/LinearSolver.h"
#include "mpi.h"
#include "petscksp.h"
#include "petscmat.h"
#include "petscpc.h"
#include "petscsys.h"
#include "petscvec.h"
#include "tbox/Database.h"
#include "tbox/Pointer.h"

namespace IBTK
{
class LinearOperator;
} // namespace IBTK

/////////////////////////////// FORWARD DECLARATION //////////////////////////

namespace IBTK
{
}

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBTK
{
/*!
 * \brief Class PETScAugmentedKrylovLinearSolver provides a KrylovLinearSolver interface
 * for a <A HREF="http://www.mcs.anl.gov/petsc">PETSc</A> Krylov subspace
 * iterative linear solver (KSP).
 *
 * This solver class provides access to a large number of Krylov subspace
 * solvers for linear problems of the form \f$Ax=b\f$ using the PETSc KSP linear
 * solver interface.  See <A
 *
 HREF="http://www.mcs.anl.gov/petsc/documentation/linearsolvertable.html">http://www.mcs.anl.gov/petsc/documentation/linearsolvertable.html</A>
 * for a complete list of the Krylov solvers provided by this class.  Note that
 * solver configuration is typically done at runtime via command line options.
 *
 * \note
 * - Preconditioners and direct solvers provided by PETSc \em cannot be used
 *   within the present IBTK solver framework.  However, this <em>does not
 *   mean</em> that preconditioners cannot be used with the
 *   PETScAugmentedKrylovLinearSolver class; see, for instance, classes FACPreconditioner
 *   and CCPoissonPointRelaxationFACOperator.  \par
 * - Users have the \em option of supplying the PETScAugmentedKrylovLinearSolver class
 *   constructor with a KSP object.  It is important to emphasize that doing so
 *   is optional.  When a KSP object is provided to the class constructor, the
 *   PETScAugmentedKrylovLinearSolver class acts as a \em wrapper for the supplied KSP
 *   object.  \par
 * - The functionality of the PETScAugmentedKrylovLinearSolver class is essentially
 *   identical regardless as to whether a PETSc KSP object is provided to the
 *   class constructor.  The main exception is that when an external KSP object
 *   is provided to the class constructor, memory management of that object is
 *   \em NOT handled by the PETScAugmentedKrylovLinearSolver.  In particular, it is the
 *   caller's responsibility to ensure that the supplied KSP object is properly
 *   destroyed via KSPDestroy().
 *
 * Sample parameters for initialization from database (and their default
 * values): \verbatim

 options_prefix = ""           // see setOptionsPrefix()
 ksp_type = "gmres"            // see setKSPType()
 initial_guess_nonzero = TRUE  // see setInitialGuessNonzero()
 rel_residual_tol = 1.0e-5     // see setRelativeTolerance()
 abs_residual_tol = 1.0e-50    // see setAbsoluteTolerance()
 max_iterations = 10000        // see setMaxIterations()
 enable_logging = FALSE        // see setLoggingEnabled()
 \endverbatim
 *
 * PETSc is developed in the Mathematics and Computer Science (MCS) Division at
 * Argonne National Laboratory (ANL).  For more information about PETSc, see <A
 * HREF="http://www.mcs.anl.gov/petsc">http://www.mcs.anl.gov/petsc</A>.
 */
class PETScAugmentedKrylovLinearSolver : public KrylovLinearSolver
{
public:
    friend class PETScNewtonKrylovSolver;

    /*!
     * \brief Constructor for a concrete KrylovLinearSolver that employs the
     * PETSc KSP solver framework.
     */
    PETScAugmentedKrylovLinearSolver(const std::string& object_name,
                                     SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> input_db,
                                     const std::string& default_options_prefix,
                                     MPI_Comm petsc_comm = PETSC_COMM_WORLD);

    /*!
     * \brief Destructor.
     */
    ~PETScAugmentedKrylovLinearSolver();

    /*!
     * \brief Set the KSP type.
     */
    void setKSPType(const std::string& ksp_type);

    /*!
     * \brief Set the options prefix used by this PETSc solver object.
     */
    void setOptionsPrefix(const std::string& options_prefix);

    /*!
     * \name Functions to access the underlying PETSc objects.
     */
    //\{

    /*!
     * \brief Get the PETSc KSP object.
     */
    const KSP& getPETScKSP() const;

    //\}

    /*!
     * \name Krylov solver functionality.
     */
    //\{

    /*!
     * \brief Set the linear operator used when solving \f$Ax=b\f$.
     */
    void setOperator(SAMRAI::tbox::Pointer<LinearOperator> A);

    /*!
     * \brief Set the preconditioner used by the Krylov subspace method when
     * solving \f$Ax=b\f$.
     *
     * \note If the preconditioner is NULL, no preconditioning is performed.
     */
    void setPreconditioner(SAMRAI::tbox::Pointer<LinearSolver> pc_solver = NULL);
#if 0
    /*!
     * \brief Set the nullspace of the linear system.
     *
     * Basis vectors must be orthogonal but are not required to be orthonormal.
     * Basis vectors will be normalized automatically.
     */
    void setNullspace(
        bool contains_constant_vec,
        const std::vector<SAMRAI::tbox::Pointer<SAMRAI::solv::SAMRAIVectorReal<NDIM, double> > >& nullspace_basis_vecs =
            std::vector<SAMRAI::tbox::Pointer<SAMRAI::solv::SAMRAIVectorReal<NDIM, double> > >());
#endif
    /*!
     * \brief Solve the linear system of equations \f$Ax=b\f$ for \f$x\f$.
     *
     * Before calling solveSystem(), the form of the solution \a x and
     * right-hand-side \a b vectors must be set properly by the user on all
     * patch interiors on the specified range of levels in the patch hierarchy.
     * The user is responsible for all data management for the quantities
     * associated with the solution and right-hand-side vectors.  In particular,
     * patch data in these vectors must be allocated prior to calling this
     * method.
     *
     * \param x solution vector
     * \param b right-hand-side vector
     *
     * <b>Conditions on Parameters:</b>
     * - vectors \a x and \a b must have same patch hierarchy
     * - vectors \a x and \a b must have same structure, depth, etc.
     *
     * \note The vector arguments for solveSystem() need not match those for
     * initializeSolverState().  However, there must be a certain degree of
     * similarity, including:\par
     * - hierarchy configuration (hierarchy pointer and range of levels)
     * - number, type and alignment of vector component data
     * - ghost cell widths of data in the solution \a x and right-hand-side \a b
     *   vectors
     *
     * \note The solver need not be initialized prior to calling solveSystem();
     * however, see initializeSolverState() and deallocateSolverState() for
     * opportunities to save overhead when performing multiple consecutive
     * solves.
     *
     * \see initializeSolverState
     * \see deallocateSolverState
     *
     * \return \p true if the solver converged to the specified tolerances, \p
     * false otherwise
     */
    bool solveSystem(SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& x, SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& b);

    /*!
     * \brief Compute hierarchy dependent data required for solving \f$Ax=b\f$.
     *
     * By default, the solveSystem() method computes some required hierarchy
     * dependent data before solving and removes that data after the solve.  For
     * multiple solves that use the same hierarchy configuration, it is more
     * efficient to:
     *
     * -# initialize the hierarchy-dependent data required by the solver via
     *    initializeSolverState(),
     * -# solve the system one or more times via solveSystem(), and
     * -# remove the hierarchy-dependent data via deallocateSolverState().
     *
     * Note that it is generally necessary to reinitialize the solver state when
     * the hierarchy configuration changes.
     *
     * When linear operator or preconditioner objects have been registered with
     * this class via setOperator() and setPreconditioner(), they are also
     * initialized by this member function.
     *
     * \param x solution vector
     * \param b right-hand-side vector
     *
     * <b>Conditions on Parameters:</b>
     * - vectors \a x and \a b must have same patch hierarchy
     * - vectors \a x and \a b must have same structure, depth, etc.
     *
     * \note The vector arguments for solveSystem() need not match those for
     * initializeSolverState().  However, there must be a certain degree of
     * similarity, including:\par
     * - hierarchy configuration (hierarchy pointer and range of levels)
     * - number, type and alignment of vector component data
     * - ghost cell widths of data in the solution \a x and right-hand-side \a b
     *   vectors
     *
     * \note It is safe to call initializeSolverState() when the state is
     * already initialized.  In this case, the solver state is first deallocated
     * and then reinitialized.
     *
     * \see deallocateSolverState
     */
    void initializeSolverState(const SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& x,
                               const SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& b);

    /*!
     * \brief Remove all hierarchy dependent data allocated by
     * initializeSolverState().
     *
     * When linear operator or preconditioner objects have been registered with
     * this class via setOperator() and setPreconditioner(), they are also
     * deallocated by this member function.
     *
     * \note It is safe to call deallocateSolverState() when the solver state is
     * already deallocated.
     *
     * \see initializeSolverState
     */
    void deallocateSolverState();

    //\}

    /*!
     * \brief Extra vectors used in solving the system.
     *
     * \note These vectors must be setup \em prior to calling solveSystem().  They may optionally be initialized by
     * initializeSolverStateAugmented().
     */
    Vec extra_x_vec, extra_b_vec;

protected:
    /*!
     * \brief Initialize augmented solver state data.
     *
     * This routine is called after initializing regular solver data structures.
     *
     * \note A default empty implementation is provided for cases in which no extra setup is needed.
     */
    virtual void initializeSolverStateAugmented(const SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& x,
                                                const SAMRAI::solv::SAMRAIVectorReal<NDIM, double>& b);

    /*!
     * \brief Matrix-vector multiply for the augmented system.
     */
    virtual void MatVecMultAugmented(Vec x, Vec y) = 0;

    /*!
     * \brief Apply the preconditioner for the augmented system.
     *
     * \note A default implementation is provided that applies the Cartesian grid preconditioner to the SAMRAI data and
     * the identity operator to the extra data.
     */
    virtual void PCApplyAugmented(Vec x, Vec y);

private:
    /*!
     * \brief Default constructor.
     *
     * \note This constructor is not implemented and should not be used.
     */
    PETScAugmentedKrylovLinearSolver();

    /*!
     * \brief Copy constructor.
     *
     * \note This constructor is not implemented and should not be used.
     *
     * \param from The value to copy to this object.
     */
    PETScAugmentedKrylovLinearSolver(const PETScAugmentedKrylovLinearSolver& from);

    /*!
     * \brief Assignment operator.
     *
     * \note This operator is not implemented and should not be used.
     *
     * \param that The value to assign to this object.
     *
     * \return A reference to this object.
     */
    PETScAugmentedKrylovLinearSolver& operator=(const PETScAugmentedKrylovLinearSolver& that);

    /*!
     * \brief Common routine used by all class constructors.
     */
    void common_ctor();

    /*!
     * \brief Report the KSPConvergedReason.
     */
    void reportKSPConvergedReason(const KSPConvergedReason& reason, std::ostream& os) const;

    /*!
     * \brief Reset the values of the convergence tolerances for the PETSc KSP
     * object.
     */
    void resetKSPOptions();

    /*!
     * \brief Reset the KSP operators to correspond to the supplied
     * LinearOperator.
     */
    void resetKSPOperators();

    /*!
     * \brief Reset the KSP PC to correspond to the supplied preconditioner.
     */
    void resetKSPPC();

    /*!
     * \name Static functions for use by PETSc KSP and MatShell objects.
     */
    //\{

    /*!
     * \brief Compute the matrix vector product \f$y=Ax\f$.
     */
    static PetscErrorCode MatVecMult_SAMRAI(Mat A, Vec x, Vec y);

    /*!
     * \brief Apply the preconditioner to \a x and store the result in \a y.
     */
    static PetscErrorCode PCApply_SAMRAI(PC pc, Vec x, Vec y);

    //\}

    std::string d_ksp_type;

    bool d_reinitializing_solver;

    Vec d_petsc_x, d_petsc_b;

    std::string d_options_prefix;

    MPI_Comm d_petsc_comm;
    KSP d_petsc_ksp;
    Mat d_petsc_mat;
};
} // namespace IBTK

//////////////////////////////////////////////////////////////////////////////

#endif //#ifndef included_PETScAugmentedKrylovLinearSolver