#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class logger_engine {
    ofstream ofs;
public:
    logger_engine() {
        ofs.open("log.txt", ofstream::out | ofstream::app);
    }
    ~logger_engine() {
        ofs.close();
    }
    void log (const string& line) {
        ofs << line << endl;
    }
};

int main() {
    logger_engine l;
    l.log("some line");
    return 0;
}