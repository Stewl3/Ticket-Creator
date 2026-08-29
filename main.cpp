#include "MainMenu.h"

#include <filesystem>

int main(int, char *argv[]) {
    namespace fs = std::filesystem;

    const fs::path executablePath = fs::absolute(argv[0]);
    const fs::path projectPath = executablePath.parent_path().parent_path();
    fs::current_path(projectPath);

    const fs::path legacyDataPath = "output/output";
    const fs::path dataPath = "output";
    if (fs::exists(legacyDataPath)) {
        fs::copy(legacyDataPath, dataPath,
                 fs::copy_options::recursive | fs::copy_options::skip_existing);
    }

    MainMenu menu;
    menu.run();
    return 0;
}