#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <set>
#include <unistd.h>

using namespace std;

class message {
    string text;
public:
    typedef shared_ptr<message> ptr;
    message(const string &text) : text(text) {}

    message() {}

    virtual string getText() {
        return "MSG: " + text;
    }
};

class message_decorator : public message {
protected:
    message::ptr msg;

    message_decorator(message::ptr msg) : msg(msg) {}
};

class date_message_decorator : public message_decorator {
    chrono::system_clock::time_point tp;
public:
    date_message_decorator(message::ptr msg, chrono::system_clock::time_point tp) : message_decorator(msg), tp(tp) {}

    string getText() override {
        std::time_t t = chrono::system_clock::to_time_t(tp);
        std::string t1( ctime( &t ) );
        return t1.substr( 0, t1.length() -1  ) + " " + msg->getText();
    }
};

class type_message_decorator : public message_decorator {
    string type;
public:
    type_message_decorator(message::ptr msg, string type) : message_decorator(msg), type(type) {}

    string getText() override {
        return "Type: " + type + " " + msg->getText();
    }
};

class pid_message_decorator : public message_decorator {
    pid_t pid;
public:
    pid_message_decorator(message::ptr msg, pid_t pid) : message_decorator(msg),
                                                     pid(pid) {}

    string getText() override {
        stringstream ss;
        ss << "PID: " << pid << " " << msg->getText();
        return ss.str();
    }
};

class tid_message_decorator : public message_decorator {
    thread::id tid;
public:
    tid_message_decorator(message::ptr msg, thread::id tid) : message_decorator(msg),
                                                          tid(tid) {}

    string getText() override {
        stringstream ss;
        ss << "TID: " << tid << " " << msg->getText();
        return ss.str();
    }
};

class i_log_buffer {
public:
    typedef shared_ptr<i_log_buffer> ptr;

    virtual ~i_log_buffer() = default;

    virtual void open() = 0;

    virtual void write(const string &) = 0;

    virtual void close() = 0;
};

class console_buff : public i_log_buffer {
public:
    void open() override {}

    void write(const string &line) override {
        cout << line;
    }

    virtual void close() override {}
};

class file_buff : public i_log_buffer {
    string fname;
    ofstream ofs;
public:
    file_buff(const string &fname) : fname(fname) {}

    void open() override {
        ofs.open(fname, ofstream::out | ofstream::app);
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

class endliner : public message_decorator {
public:
    endliner(ptr msg) : message_decorator(msg) {}

    string getText() override {
        string res(msg->getText());
        res.append("\n");
        return res;
    }
};


class log_chain {
public:
    typedef shared_ptr<log_chain> ptr;

    log_chain(ptr next, i_log_buffer::ptr buffer) : next(next), buffer(buffer) { buffer->open();}
    ~log_chain() { buffer->close(); }

    void log(const string& msg) {
        buffer->write(msg);
        if (next) next->log(msg);
    }
private:
    ptr next;
    i_log_buffer::ptr buffer;
};

class logger_engine {
    log_chain::ptr root;
public:
    typedef shared_ptr<logger_engine> ptr;
    logger_engine(log_chain::ptr root) : root(root) {}
    void log(message::ptr msg) {
        root->log(msg->getText());
    }
};

class factory {
public:
    static logger_engine::ptr create (const set<string>& types) {
        log_chain::ptr current, next;
        for (string type : types) {
            if (type == "console")
                next.reset(new log_chain(current, i_log_buffer::ptr(new console_buff)));
            else
                next.reset(new log_chain(current, i_log_buffer::ptr(new file_buff(type))));

            current = next;
        }
        return logger_engine::ptr(new logger_engine(current));
    }
};

class service_locator {
    static logger_engine::ptr le;
public:
    static logger_engine::ptr get_le() {
        return le;
    }
    static void set_le(logger_engine::ptr le) {
        service_locator::le = le;
    }
};
logger_engine::ptr service_locator::le;

class log_stream {
    stringstream ss;
    string type;
public:
    log_stream(string type) : type(type) {};
    stringstream& get () {
        return ss;
    }
    ~log_stream() {
        message::ptr msg(new endliner(message::ptr(new date_message_decorator(message::ptr(
                new type_message_decorator(message::ptr(
                        new pid_message_decorator(message::ptr(new tid_message_decorator(message::ptr(new message(ss.str())), this_thread::get_id())), getpid())), type)), chrono::system_clock::now()))));
      service_locator::get_le()->log(msg);

    }
};



#define INIT_LOGGER(types) service_locator::set_le(factory::create(types))
#define TRACE_LOG log_stream("TRACE").get()
#define ERROR_LOG log_stream("ERROR").get()
#define TERMINATE_LOGGER service_locator::set_le(nullptr)

int main(int argc, const char *argv[]) {
    INIT_LOGGER(set<string>({"console", "log.txt", "log2.txt"}));
    TRACE_LOG << "Wiadomosc Romana, działa, ale słabo!!!";
    ERROR_LOG << "Wiadomosc Romana, działa, ale słabo???";
    TERMINATE_LOGGER;
    return 0;
}
