#pragma once

#include "statusDisplayGraph.hpp"

namespace libtrainsim {
    namespace extras {

        /**
         * @brief a window to display real time stats for the simulator
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO [[maybe_unused]] statusDisplay : public Gtk::Box, public SimpleGFX::tracked_eventHandle {
            // friend class statusDisplaySettings;

          private:
            // control if the latest value should be displayed
            bool displayLatestValue = true;

            // control if the graphs should be displayed
            bool displayGraphs = false;

            // control if the progress along the track should be shown
            bool displayProgress = true;

            /**
             * @brief a vector with all of graphs that are displayed and if they should be displayed
             */
            std::vector<std::pair<statusDisplayGraph<100>*, bool>> graphs;

            /**
             * @brief the names of all of the graphs that have to exist and cannot be deleted
             */
            std::vector<std::string> defaultGraphNames;

            // The position where the track begins
            sakurajin::unit_system::length beginPosition;

            // the current position along the track
            sakurajin::unit_system::length currentPosition;

            // the position where the track ends
            sakurajin::unit_system::length endPosition;

            Gtk::ListBox* graphsList;
            
            /**
             * @brief The appLauncher used by this class.
             * It is needed to queue api calls to happen in the main thread.
             * This allows gotoFrame to be called from any thread without crashing the program.
             */
            std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> mainAppLauncher;

          public:
            /**
             * @brief create a new status display
             *
             * @param _manageSettings if false is passed this class will not create a settings tab
             */
            explicit statusDisplay(std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> mainAppLauncher);

            /**
             * @brief destroy the status display
             *
             * @note if a settings tab was added it will be removed in here
             */
            ~statusDisplay() override;

            /**
             * @brief add a new frametime which will be displayed as the latest value.
             */
            [[maybe_unused]]
            void appendFrametime(sakurajin::unit_system::time_si frametime);

            /**
             * @brief add a new rendertime which will be displayed as the latest value.
             */
            [[maybe_unused]]
            void appendRendertime(sakurajin::unit_system::time_si rendertime);

            /**
             * @brief update the current position of the train, to move the progress bar
             */
            [[maybe_unused]]
            void changePosition(sakurajin::unit_system::length newPosition);

            /**
             * @brief set the begin position of the track for the progress bar
             */
            [[maybe_unused]]
            void changeEndPosition(sakurajin::unit_system::length newEndPosition);

            /**
             * @brief set the end position of the track for the progress bar
             */
            [[maybe_unused]]
            void changeBeginPosition(sakurajin::unit_system::length newBeginPosition);

            /**
             * @brief add a new speedLevel which will be displayed as the latest value.
             */
            [[maybe_unused]]
            void setSpeedLevel(const libtrainsim::core::input_axis& newSpeedLevel);

            /**
             * @brief add a new acceleration which will be displayed as the latest value.
             */
            [[maybe_unused]]
            void setAcceleration(sakurajin::unit_system::acceleration newAcceleration);

            /**
             * @brief add a new velocity which will be displayed as the latest value.
             */
            [[maybe_unused]]
            void setVelocity(sakurajin::unit_system::speed newVelocity);

            /**
             * @brief create a new graph to keep track of any value on a graph
             *
             * @warning this throws an exception if that graph already exists
             */
            void createCustomGraph(const std::string& graphName, const std::string& tooltipMessage);

            /**
             * @brief remove a custom graph
             *
             * @warning this throws an exception if there is no graph with that name or
             * you try to remove one of the default graphs
             */
            [[maybe_unused]]
            void removeGraph(const std::string& graphName);

            /**
             * add a new value which will be displayed on graphName as the latest value.
             *
             * @warning this throws an exception if the graph does not exist
             */
            [[maybe_unused]]
            void appendToGraph(const std::string& graphName, double value);

            /**
             * change the data range for a given graph
             *
             * @warning this throws an exception if the graph does not exist
             */
            [[maybe_unused]]
            void changeGraphRange(const std::string& graphName, double minVal, double maxVal);

            /**
             * redraw all graphs
             */
            [[maybe_unused]]
            void redrawGraphs();

            void operator()(const SimpleGFX::inputEvent& event, bool& handled) override;

            void on_unrealize() override;
        };
    } // namespace extras
} // namespace libtrainsim
