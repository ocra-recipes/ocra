#ifndef TASK_BUILDER_H
#define TASK_BUILDER_H

#include <ocra/control/TaskBuilders/TaskBuilderOptions.h>
#include <ocra/control/Model.h>
#include <ocra/control/Task.h>
#include <ocra/control/TaskState.h>
#include <ocra/control/Feature.h>
#include <ocra/control/ControlFrame.h>
#include <ocra/utilities.h>

namespace ocra {

class TaskBuilder {
DEFINE_CLASS_POINTER_TYPEDEFS(TaskBuilder)

protected:
    Task::Ptr task;
    Model::Ptr model;
    TaskBuilderOptions options;

protected: // pure virtual functions
    virtual void setTaskType() = 0;
    virtual void setTaskState() = 0;
    virtual Feature::Ptr buildFeature() = 0;
    virtual Feature::Ptr buildFeatureDesired() = 0;


public:
    TaskBuilder (const TaskBuilderOptions& taskOptions, Model::Ptr modelPtr);
    virtual ~TaskBuilder ();

    void buildTask();
    Task::Ptr getTask();
    void setTaskParameters();


private:
    void setTaskAsObjective();
    void setTaskLevel();
    void setTaskWeight();
    void setTaskStiffness();
    void setTaskDamping();



};


} // namespace ocra

#endif //TASK_BUILDER_H