#include "inputHandler.hpp"

using namespace std::literals;

libtrainsim::control::input_handler::input_handler(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) noexcept(false)
    : conf{_conf} {

    try {
        serial = std::make_unique<serialcontrol>(conf);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error initializing the serial control"));
    }

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

    conf->getInputManager()->registerPoller(sigc::mem_fun(*keyboardPoller, &SimpleGFX::SimpleGL::eventPollerGtkKeyboard::poll) );
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

    if (!serial->IsConnected()) {
        serial->connect();
    }

    *conf->getLogger() << SimpleGFX::loggingLevel::normal << "starting simulation. Serial connection status: " << serial->IsConnected();

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
}

bool libtrainsim::control::input_handler::emergencyFlag() noexcept {
    std::shared_lock lock{dataMutex};
    if (shouldEmergencyBreak) {
        shouldEmergencyBreak = false;
        return true;
    }
    return false;
}

void libtrainsim::control::input_handler::onEvent(const SimpleGFX::inputEvent& event, bool& handled) {

    std::scoped_lock lock{dataMutex};
    auto             eventName       = event.name;
    bool             serialConnected = serial && serial->IsConnected();

    if (serialConnected) {
        static double accelVal = 0;
        static double brakeVal = 0;

        const auto analogCases = {"ACCELERATE_ANALOG", "BREAK_ANALOG"};
        switch (SimpleGFX::TSwitch(eventName, analogCases)) {
            case (0):
                accelVal         = event.amount / 255;
                currentInputAxis = accelVal - brakeVal;
                handled          = true;
                return;
            case (1):
                brakeVal         = event.amount / 255;
                currentInputAxis = accelVal - brakeVal;
                handled          = true;
                return;
            default:
                break;
        }
    }

    const auto actionCases = {"TERMINATE", "CLOSE", "EMERGENCY_BREAK", "ACCELERATE", "BREAK"};
    switch (SimpleGFX::TSwitch(eventName, actionCases)) {
        case (0):
            shouldTeminate = true;
            handled        = true;
            return;
        case (1):
            shouldClose = true;
            handled     = true;
            return;
        case (2):
            if (running) {
                shouldEmergencyBreak = true;
                handled              = true;
            };
            return;
        case (3):
            if (running && serialConnected) {
                currentInputAxis += 0.1;
                if (abs(currentInputAxis.get()) < 0.07) {
                    currentInputAxis = 0.0;
                }
                handled = true;
            };
            return;
        case (4):
            if (running && serialConnected) {
                currentInputAxis -= 0.1;
                if (abs(currentInputAxis.get()) < 0.07) {
                    currentInputAxis = 0.0;
                }
                handled = true;
            };
            return;
        default:
            return;
    }
}
