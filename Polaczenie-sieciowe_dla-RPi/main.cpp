////////////////////////////////////////////////////////////

#include <fstream>
#include "main_functions.hpp"
#include "System.hpp"

int main(){
    auto system = System();

    while (true){
        bool is_end = system.update();

        if(is_end)
            break;

    }
}