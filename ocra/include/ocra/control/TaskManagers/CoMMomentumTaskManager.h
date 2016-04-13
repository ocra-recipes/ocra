#ifndef COMMOMENTUMTASKMANAGER_H
#define COMMOMENTUMTASKMANAGER_H

#include "ocra/control/TaskManagers/TaskManager.h"
#include "ocra/control/Model.h"



#include <Eigen/Dense>

namespace ocra
{

/** \brief Task Manager for the Center of Mass (CoM) momentum task with the  Controller
 *
 */
class CoMMomentumTaskManager: public TaskManager
{
    public:
        CoMMomentumTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, ocra::ECartesianDof axes, double damping, double weight, int hierarchyLevel = -1 , bool usesYarpPorts = true);

        CoMMomentumTaskManager(ocra::Controller& ctrl, const ocra::Model& model, const std::string& taskName, ocra::ECartesianDof axes, double damping, const Eigen::VectorXd& weight, int hierarchyLevel = -1 , bool usesYarpPorts = false);


        ~CoMMomentumTaskManager();


//        // Sets the position
//        void setState(const Eigen::Vector3d& position);
//        // Sets the position velocity and acceleration
//        void setState(const Eigen::Vector3d& position, const Eigen::Vector3d& velocity, const Eigen::Vector3d& acceleration);


        // Yarp related:
        virtual const double * getCurrentState();
        virtual std::string getTaskManagerType();

//        void setDesiredState();

        // Task error
//

    private:

        ocra::ECartesianDof              axes;
        ocra::PositionFeature*           feat;
        ocra::CoMFrame*                  comFeatFrame;
        ocra::PositionFeature*           featDes;
        ocra::TargetFrame*               featDesFrame;

        void _init(double damping, double weight, int hierarchyLevel);
        void _init(double damping, const Eigen::VectorXd& weight, int hierarchyLevel);
};

}

#endif // COMTASKMANAGER_H
