#include "inputHandler.hpp"

using namespace std::literals;

libtrainsim::control::input_handler::input_handler(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) noexcept(false)
    : conf{_conf} {

#ifdef HAS_VIDEO_SUPPORT
    keyboardPoller = std::make_shared<SimpleGFX::SimpleGL::eventPollerGtkKeyboard>();

    keyboardPoller->addKey(GDK_KEY_Escape, "CLOSE");

    keyboardPoller->addKey(GDK_KEY_F10, "MAXIMIZE");
    keyboardPoller->addKey(GDK_KEY_F11, "STATUS_WINDOW_TOGGLE_VISIBILITY");
    keyboardPoller->addKey(GDK_KEY_F12, "STATUS_WINDOW_SHOW_LATEST");

    keyboardPoller->addKey(GDK_KEY_w, "ACCELERATE");
    keyboardPoller->addKey(GDK_KEY_s, "BREAK");
    keyboardPoller->addKey(GDK_KEY_p, "EMERGENCY_BREAK");

    keyboardPoller->addKey(GDK_KEY_W, "ACCELERATE");
    keyboardPoller->addKey(GDK_KEY_S, "BREAK");
    keyboardPoller->addKey(GDK_KEY_P, "EMERGENCY_BREAK");
#endif
}

libtrainsim::control::input_handler::~input_handler() {
    resetFlags();
}

void libtrainsim::control::input_handler::resetFlags() {
    std::scoped_lock lock{dataMutex};
    serial.reset();

    currentInputAxis     = 0.0;
    shouldClose          = false;
    shouldTeminate       = false;
    shouldEmergencyBreak = false;

    running = false;
}

void libtrainsim::control::input_handler::startSimulation() {

    std::scoped_lock lock{dataMutex};

    try {
        // create a serial controller and if it cannot connect destroy it
        serial = std::make_unique<serialcontrol>(conf);
        if (!serial->IsConnected()) {
            serial.reset();
        } else {
            if (registered) {
                serial->registerWithEventManager(manager, 0);
            }
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error initializing the serial control"));
    }

    running = true;
}


#ifdef HAS_VIDEO_SUPPORT
std::shared_ptr<SimpleGFX::SimpleGL::eventPollerGtkKeyboard> libtrainsim::control::input_handler::getKeyboardPoller() {
    return keyboardPoller;
}
#endif

libtrainsim::core::input_axis libtrainsim::control::input_handler::getSpeedAxis() noexcept {
    std::shared_lock lock{dataMutex};
    return currentInputAxis;
}

bool libtrainsim::control::input_handler::closingFlag() noexcept {
    std::shared_lock lock{dataMutex};
    if (shouldClose || shouldTeminate) {
        shouldClose = false;
        return true;
    }

    return false;
    ;
}

bool libtrainsim::control::input_handler::emergencyFlag() noexcept {
    std::shared_lock lock{dataMutex};
    if (shouldEmergencyBreak) {
        shouldEmergencyBreak = false;
        return true;
    }
    return false;
}

std::shared_ptr<SimpleGFX::trackedFuture<uint64_t>> libtrainsim::control::input_handler::registerWithEventManager(SimpleGFX::eventManager* manager, int priority) {

#ifdef HAS_VIDEO_SUPPORT
    keyboardPoller->registerWithEventManager(manager, priority);
#endif

    if (serial) {
        serial->registerWithEventManager(manager, priority);
    }

    return SimpleGFX::eventHandle::registerWithEventManager(manager, priority);
}

void libtrainsim::control::input_handler::unregister() {

#ifdef HAS_VIDEO_SUPPORT
    keyboardPoller->unregister();
#endif

    if (serial) {
        serial->unregister();
    }

    SimpleGFX::eventHandle::unregister();
}

bool libtrainsim::control::input_handler::onEvent(const SimpleGFX::inputEvent& event) {

    std::scoped_lock lock{dataMutex};
    auto             eventName = event.name;

    if (serial) {
        static double accelVal = 0;
        static double brakeVal = 0;

        const auto analogCases = {"ACCELERATE_ANALOG", "BREAK_ANALOG"};
        switch (SimpleGFX::TSwitch(eventName, analogCases)) {
            case (0):
                accelVal         = event.amount / 255;
                currentInputAxis = accelVal - brakeVal;
                return true;
            case (1):
                brakeVal         = event.amount / 255;
                currentInputAxis = accelVal - brakeVal;
                return true;
            default:
                break;
        }
    }

    const auto actionCases = {"TERMINATE", "CLOSE", "EMERGENCY_BREAK", "ACCELERATE", "BREAK"};
    switch (SimpleGFX::TSwitch(eventName, actionCases)) {
        case (0):
            shouldTeminate = true;
            return false;
        case (1):
            shouldClose = true;
            return false;
        case (2):
            if (!running) {
                return false;
            };
            shouldEmergencyBreak = true;
            return true;
        case (3):
            if (!running || serial) {
                return false;
            };
            currentInputAxis += 0.1;
            if (abs(currentInputAxis.get()) < 0.07) {
                currentInputAxis = 0.0;
            }
            return true;
        case (4):
            if (!running || serial) {
                return false;
            };
            currentInputAxis -= 0.1;
            if (abs(currentInputAxis.get()) < 0.07) {
                currentInputAxis = 0.0;
            }
            return true;
        default:
            return SimpleGFX::eventHandle::onEvent(event);
    }
}
