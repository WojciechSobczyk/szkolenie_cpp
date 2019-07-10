#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using namespace std;

class i_formatter {
public:
    virtual ~i_formatter() = default;
    virtual string format(string& text) = 0;
};

class formatter : public i_formatter {
    string format (string& text) override {
        text += "\n";
        return text;
    }
};

class i_stream_service {
public:
    typedef shared_ptr<i_stream_service> ptr;

    virtual ~i_stream_service() = default;

    virtual void log(const string &text) = 0;

    virtual void open() = 0;

    virtual void close() = 0;
};

class file_stream_service : public i_stream_service {
    ofstream ofs;
public:
    void open() override {
        ofs.open("log1.txt", ofstream::out | ofstream::app);
    }

    void log(const string &text) override {
        ofs << text;
    }

    void close() override {
        ofs.close();
    }
};

class console_stream_service : public i_stream_service {
public:
    void log(const string &text) override {
        cout << text;
    }

    void open() override {

    }

    void close() override {

    }
};

class logger_engine {
    i_stream_service &streamService;
    i_formatter &formatter;

public:
    logger_engine(i_stream_service &streamService, i_formatter &formatter) : streamService(streamService),
                                                                             formatter(formatter) {
        streamService.open();
    }

    ~logger_engine() {
        streamService.close();
    }

    void log(string line) {
        string formatedLine = formatter.format(line);
        streamService.log(formatedLine);
    }
};

int main() {
    file_stream_service css;
    formatter f;
    logger_engine l(css, f);
    l.log("first line");
    l.log("second line");
    return 0;
}