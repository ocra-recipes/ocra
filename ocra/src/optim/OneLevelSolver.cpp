#include "ocra/optim/OneLevelSolver.h"

using namespace ocra;




/** Initialize an abstract wOcra Solver.
 *
 */
OneLevelSolver::OneLevelSolver()
    : ocra::Solver()
    , ocra::NamedInstance("OneLevelSolver")
{

}

/** Destructor
 *
 */
OneLevelSolver::~OneLevelSolver()
{

}

/** I don't really know, I suppose to print internal values when solution is found.
 *
 * Actually does nothing.
 */
void OneLevelSolver::printValuesAtSolution()
{
    std::cout<<"TODO: 'printValuesAtSolution' method not implemented yet!"<<std::endl;
}

/** Add a quadratic objective to the controller.
 *
 * \param obj The quadratic objective to add
 *
 * This objective is saved in an internal vector of objectives.
 */
void OneLevelSolver::addObjective(ocra::QuadraticObjective& obj)
{
    internalAddObjective(obj);
    _objectives.push_back(&obj);
}

/** Remove a quadratic objective to the controller.
 *
 * \param obj The quadratic objective to remove
 *
 * This objective is removed from the internal vector of objectives.
 */
void OneLevelSolver::removeObjective(ocra::QuadraticObjective& obj)
{
    internalRemoveObjective(obj);
    _objectives.erase(std::find(_objectives.begin(), _objectives.end(), &obj));
}

/** Add a linear constraint to the controller.
 *
 * \param constraint The linear constraint to add
 *
 * This constraint is saved in an internal vector of equality or inequality constraints, depending on the type of \a constraint.
 */
void OneLevelSolver::addConstraint(ocra::LinearConstraint& constraint)
{
    internalAddConstraint(constraint);
    if(constraint.isEquality())
        _equalityConstraints.push_back(&constraint);
    else
        _inequalityConstraints.push_back(&constraint);
}

/** Removea linear constraint to the controller.
 *
 * \param constraint The linear constraint to remove
 */
void OneLevelSolver::removeConstraint(ocra::LinearConstraint& constraint)
{
    internalRemoveConstraint(constraint);
    if(constraint.isEquality())
        _equalityConstraints.erase(std::find(_equalityConstraints.begin(), _equalityConstraints.end(), &constraint));
    else
        _inequalityConstraints.erase(std::find(_inequalityConstraints.begin(), _inequalityConstraints.end(), &constraint));
}




/** Prepare the basic matrices, which can be considered as the task \f$ T_0 \f$  because it minimizes the whole problem vector with very low coefficient.
 *
 */
void OneLevelSolver::prepareMatrices()
{
    Xsolution.resize(n());
}

/** Do prepare, meaning prepare matrices before solving.
 *
 * It calls in this order:
 *
 *      - #prepareMatrices()
 *      - #updateObjectiveEquations()
 *      - #updateConstraintEquations()
 */
void OneLevelSolver::doPrepare()
{
    // prepareRecorder.initializeTime();

    prepareMatrices();
    updateObjectiveEquations();
    updateConstraintEquations();

    // prepareRecorder.saveRelativeTime();
}

/** Do conclude, meaning what to do after solving.
 *
 * Actually does nothing.
 */
void OneLevelSolver::doConclude()
{

}


/** Write information about solver performances in a string stream.
 *
 * \param outstream the output stream where to write the performances information
 * \param addCommaAtEnd If true, add a comma at the end of the stream. If false, it means that this is the end of the json file, nothing will be added after that, no comma is added.
 *
 * See Orocos_wOcraController::getPerformances() to know more. Here it saves:
 *
 *  - solver_prepare
 *  - solver_solve
 */
void OneLevelSolver::writePerformanceInStream(std::ostream& outstream, bool addCommaAtEnd)
{
    // prepareRecorder.writeInStream("solver_prepare", outstream, true);
    // solveRecorder.writeInStream("solver_solve", outstream, addCommaAtEnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Reduce the set of equality constraints if it is not full rank.
 *
 * \param A The matrix associated to the set of equality constraints
 * \param b The vector associated to the set of equality constraints
 * \param Ar The matrix instance where will be saved the reduced version of \a A
 * \param br The vector instance where will be saved the reduced version of \a b
 * \param tolerance The tolerance of reduction
 *
 * To reduce the set of equality constraints, we make a SVD decomposition \f$ \A = U . S . V\tp \f$.
 * Then we check the dimension (denoted by \f$ r \f$) of the diagonal \f$ S \f$ where value are greater than the tolerance.
 *
 * The reduced set of task is then (noted in a python way):
 *
 * \f{align*}{
 *      \A_r &= S[0:r, :] V[:, 0:r] \tp
 *      & &
 *      & \b_r &= U[:, 0:r] \tp \b
 * \f}
 *
 */
void OneLevelSolver::reduceConstraints(const Eigen::MatrixXd& A, const Eigen::VectorXd& b, Eigen::MatrixXd& Ar, Eigen::VectorXd& br, double tolerance)
{
    if (A.rows()>0)
    {
        Eigen::JacobiSVD<Eigen::MatrixXd> svdOfMat(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
        Eigen::VectorXd S  = svdOfMat.singularValues();
        Eigen::MatrixXd Ur = svdOfMat.matrixU();
        Eigen::MatrixXd Vr = svdOfMat.matrixV();

        int r;
        for (r = 0; r<S.size(); r++)
        {
            if (S[r] <= tolerance)
                break;
        }

        Eigen::VectorXd Sr = S.segment(0,r);
        Ur.conservativeResize(Eigen::NoChange, r);
        Vr.conservativeResize(Eigen::NoChange, r);

        Ar = Sr.asDiagonal() * Vr.transpose();
        br = Ur.transpose() * b;
    }
    else
    {
        Ar = A;
        br = b;
    }
}


/** Set the level of a particular objective registered  in the solver.
 *
 * \param obj The objective instance with a new level
 * \param level the new level
 *
 * Actually, as this solver considers that all tasks have the same level, this function does nothing.
 */
void OneLevelSolver::setObjectiveLevel(ocra::QuadraticObjective& obj, int level)
{
    //std::cout<<"[OneLevelSolver::setObjectiveLevel] Warning: solver doesn't take into account level for task.\n";
}



//#include <sstream>

/** I don't really know, I support to write the problem as a string.
 *
 * Actually does nothing.
 */
std::string OneLevelSolver::toString() const
{
    std::stringstream ss;
    ss<<"-----------------------------------------------\n";
    ss<<"probvar:\n";
    getProblemVariable().printSubTree(3,ss);
    ss<<"C:\n"<<_C<<"\n";
    ss<<"d:\n"<<-_d.transpose()<<"\n";
    ss<<"\n";
    ss<<"A:\n"<<_Atotal<<"\n";
    ss<<"b:\n"<<-_btotal.transpose()<<"\n";
    ss<<"\n";
    ss<<"G:\n"<<_G<<"\n";
    ss<<"h:\n"<<-_h.transpose()<<"\n";

    return ss.str();

}




///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Hierarchy Solver with QuadProg sub-Solver
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Instanciate a concrete one level solver with Quadprog++.
 *
 */
OneLevelSolverWithQuadProg::OneLevelSolverWithQuadProg()
    : ocra::NamedInstance("One Level Solver with QuadProg++ subSolver")
    , OneLevelSolver()
{

}

/** Destructor
 *
 */
OneLevelSolverWithQuadProg::~OneLevelSolverWithQuadProg()
{

}

/** Update the objective function, by considering all the active registered tasks.
 *
 */
void OneLevelSolverWithQuadProg::updateObjectiveEquations()
{
//    _C = _Pbase;
    _C.setZero(n(), n());
    _d.setZero(n());

    for(int i=0; i<_objectives.size(); i++)
    {

        ocra::QuadraticFunction& obj     = _objectives[i]->getFunction();
        double weight                   = _objectives[i]->getWeight();

        const std::vector<int>& objMap  = findMapping(obj.getVariable());

        ocra::utils::addCompressed2d(   obj.getPi(), _C, objMap, weight);
        ocra::utils::addCompressedByRow(obj.getqi(), _d, objMap, weight);

    }

}

/** Update the constraint set, by considering all the active ones.
 *
 */
void OneLevelSolverWithQuadProg::updateConstraintEquations()
{
    // UPDATE EQUALITY CONSTRAINTS
    int ne = 0;
    for (int i=0; i<_equalityConstraints.size(); ++i)
    {
        ne += _equalityConstraints[i]->getDimension();
    }

    _A.setZero(ne, n());
    _b.setZero(ne);

    int idx = 0;
    for (int i=0; i<_equalityConstraints.size(); ++i)
    {
        ocra::LinearConstraint* cstr = _equalityConstraints[i];
        int dim = cstr->getDimension();

        if (dim > 0)
        {
            Eigen::Block<Eigen::MatrixXd> _A_block = _A.block(idx, 0, dim, n());
            Eigen::VectorBlock<Eigen::VectorXd> _b_segment = _b.segment(idx, dim);

            Eigen::VectorXd v; // ?? is it useless ??
            ocra::utils::convert(*cstr, findMapping(cstr->getVariable()), ocra::CSTR_PLUS_EQUAL, _A_block, _b_segment, v);

            idx += dim;
        }

    }

    reduceConstraints(_A, _b, _Atotal, _btotal);



    // UPDATE INEQUALITY CONSTRAINTS
    int ni = 0;
    for (int i=0; i<_inequalityConstraints.size(); ++i)
    {
        ni += _inequalityConstraints[i]->getDimension();
    }

    _G.setZero(ni, n());
    _h.setZero(ni);

    idx = 0;
    for (int i=0; i<_inequalityConstraints.size(); ++i)
    {
        ocra::LinearConstraint* cstr = _inequalityConstraints[i];
        int dim = cstr->getDimension();

        if (dim > 0)
        {
            Eigen::Block<Eigen::MatrixXd > _G_block = _G.block(idx, 0, dim, n());
            Eigen::VectorBlock<Eigen::VectorXd> _h_segment = _h.segment(idx, dim);
            Eigen::VectorXd v;  // ?? is it useless ??
            ocra::utils::convert(*cstr, findMapping(cstr->getVariable()), ocra::CSTR_PLUS_GREATER, _G_block, _h_segment, v);

            idx += dim;
        }

    }
}


/** Do the call Quadprog++ to find a solution of the problem.
 *
 */
void OneLevelSolverWithQuadProg::doSolve()
{
    // solveRecorder.initializeTime();
    QuadProgPP::solve_quadprog(_C, -_d, _Atotal, _btotal, _G, _h, Xsolution);
    _result.solution = Xsolution;
    _result.info = ocra::RETURN_SUCCESS; //TODO: should be dfined through the result of the QuadProgPP::solve_quadprog

    // solveRecorder.saveRelativeTime();
}









///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Instanciate a concrete one level solver with QLD.
 *
 */
OneLevelSolverWithQLD::OneLevelSolverWithQLD()
    : ocra::NamedInstance("One Level Solver with QLD subSolver")
    , OneLevelSolver()
    , _QLDsolver( new ocra::ObjQLD() )
    , MapP(NULL, 0,0)
    , Mapq(NULL,0)
    , MapAandG(NULL,0,0)
    , Mapbandh(NULL,0)
    , MapXsol(NULL,0)
    , MapXl(NULL,0)
    , MapXu(NULL,0)
{

}

/** Destructor
 *
 */
OneLevelSolverWithQLD::~OneLevelSolverWithQLD()
{

}



/** Update the objective function, by considering all the active registered tasks.
 *
 */
void OneLevelSolverWithQLD::updateObjectiveEquations()
{
//    _C = _Pbase;
    _C.setZero(n(), n());
    _d.setZero(n());

    if (MapP.size() != _C.size())
    {
        new (&MapP) MatrixMap(_C.data(), _C.rows(), _C.cols());
        new (&Mapq) VectorMap(_d.data(), _d.size());
    }

    for(int i=0; i<_objectives.size(); i++)
    {

        ocra::QuadraticFunction& obj     = _objectives[i]->getFunction();
        double weight                   = _objectives[i]->getWeight();
        const std::vector<int>& objMap  = findMapping(obj.getVariable());

        ocra::utils::addCompressed2d(   obj.getPi(), _C, objMap, weight);
        ocra::utils::addCompressedByRow(obj.getqi(), _d, objMap, weight);

    }

}

/** Update the constraint set, by considering all the active ones.
 *
 */
void OneLevelSolverWithQLD::updateConstraintEquations()
{
    // UPDATE EQUALITY CONSTRAINTS
    int ne = 0;
    for (int i=0; i<_equalityConstraints.size(); ++i)
    {
        ne += _equalityConstraints[i]->getDimension();
    }

    _A.setZero(ne, n());
    _b.setZero(ne);

    int idx = 0;
    for (int i=0; i<_equalityConstraints.size(); ++i)
    {
        ocra::LinearConstraint* cstr = _equalityConstraints[i];
        int dim = cstr->getDimension();

        if (dim > 0)
        {
            Eigen::Block<Eigen::MatrixXd> _A_block = _A.block(idx, 0, dim, n());
            Eigen::VectorBlock<Eigen::VectorXd> _b_segment = _b.segment(idx, dim);

            Eigen::VectorXd v; // ?? is it useless ??
            ocra::utils::convert(*cstr, findMapping(cstr->getVariable()), ocra::CSTR_PLUS_EQUAL, _A_block, _b_segment, v);

            idx += dim;
        }

    }

    reduceConstraints(_A, _b, _Atotal, _btotal);



    // UPDATE INEQUALITY CONSTRAINTS
    int ni = 0;
    for (int i=0; i<_inequalityConstraints.size(); ++i)
    {
        ni += _inequalityConstraints[i]->getDimension();
    }

    _G.setZero(ni, n());
    _h.setZero(ni);

    idx = 0;
    for (int i=0; i<_inequalityConstraints.size(); ++i)
    {
        ocra::LinearConstraint* cstr = _inequalityConstraints[i];
        int dim = cstr->getDimension();

        if (dim > 0)
        {
            Eigen::Block<Eigen::MatrixXd > _G_block = _G.block(idx, 0, dim, n());
            Eigen::VectorBlock<Eigen::VectorXd> _h_segment = _h.segment(idx, dim);
            Eigen::VectorXd v;  // ?? is it useless ??
            ocra::utils::convert(*cstr, findMapping(cstr->getVariable()), ocra::CSTR_PLUS_GREATER, _G_block, _h_segment, v);

            idx += dim;
        }

    }

    if ( ( AandG.rows() != _Atotal.rows()+_G.rows() ) || ( AandG.cols() != _Atotal.cols() ) )
    {
        AandG.resize(_Atotal.rows()+_G.rows(), n());
        bandh.resize(_btotal.size()+_h.size());
        new (&MapAandG) MatrixMap(AandG.data(), AandG.rows(), AandG.cols());
        new (&Mapbandh) VectorMap(bandh.data(), bandh.size());
    }
    AandG.topRows(_Atotal.rows())   = _Atotal;
    AandG.bottomRows(_G.rows())     = _G;
    bandh.head(_btotal.size())      = _btotal;
    bandh.tail(_h.size())           = _h;

    if (_xl.size() != n())
    {
        _xl = - 1e10 * Eigen::VectorXd::Ones(n()); // X lower bound
        _xu =   1e10 * Eigen::VectorXd::Ones(n()); // X upper bound
        new (&MapXl)    VectorMap(_xl.data(), _xl.size());
        new (&MapXu)    VectorMap(_xu.data(), _xu.size());
        new (&MapXsol)  VectorMap(Xsolution.data(), Xsolution.size()); // if bounds have changed, so has Xsolution
    }
}


/** Do the call QLD to find a solution of the problem.
 *
 */
void OneLevelSolverWithQLD::doSolve()
{
    // solveRecorder.initializeTime();


//    if (MapXsol.size() != Xsolution.size())                               // this test is done in #updateConstraintEquations()
//        new (&MapXsol) VectorMap(Xsolution.data(), Xsolution.size());

    _d = -_d; // this is to set -Mapq

    int res = _QLDsolver->solve(MapP, Mapq, MapAandG, Mapbandh, _Atotal.rows(), MapXsol, MapXl, MapXu, false);

    _result.solution = MapXsol;

    _result.info = ocra::RETURN_SUCCESS; //TODO: should be dfined through the result of the QuadProgPP::solve_quadprog

    // solveRecorder.saveRelativeTime();
}