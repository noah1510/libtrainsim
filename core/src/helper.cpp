#include "helper.hpp"

libtrainsim::core::Helper::Helper() {}

void libtrainsim::core::Helper::print_exception_impl(const std::exception& e, int level) {
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& nestedException) {
        print_exception_impl(nestedException, level+1);
    } catch(...) {}
}
