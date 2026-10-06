#include "compiler.h"
#include "vm.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::string read_file(const fs::path &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("cannot open " + path.string());
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

static int validate_one(const fs::path &path) {
  Compiler compiler;
  VM vm;
  try {
    compiler.compile(read_file(path), vm);
    std::cout << "OK  " << path << "  (ops=" << vm.code.size() << ", scenes=" << vm.scenes.size()
              << ")\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "FAIL " << path << "\n  " << e.what() << "\n";
    return 1;
  }
}

int main(int argc, char **argv) {
  std::vector<fs::path> roots;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i)
      roots.emplace_back(argv[i]);
  } else {
    roots.emplace_back("quests");
  }

  int failures = 0;
  int total = 0;
  for (const auto &root : roots) {
    if (!fs::exists(root)) {
      std::cerr << "missing path: " << root << "\n";
      return 2;
    }
    if (fs::is_regular_file(root)) {
      ++total;
      failures += validate_one(root);
      continue;
    }
    for (auto it = fs::recursive_directory_iterator(root); it != fs::recursive_directory_iterator();
         ++it) {
      if (!it->is_regular_file())
        continue;
      const auto ext = it->path().extension().string();
      // Skip catalog/notes (.txt); validate .txt only when passed explicitly
      if (ext != ".forth" && ext != ".fth")
        continue;
      ++total;
      failures += validate_one(it->path());
    }
  }

  std::cout << "Validated " << total << " script(s), failures=" << failures << "\n";
  return failures == 0 ? 0 : 1;
}
