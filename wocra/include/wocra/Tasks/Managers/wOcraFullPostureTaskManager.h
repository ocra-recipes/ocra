#ifndef wOcraFULLPOSTURETASKMANAGER_H
#define wOcraFULLPOSTURETASKMANAGER_H

#include "ocra/control/Model.h"
#include "wocra/Tasks/wOcraTaskManagerBase.h"
#include "ocra/control/FullState.h"
// 


#include <Eigen/Dense>

namespace wocra
{

/** \brief wOcra Task Manager for the joint space posture
 *
 */
class wOcraFullPostureTaskManager : public wOcraTaskManagerBase
{
    public:
        wOcraFullPostureTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, int fullStateType, double stiffness, double damping, double weight, bool usesYarpPorts = true);

        wOcraFullPostureTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, int fullStateType, double stiffness, double damping, const Eigen::VectorXd& weight, bool usesYarpPorts = true);

        wOcraFullPostureTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, int fullStateType, double stiffness, double damping, double weight, const Eigen::VectorXd& init_q, bool usesYarpPorts = true);

        wOcraFullPostureTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, int fullStateType, double stiffness, double damping, const Eigen::VectorXd& weight, const Eigen::VectorXd& init_q, bool usesYarpPorts = true);

        ~wOcraFullPostureTaskManager();

        // All Managers have this
        void activate();
        void deactivate();

        // Yarp related:
        virtual const double * getCurrentState();
        virtual std::string getTaskManagerType();
        virtual bool checkIfActivated();

        // Set the task reference
        void setPosture(const Eigen::VectorXd& q);
        void setPosture(const Eigen::VectorXd& q, const Eigen::VectorXd& qdot, const Eigen::VectorXd& qddot);

        // For objective tasks
        void setStiffness(double stiffness);
        double getStiffness();
        void setDamping(double damping);
        double getDamping();
        void setWeight(double weight);
        void setWeight(const Eigen::VectorXd& weight);
        Eigen::VectorXd getWeight();
        void setDesiredState();


        // Task error
        Eigen::VectorXd getTaskError();


    private:
        


        ocra::FullStateFeature*          feat;
        ocra::FullModelState*            featState;

        ocra::FullStateFeature*          featDes;
        ocra::FullTargetState*           featDesState;

        void _init(int fullStateType, double stiffness, double damping, double weight);
        void _init(int fullStateType, double stiffness, double damping, const Eigen::VectorXd& weight);
};

}

#endif // wOcraFULLPOSTURETASKMANAGER_H
