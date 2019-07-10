#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using namespace std;

class i_formatter {
public:
    typedef shared_ptr<i_formatter> ptr;
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
        ofs.open("log2.txt", ofstream::out | ofstream::app);
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

class i_injector {
public:
    virtual ~i_injector() = default;
    virtual i_stream_service::ptr construct_stream_service() = 0;
    virtual i_formatter::ptr construct_formatter() = 0;
};

class logger_engine {
    i_stream_service::ptr streamService;
    i_formatter:: ptr formatter;

public:
/*    logger_engine(i_stream_service &streamService, i_formatter &formatter) : streamService(streamService),
                                                                             formatter(formatter) {
        streamService.open();
    }*/

    logger_engine(i_injector& injector) {
        streamService = injector.construct_stream_service();
        formatter = injector.construct_formatter();
        streamService->open();
    }

    ~logger_engine() {
        streamService->close();
    }

    void log(string line) {
        string formatedLine = formatter->format(line);
        streamService->log(formatedLine);
    }
};

class console_injector: public i_injector {
    i_stream_service::ptr construct_stream_service() override {
        return i_stream_service::ptr(new console_stream_service);
    }
    virtual i_formatter::ptr construct_formatter() override {
        return i_formatter::ptr(new formatter);
    }
};

class file_injector: public i_injector {
    i_stream_service::ptr construct_stream_service() override {
        return i_stream_service::ptr(new file_stream_service);
    }
    virtual i_formatter::ptr construct_formatter() override {
        return i_formatter::ptr(new formatter);
    }
};

int main() {
    file_injector ci;
    logger_engine l(ci);
    l.log("first line");
    l.log("second line");
    return 0;
}