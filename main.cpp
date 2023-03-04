#include "rabbit.h"
int main() {
    if(rabbit::Init() ){
        RT_INST->Run();
        rabbit::Delete();
    }
    return 0;
}
