// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/Console.hpp"
#include <fstream> 
#include <cassert>
#include <cstdio>

namespace signal_estimator {

void Console::write(const char* str) {
    assert(str);

    fprintf(stdout, "%s", str);
}

void Console::flush() {
    fflush(stdout);
}


void FileConsole::write(const char* str) {
    assert(str);
    assert(path_!="");
    //write to file
    std::ofstream my_file(path_,std::ios::app);
    my_file<<str;

    my_file.close();
    
}
void FileConsole::flush() {
   
}
} // namespace signal_estimator
