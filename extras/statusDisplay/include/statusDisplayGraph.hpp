#pragma once

#include "helper.hpp"
#include "simplegl.hpp"

namespace libtrainsim {
    namespace extras {
        /**
         * @brief an abstraction over the ImGui graphs for use with the statusDisplay
         * This class provides an easy abstraction over the PlotLines function to
         * handle all of the common graph display needs
         *
         * @tparam VALUE_COUNT The number of samples this graph should have
         */
        template <size_t VALUE_COUNT>
        class LIBTRAINSIM_EXPORT_MACRO [[maybe_unused]] statusDisplayGraph : public Gtk::DrawingArea {
          private:
            /**
             * @brief the internal array to store the data values that are displayed
             */
            std::array<double, VALUE_COUNT> values;

            /**
             * @brief the name of the graph
             */
            std::string name;

            /**
             * @brief the tooltip that should be shown when the graph is hovered
             */
            std::string tooltip_txt;

            double minVal;
            double maxVal;
            bool   fixedRange = false;

            std::shared_mutex dataMutex;

            bool showLatest = true;
            bool showGraph  = true;

            const double margin = 0.1;
            double       scaleValue(double val);

          public:
            /**
             * @brief create a new graph with a name and tooltip
             */
            statusDisplayGraph(const std::string& graphName, const std::string& tooltipMessage);

            /**
             * @brief display the graph in a window
             *
             * @param showLatest if true next to the name of the graph there will be the latest value
             */

            [[maybe_unused]]
            void setShowLatest(bool latest = true);
            void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

            /**
             * @brief append a value to the graph to be the latest value to be displayed
             */

            [[maybe_unused]]
            void appendValue(double newValue, bool redraw = true);

            [[maybe_unused]]
            void setRange(double _minVal, double _maxVal);

            /**
             * @brief get the name of the graph
             */
            [[nodiscard]] [[maybe_unused]]
            const std::string& getName();

            /**
             * @brief get the latest value of the graph
             */
            [[nodiscard]] [[maybe_unused]]
            double getLatest();

            void on_unrealize() override;
        };
    } // namespace extras
} // namespace libtrainsim

template <size_t VALUE_COUNT>
libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::statusDisplayGraph(const std::string& graphName, const std::string& tooltipMessage) {
    std::scoped_lock lock{dataMutex};
    name        = graphName;
    tooltip_txt = tooltipMessage;
    minVal      = 0.0;
    maxVal      = 0.0;

    for (auto& val : values) {
        val = 0.0;
    }

    set_draw_func(sigc::mem_fun(*this, &libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::on_draw));
    set_content_width(1280);
    set_content_height(150);

    set_tooltip_text(tooltip_txt);
}


template <size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::on_unrealize() {
    std::scoped_lock lock{dataMutex};

    return Gtk::DrawingArea::on_unrealize();
}

template <size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::setShowLatest(bool latest) {
    std::scoped_lock lock{dataMutex};
    showLatest = latest;
}

template <size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::appendValue(double newValue, bool redraw) {
    std::scoped_lock lock{dataMutex};
    libtrainsim::core::Helper::appendValue<double, VALUE_COUNT>(values, newValue);
    if (!fixedRange) {
        if (newValue > maxVal) {
            maxVal = newValue;
        } else if (newValue < minVal) {
            minVal = newValue;
        }
    }

    if (redraw) {
        queue_draw();
    }
}

template <size_t VALUE_COUNT>
inline double libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::scaleValue(double val) {

    const double range = maxVal - minVal;

    auto scaledY = 1 - (val - minVal) / range;
    scaledY      = std::clamp(scaledY, 0.0, 1.0);

    return scaledY * (1.0 - 2.0 * margin) + margin;
}

template <size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    if (!get_realized()) {
        return;
    }

    std::shared_lock lock{dataMutex};

    get_style_context()->render_background(cr, 0, 0, width, height);

    std::stringstream ss;
    ss << name;
    if (showLatest) {
        ss << ": " << getLatest();
    }

    double widthScale = 1.0;
    if (showLatest) {
        widthScale = 0.7;
    }

    cr->set_dash(std::vector<double>{4, 2}, 4);
    cr->set_line_width(1.0);
    cr->move_to(0.25 * margin * width, scaleValue(0) * height);
    cr->line_to((widthScale + 0.5 * margin) * width, scaleValue(0) * height);
    cr->stroke();

    cr->unset_dash();
    cr->set_line_width(2.0);

    if (showGraph) {
        for (size_t i = 0; i < VALUE_COUNT; i++) {
            auto   val = values[i];
            double dx  = static_cast<double>(i) / static_cast<double>(VALUE_COUNT - 1) * widthScale;
            dx += 0.25 * margin;
            cr->line_to(dx * width, scaleValue(val) * height);
        }
        cr->stroke();
    }

    if (showLatest) {
        cr->move_to((widthScale + margin) * width, 0.5 * height);

        cr->set_font_size(17.5);
        cr->select_font_face("Noto", Cairo::ToyFontFace::Slant::OBLIQUE, Cairo::ToyFontFace::Weight::NORMAL);

        cr->text_path(ss.str());
        cr->stroke();
    }
}

template <size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::setRange(double _minVal, double _maxVal) {
    std::scoped_lock lock{dataMutex};

    minVal     = _minVal;
    maxVal     = _maxVal;
    fixedRange = true;
}

template <size_t VALUE_COUNT>
const std::string& libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::getName() {
    std::shared_lock lock{dataMutex};
    return name;
}

template <size_t VALUE_COUNT>
double libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::getLatest() {
    std::shared_lock lock{dataMutex};
    return values.back();
}
