#include "statusDisplay.hpp"

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace std::literals;

libtrainsim::extras::statusDisplay::statusDisplay(std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> _mainAppLauncher)
    : Gtk::Box{},
      mainAppLauncher{std::move(_mainAppLauncher)} {
    
    hide();
    set_can_focus(false);
    set_can_target(false);
    
    set_hexpand(true);
    set_vexpand(true);
    
    graphsList = Gtk::make_managed<Gtk::ListBox>();
    
    graphsList->set_hexpand(true);
    graphsList->set_vexpand(true);
    
    append(*graphsList);
    
    Glib::ustring data = ".invis_bg {background-color: rgba(255, 255, 255, 0);}";
    auto provider = Gtk::CssProvider::create();
    provider->load_from_string(data);
    
    auto ctx = get_style_context();
    ctx->add_class("invis_bg");
    ctx->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    ctx = graphsList->get_style_context();
    ctx->add_class("invis_bg");
    ctx->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    defaultGraphNames = {"frametimes", "rendertimes", "acceleration", "velocity", "speedLevel"};

    beginPosition   = 0_m;
    currentPosition = 0_m;
    endPosition     = 0_m;

    createCustomGraph("frametimes", "frametimes in ms");
    createCustomGraph("rendertimes", "rendertimes in ms");
    createCustomGraph("acceleration", "Acceleration in m/s²");
    createCustomGraph("velocity", "Velocity in km/h");
    createCustomGraph("speedLevel", "SpeedLevel");

    changeGraphRange("acceleration", -2.0, 2.0);
    changeGraphRange("velocity", 0.0, 60.0);
    changeGraphRange("speedLevel", -1.0, 1.0);
}

void libtrainsim::extras::statusDisplay::on_unrealize() {
    Gtk::Box::on_unrealize();
    graphs.clear();
}


void libtrainsim::extras::statusDisplay::appendFrametime(sakurajin::unit_system::time_si frametime) {
    appendToGraph("frametimes", frametime.convert_like(1_ms).val());
}

void libtrainsim::extras::statusDisplay::appendRendertime(sakurajin::unit_system::time_si rendertime) {
    appendToGraph("rendertimes", rendertime.convert_like(1_ms).val());
}

void libtrainsim::extras::statusDisplay::setAcceleration(sakurajin::unit_system::acceleration newAcceleration) {
    auto acc = newAcceleration.convert_like(1_mps2);
    appendToGraph("acceleration", acc.val());
}

void libtrainsim::extras::statusDisplay::setVelocity(sakurajin::unit_system::speed newVelocity) {
    auto vel = newVelocity.convert_like(1_kmph);
    appendToGraph("velocity", vel.val());
}

void libtrainsim::extras::statusDisplay::setSpeedLevel(const core::input_axis& newSpeedLevel) {
    appendToGraph("speedLevel", newSpeedLevel.get());
}


void libtrainsim::extras::statusDisplay::changeBeginPosition(sakurajin::unit_system::length newBeginPosition) {
    beginPosition =  newBeginPosition.convert_like(1_m);
}

void libtrainsim::extras::statusDisplay::changePosition(sakurajin::unit_system::length newPosition) {
    currentPosition = newPosition.convert_like(1_m);
}

void libtrainsim::extras::statusDisplay::changeEndPosition(sakurajin::unit_system::length newEndPosition) {
    endPosition = newEndPosition.convert_like(1_m);
}

void libtrainsim::extras::statusDisplay::createCustomGraph(const std::string& graphName, const std::string& tooltipMessage) {
    for (auto& graph : graphs) {
        if (graph.first->getName() == graphName) {
            throw std::invalid_argument("A graph with the given name already exists!");
        }
    }

    auto newGraph = Gtk::make_managed<statusDisplayGraph<100>>(graphName, tooltipMessage);
    graphsList->append(*newGraph);
    graphs.emplace_back(newGraph, true);
}

void libtrainsim::extras::statusDisplay::removeGraph(const std::string& graphName) {
    if(std::ranges::contains(defaultGraphNames, graphName)){
        throw std::invalid_argument("default graphs may not be removed!");
    }

    for (auto i = graphs.begin(); i < graphs.end(); i++) {
        if (i->first->getName() == graphName) {
            graphsList->remove(*(i->first));
            graphs.erase(i);
            return;
        }
    }

    throw std::invalid_argument("no graph with this name exists");
}

void libtrainsim::extras::statusDisplay::operator()(const SimpleGFX::inputEvent& event, bool& handled) {
    //static auto app        = get_application();
    static bool showLatest = true;

    if (event.inputType != SimpleGFX::inputAction::press) {
        return;
    }

    const auto actionCases = {"STATUS_WINDOW_TOGGLE_VISIBILITY", "STATUS_WINDOW_SHOW_LATEST"};
    switch (SimpleGFX::TSwitch(event.name, actionCases)) {
        case (0):
            mainAppLauncher->callDeffered([this]() {if (is_visible()) {hide();} else {show();}}, sec_getID());
            
            handled = true;
            return;
        case (1):
            showLatest = !showLatest;
            for (auto [graph, _] : graphs) {
                graph->setShowLatest(showLatest);
            }
            handled = true;
            return;
        default:
            return;
    }
}
