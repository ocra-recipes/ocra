#ifndef MESSAGE_VOCABULARY_H
#define MESSAGE_VOCABULARY_H

#include <iostream>

namespace ocra_recipes
{

enum SERVER_COMMUNICATIONS_MESSAGE
{
    // General indicators
    FAILURE = 0,
    SUCCESS,
    WARNING,

    // Controller requests
    GET_CONTROLLER_STATUS,
    GET_WBI_CONFIG_FILE_PATH,
    GET_ROBOT_NAME,
    GET_IS_FLOATING_BASE,

    START_CONTROLLER,
    STOP_CONTROLLER,
    PAUSE_CONTROLLER,

    // Controller status indicators
    CONTROLLER_RUNNING,
    CONTROLLER_STOPPED,
    CONTROLLER_PAUSED,

    // Task requests
    ADD_TASK,
    ADD_TASK_FROM_FILE,
    REMOVE_TASK,
    REMOVE_TASKS,
    REMOVE_TASK_PORT,
    GET_TASK_LIST,
    GET_TASK_PORT_LIST,
    // Other
    HELP
};

} // namespace ocra_yarp
#endif // MESSAGE_VOCABULARY_H
