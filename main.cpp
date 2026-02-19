#include "lc3.hpp"
#include <cstdio>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image-file1> ...\n", argv[0]);
        return 2;
    }

    LC3 vm;

    // Load each program image
    for (int i = 1; i < argc; i++) {
        if (!vm.load_image(argv[i])) {
            printf("Failed to load image: %s\n", argv[i]);
            return 1;
        }
    }

    // Run the VM
    vm.run();

    return 0;
}
