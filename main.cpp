#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class i_log_buffer {
public:
    virtual ~i_log_buffer() = default;
    virtual void open() = 0;
    virtual void write(const string &) = 0;
    virtual void close() = 0;
};

class console_buff: public i_log_buffer {
public:
    void open() override {}
    void write(const string &line) override {
        cout << line;
    }
    virtual void close() override {}
};

class file_buff: public i_log_buffer {
    string fname;
    ofstream ofs;
public:
    file_buff(const string &fname): fname(fname) {}
    void open() override {
        ofs.open("log.txt", ofstream::out | ofstream::app);
    }
    void write(const string &line) override {
        ofs << line;
    }
    void close() override {
        ofs.close();
    }
};

class i_formatter {
public:
    virtual ~i_formatter() = default;
    virtual string format(const string &) = 0;
};

class endliner: public i_formatter {
public:
    string format(const string &str) override {
        string res(str);
        res.append("\n");
        return res;
    }
};

class logger_engine {
    i_log_buffer &buff;
    i_formatter &format;
public:
    logger_engine(i_log_buffer &buff, i_formatter &format)
            : buff(buff), format(format) {
        buff.open();
    }
    ~logger_engine() {
        buff.close();
    }
    void log(const string& line) {
        buff.write(format.format(line));
    }
};

int main(int argc, const char *argv[])
{
    console_buff cb;
    file_buff fb("log.txt");
    endliner e;
    logger_engine l1(cb, e);
    l1.log("some line");
    logger_engine l2(fb, e);
    l2.log("some line");
    return 0;
}
