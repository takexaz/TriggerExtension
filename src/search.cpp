#include "_TriggerExtension.h"

int findTargetTriggerByName(string trigger) {
    for (size_t i = 0; i < gTriggerList.size(); i++) {
        if (gTriggerList[i].trigger == trigger) {
            return static_cast<int>(i);
        }
    }
    return -1;
}