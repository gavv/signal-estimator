// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <CLI/CLI.hpp>

namespace signal_estimator {

// custom --help formatter
class Formatter : public CLI::Formatter {
public:
    Formatter();

    std::string make_expanded(const CLI::App* sub) const override;
};

} // namespace signal_estimator
