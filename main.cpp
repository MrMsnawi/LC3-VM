#include "lc3.hpp"
#include <cstdio>

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image-file>\n", argv[0]);
        return 2;
    }

    LC3 vm;

    if (!vm.load_image(argv[1])) {
        printf("Failed to load image: %s\n", argv[1]);
        return 1;
    }

    // Run the VM
    vm.run();

    return 0;
}
