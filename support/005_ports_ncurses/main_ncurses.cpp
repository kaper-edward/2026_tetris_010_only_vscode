#include "LocalGameApp.hpp"
#include "NcursesInput.hpp"
#include "NcursesRenderer.hpp"

#include <cstdlib>

namespace {

void configure_ncurses_environment() {
#ifdef _WIN32
    if (std::getenv("TERM") == nullptr) {
        _putenv_s("TERM", "xterm");
    }
#ifdef TETRIS005_NCURSES_TERMINFO_DIR
    if (std::getenv("TERMINFO") == nullptr) {
        _putenv_s("TERMINFO", TETRIS005_NCURSES_TERMINFO_DIR);
    }
#endif
#endif
}

}  // namespace

int main() {
    configure_ncurses_environment();
    NcursesRenderer renderer;
    NcursesInput input;
    renderer.initialize();
    {
        LocalGameApp app(GameCore(888), renderer, input);
        app.run();
    }
    renderer.shutdown();
    return 0;
}
