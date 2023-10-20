// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Formatter.hpp"

namespace signal_estimator {

Formatter::Formatter() {
    label("REQUIRED", "[required]");
}

std::string Formatter::make_expanded(const CLI::App* sub) const {
    std::stringstream out;

    out << "\n" << sub->get_group() << ":\n";
    for (const CLI::Option* opt : sub->get_options()) {
        if (opt == sub->get_help_ptr() || opt == sub->get_help_all_ptr()) {
            continue;
        }
        out << make_option(opt, false);
    }

    return out.str();
}

} // namespace signal_estimator
