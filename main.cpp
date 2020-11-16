#include "Engine.hpp"


int main(int argc, char const** argv){
    Rooten::Engine* engine = new Rooten::Engine("StarkOS");
    engine->run();
    delete engine;
    return 0;
}
