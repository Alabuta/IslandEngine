#pragma once

#ifndef CAMERA_BEHAVIOUR_H           // Include guard "CameraBehaviour.h"
#define CAMERA_BEHAVIOUR_H

namespace __hidden {
class ICameraBehaviour {
public:

    virtual void Update() = 0;
};

class ICameraEditorBehaviour : public ICameraBehaviour {
public:

    void Update() {};
};
}

#endif // CAMERA_BEHAVIOUR_H