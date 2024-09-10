#pragma once

#include "core.hpp"
#include "simplegl.hpp"

namespace libtrainsim::extras {
    /**
     * @brief an abstraction over the ImGui graphs for use with the statusDisplay
     * This class provides an easy abstraction over the PlotLines function to
     * handle all of the common graph display needs
     *
     * @tparam VALUE_COUNT The number of samples this graph should have
     */
    template <size_t VALUE_COUNT>
        requires SimpleGFX::Concepts::notZeroSize<VALUE_COUNT>
    class statusDisplayGraph : public Gtk::DrawingArea {
      private:
        /**
         * @brief A mutex to protext access to the values array
         */
        std::mutex dataMutex;

        /**
         * @brief the internal array to store the data values that are displayed
         */
        std::array<double, VALUE_COUNT> values;

        /**
         * @brief the name of the graph
         */
        const std::string name;

        /**
         * @brief the tooltip that should be shown when the graph is hovered
         */
        std::string tooltip_txt;

        std::atomic<double> minVal       = 0.0;
        std::atomic<double> maxVal       = 0.0;
        std::atomic<double> latestVal    = 0.0;
        std::atomic<size_t> latestIdx    = 0;
        std::atomic<bool>   fixedRange   = false;
        std::atomic<bool>   showLatest   = true;
        std::atomic<bool>   showGraph    = true;
        std::atomic<bool>   got_new_data = false;

        const double margin = 0.1;

        double scaleValue(double val) const {
            const auto currMin = minVal.load();
            const auto currMax = maxVal.load();

            const auto range   = currMax - currMin;
            const auto scaledY = std::clamp(1 - (val - currMin) / range, 0.0, 1.0);

            return scaledY * (1.0 - 2.0 * margin) + margin;
        }

        /**
         * redraw all graphs
         */
        [[maybe_unused]]
        bool redrawIfUpdated() {
            if (got_new_data) {
                got_new_data = false;
                queue_draw();
            }

            return true;
        }

        const int interval_ms = 10;

      public:
        /**
         * @brief create a new graph with a name and tooltip
         */
        statusDisplayGraph(std::string graphName, std::string tooltipMessage)
            : name{std::move(graphName)},
              tooltip_txt(std::move(tooltipMessage)) {
            std::scoped_lock lock{dataMutex};
            for (auto& val : values) {
                val = 0.0;
            }

            set_content_width(1280);
            set_content_height(150);

            set_can_focus(false);
            set_can_target(false);

            set_hexpand(true);
            set_vexpand(true);

            Glib::ustring data     = ".invis_bg {background-color: rgba(255, 255, 255, 0);}";
            auto          provider = Gtk::CssProvider::create();
            provider->load_from_string(data);

            auto ctx = get_style_context();
            ctx->add_class("invis_bg");
            ctx->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

            set_tooltip_text(tooltip_txt);

            set_draw_func(sigc::mem_fun(*this, &libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::on_draw));
            Glib::signal_timeout().connect(sigc::mem_fun(*this, &libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::redrawIfUpdated),
                                           interval_ms);
        }

        /**
         * @brief display the graph in a window
         *
         * @param showLatest if true next to the name of the graph there will be the latest value
         */
        [[maybe_unused]]
        void setShowLatest(bool latest = true) {
            showLatest = latest;
        }

        /**
         * @brief append a value to the graph to be the latest value to be displayed
         */
        template <class value_type = double>
            requires std::convertible_to<value_type, double>
        [[maybe_unused]]
        void appendValue(value_type newValue) {
            auto converted_val = static_cast<double>(newValue);
            auto new_index     = (latestIdx + 1) % VALUE_COUNT;
            auto new_max_val   = std::max(maxVal.load(), converted_val);
            auto new_min_val   = std::min(minVal.load(), converted_val);

            std::scoped_lock lock{dataMutex};

            values[latestIdx] = converted_val;
            latestVal         = converted_val;
            latestIdx         = new_index;
            got_new_data      = true;
            if (!fixedRange) {
                maxVal = new_max_val;
                minVal = new_min_val;
            }
        }

        template <class _minValType = double, class _maxValType = double>
            requires std::convertible_to<_minValType, double> && std::convertible_to<_maxValType, double>
        [[maybe_unused]]
        void setRange(_minValType _minVal, _maxValType _maxVal) {
            minVal     = static_cast<double>(_minVal);
            maxVal     = static_cast<double>(_maxVal);
            fixedRange = true;
        }

        /**
         * @brief get the name of the graph
         */
        [[nodiscard]] [[maybe_unused]]
        const std::string& getName() const {
            return name;
        }

        /**
         * @brief get the latest value of the graph
         */
        [[nodiscard]] [[maybe_unused]]
        double getLatest() const {
            return latestVal;
        }

        void on_unrealize() override {
            std::scoped_lock lock{dataMutex};
            return Gtk::DrawingArea::on_unrealize();
        }

        void on_resize(int width, int height) override {
            set_content_width(width);
            set_content_height(height);
        }

        void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
            if (!get_realized()) {
                return;
            }

            get_style_context()->render_background(cr, 0, 0, width, height);

            Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());

            const double widthScale = showLatest ? 0.7 : 1.0 - margin;

            cr->set_dash(std::vector<double>{4, 2}, 4);
            cr->set_line_width(1.0);
            cr->move_to(0.25 * margin * width, scaleValue(0) * height);
            cr->line_to((widthScale + 0.5 * margin) * width, scaleValue(0) * height);
            cr->stroke();

            cr->unset_dash();
            cr->set_line_width(2.0);

            if (showGraph) {
                std::scoped_lock lock{dataMutex};
                const auto       idx_offset = latestIdx.load();

                for (size_t i = 0; i < VALUE_COUNT; i++) {
                    auto val = values.at((i + idx_offset) % VALUE_COUNT);
                    auto dx  = static_cast<double>(i) / static_cast<double>(VALUE_COUNT - 1) * widthScale;
                    dx += 0.25 * margin;

                    cr->line_to(dx * width, scaleValue(val) * height);
                }

                cr->stroke();
            }

            if (showLatest) {
                std::stringstream ss;
                ss << name << ": " << latestVal.load();

                cr->move_to((widthScale + margin) * width, 0.5 * height);

                cr->set_font_size(17.5);
                cr->select_font_face("Noto", Cairo::ToyFontFace::Slant::OBLIQUE, Cairo::ToyFontFace::Weight::NORMAL);

                cr->text_path(ss.str());
                cr->stroke();
            }
        }
    };
} // namespace libtrainsim::extras