#include "SNOL.h"

int main() {
    SNOLInterpreter interpreter;
    std::cout << "\033[H\033[J"; // clear scr
    interpreter.run();
    return 0;
}
