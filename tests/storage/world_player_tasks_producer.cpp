#include "../../src/game/WorldPlayerTasks.h"

void AddTaskFromAnotherTranslationUnit(int* completed)
{
    WorldPlayerTasks::Add([completed]() { ++*completed; });
}
