// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once
#include <string> 
namespace signal_estimator {

class Console {
public:
    virtual ~Console() = default;
    virtual void write(const char* str);
    virtual void flush();
};

class FileConsole:public Console {
public:
     ~FileConsole(){};

    FileConsole(std::string p_path){
        path_=p_path;
    }
    void write(const char* str) override;
    void flush() override;
private:
    std::string path_;
};


} // namespace signal_estimator
