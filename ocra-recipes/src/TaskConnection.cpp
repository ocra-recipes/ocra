#include <ocra-recipes/TaskConnection.h>

using namespace ocra_recipes;
int TaskConnection::TASK_CONNECTION_COUNT = 0;

TaskConnection::TaskConnection()
{

}

TaskConnection::TaskConnection(const std::string& destinationTaskName)
: taskName(destinationTaskName)
, controlPortsAreOpen(false)
, firstUpdateOfTaskStateHasOccured(false)
{
    taskConnectionNumber = ++TaskConnection::TASK_CONNECTION_COUNT;

    std::shared_ptr<ClientCommunications> ccComs = std::make_shared<ClientCommunications>();
    ccComs->open();
    taskRpcServerName = ccComs->getTaskPortName(taskName);
    ccComs->close();

    this->taskRpcClientName = "/TaskConnection/"+std::to_string(taskConnectionNumber)+"/"+taskName+"/rpc:o";
    this->taskRpcClient.open(taskRpcClientName.c_str());

    this->yarp.connect(taskRpcClientName.c_str(), taskRpcServerName.c_str());
}

TaskConnection::~TaskConnection()
{
    this->taskRpcClient.close();
    this->closeControlPorts();
}

bool TaskConnection::activate()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::ACTIVATE);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != ocra::TASK_MESSAGE::OCRA_SUCCESS) {
        yLog.error() << "Could not activate " << taskName;
        return false;
    }
    return true;
}

bool TaskConnection::deactivate()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::DEACTIVATE);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != ocra::TASK_MESSAGE::OCRA_SUCCESS) {
        yLog.error() << "Could not deactivate " << taskName;
        return false;
    }
    return true;
}

std::string TaskConnection::getPortName()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_TASK_PORT_NAME);
    this->taskRpcClient.write(message, reply);
    return reply.get(0).asString();
}

bool TaskConnection::isActivated()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_ACTIVITY_STATUS);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::TASK_IS_ACTIVATED) {
        return true;
    } else {
        return false;
    }
}

Eigen::VectorXd TaskConnection::getTaskError()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_TASK_ERROR);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != 0) {
        int dummy;
        return ocra::util::pourBottleIntoEigenVector(reply, dummy);
    } else {
        return Eigen::VectorXd::Zero(0);
    }
}

double TaskConnection::getTaskErrorNorm()
{
    return this->getTaskError().norm();
}

void TaskConnection::setStiffness(double K)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_STIFFNESS);
    message.addDouble(K);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setStiffness() did not work.";
    }
}

void TaskConnection::setStiffness(const Eigen::VectorXd& K)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_STIFFNESS_VECTOR);
    ocra::util::pourEigenVectorIntoBottle(K, message);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setStiffness() did not work.";
    }
}

void TaskConnection::setStiffness(const Eigen::MatrixXd& K)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_STIFFNESS_MATRIX);
    ocra::util::pourEigenMatrixIntoBottle(K, message);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setStiffness() did not work.";
    }
}

double TaskConnection::getStiffness()
{
    return this->getStiffnessMatrix()(0,0);
}

Eigen::MatrixXd TaskConnection::getStiffnessMatrix()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_STIFFNESS);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != 0) {
        int dummy;
        return ocra::util::pourBottleIntoEigenMatrix(reply, dummy);
    } else {
        return Eigen::MatrixXd::Zero(0,0);
    }
}

void TaskConnection::setDamping(double B)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_DAMPING);
    message.addDouble(B);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setDamping() did not work.";
    }
}

void TaskConnection::setDamping(const Eigen::VectorXd& B)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_DAMPING_VECTOR);
    ocra::util::pourEigenVectorIntoBottle(B, message);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setDamping() did not work.";
    }
}

void TaskConnection::setDamping(const Eigen::MatrixXd& B)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_DAMPING_MATRIX);
    ocra::util::pourEigenMatrixIntoBottle(B, message);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setDamping() did not work.";
    }
}

double TaskConnection::getDamping()
{
    return this->getStiffnessMatrix()(0,0);
}

Eigen::MatrixXd TaskConnection::getDampingMatrix()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_DAMPING);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != 0) {
        int dummy;
        return ocra::util::pourBottleIntoEigenMatrix(reply, dummy);
    } else {
        return Eigen::MatrixXd::Zero(0,0);
    }
}

void TaskConnection::setWeight(double weight)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_WEIGHT);
    message.addDouble(weight);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setDamping() did not work.";
    }
}

void TaskConnection::setWeight(Eigen::VectorXd& weights)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_WEIGHT_VECTOR);
    ocra::util::pourEigenVectorIntoBottle(weights, message);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_FAILURE) {
        yLog.error() << "setDamping() did not work.";
    }
}

Eigen::VectorXd TaskConnection::getWeight()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_WEIGHTS);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() != 0) {
        int dummy;
        return ocra::util::pourBottleIntoEigenVector(reply, dummy);
    } else {
        return Eigen::VectorXd::Zero(0);
    }
}

int TaskConnection::getTaskDimension()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_DIMENSION);
    this->taskRpcClient.write(message, reply);
    return reply.get(0).asInt();
}

ocra::Task::META_TASK_TYPE TaskConnection::getTaskType()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_TYPE);
    this->taskRpcClient.write(message, reply);
    return ocra::Task::META_TASK_TYPE(reply.get(0).asInt());
}

std::string TaskConnection::getTaskTypeAsString()
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::GET_TYPE_AS_STRING);
    this->taskRpcClient.write(message, reply);
    return reply.get(0).asString();
}

ocra::TaskState TaskConnection::getTaskState()
{
    if (this->controlPortsAreOpen && firstUpdateOfTaskStateHasOccured) {
        return this->currentState;
    } else {
        yarp::os::Bottle message, reply;
        ocra::TaskState state;
        message.addInt(ocra::TASK_MESSAGE::GET_TASK_STATE);
        this->taskRpcClient.write(message, reply);
        int dummy;
        state.extractFromBottle(reply, dummy);
        return state;
    }
}

ocra::TaskState TaskConnection::getDesiredTaskState()
{
    yarp::os::Bottle message, reply;
    ocra::TaskState state;
    message.addInt(ocra::TASK_MESSAGE::GET_DESIRED_TASK_STATE);
    this->taskRpcClient.write(message, reply);
    int dummy;
    state.extractFromBottle(reply, dummy);
    return state;
}

void TaskConnection::setDesiredTaskState(const ocra::TaskState& newDesiredTaskState)
{
    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::SET_DESIRED_TASK_STATE);
    newDesiredTaskState.putIntoBottle(message);
    this->taskRpcClient.write(message, reply);
}

void TaskConnection::setDesiredTaskStateDirect(const ocra::TaskState& newDesiredTaskState)
{
    if(controlPortsAreOpen){
        yarp::os::Bottle bottle;
        newDesiredTaskState.putIntoBottle(bottle);
        outputPort.write(bottle);
    } else {
        std::cout << "Can't use this method until the control ports have been opened." << std::endl;
    }
}

Eigen::Displacementd TaskConnection::getTaskFrameDisplacement()
{

}

Eigen::Twistd TaskConnection::getTaskFrameVelocity()
{

}

Eigen::Twistd TaskConnection::getTaskFrameAcceleration()
{

}

Eigen::Vector3d TaskConnection::getTaskFramePosition()
{

}

Eigen::Rotation3d TaskConnection::getTaskFrameOrientation()
{

}

Eigen::Vector3d TaskConnection::getTaskFrameLinearVelocity()
{

}

Eigen::Vector3d TaskConnection::getTaskFrameAngularVelocity()
{

}

Eigen::Vector3d TaskConnection::getTaskFrameLinearAcceleration()
{

}

Eigen::Vector3d TaskConnection::getTaskFrameAngularAcceleration()
{

}

bool TaskConnection::openControlPorts()
{
    bool portsConnected = true;

    yarp::os::Bottle message, reply;
    message.addInt(ocra::TASK_MESSAGE::OPEN_CONTROL_PORTS);
    this->taskRpcClient.write(message, reply);
    if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_SUCCESS) {
        message.clear();
        reply.clear();
        message.addInt(ocra::TASK_MESSAGE::GET_CONTROL_PORT_NAMES);
        this->taskRpcClient.write(message, reply);
        this->taskInputPortName = reply.get(0).asString();
        this->taskOutputPortName = reply.get(1).asString();

        this->inputPortName = "/TaskConnection/"+std::to_string(taskConnectionNumber)+"/"+this->taskName+":i";
        this->outputPortName = "/TaskConnection/"+std::to_string(taskConnectionNumber)+"/"+this->taskName+":o";

        portsConnected = portsConnected && inputPort.open(this->inputPortName.c_str());
        portsConnected = portsConnected && outputPort.open(this->outputPortName.c_str());

        this->inpCallback = std::make_shared<inputCallback>(*this);
        inputPort.setReader(*(this->inpCallback));


        if (portsConnected) {
            portsConnected = portsConnected && yarp.connect(this->taskOutputPortName.c_str(), this->inputPortName.c_str());
            portsConnected = portsConnected && yarp.connect(this->outputPortName.c_str(), this->taskInputPortName.c_str());

            this->controlPortsAreOpen = portsConnected;
            if (!this->controlPortsAreOpen) {
                yLog.error() << "Could not open the control ports!";
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return portsConnected;
}

std::string TaskConnection::getTaskOutputPortName()
{
    if (controlPortsAreOpen) {
        return taskOutputPortName;
    } else {
        return "";
    }
}
std::string TaskConnection::getTaskInputPortName()
{
    if (controlPortsAreOpen) {
        return taskInputPortName;
    } else {
        return "";
    }
}

bool TaskConnection::closeControlPorts()
{
    if (this->controlPortsAreOpen) {
        yarp::os::Bottle message, reply;
        message.addInt(ocra::TASK_MESSAGE::CLOSE_CONTROL_PORTS);
        this->taskRpcClient.write(message, reply);
        if(reply.get(0).asInt() == ocra::TASK_MESSAGE::OCRA_SUCCESS) {
            this->inputPort.close();
            this->outputPort.close();
            this->controlPortsAreOpen = false;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }

}



/**************************************************************************************************
                                    Nested PortReader Class
**************************************************************************************************/
TaskConnection::inputCallback::inputCallback(TaskConnection& _tcRef)
: tcRef(_tcRef)
{
    //do nothing
}

bool TaskConnection::inputCallback::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle input;
    if (input.read(connection)){
        tcRef.parseInput(input);
        return true;
    }
    else{
        return false;
    }
}
/**************************************************************************************************
**************************************************************************************************/

void TaskConnection::parseInput(yarp::os::Bottle& input)
{
    int dummy;
    if(!firstUpdateOfTaskStateHasOccured) {
        firstUpdateOfTaskStateHasOccured = true;
    }
    this->currentState.extractFromBottle(input, dummy);
}
