#include "_TriggerExtension.h"

int findTargetStateByName(string type) {
    for (size_t i = 0; i < gTriggerList.size(); i++) {
        if (gTriggerList[i].trigName == type) {
            return static_cast<int>(i);
        }
    }
    return -1;
}