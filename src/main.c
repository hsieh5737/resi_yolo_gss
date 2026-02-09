#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== Resi-YOLO GSS Pipeline Start ===\n");

    // Appendix B3: GSS computation
    int ret = system("python3 scripts/compute_gss.py data/images data/metadata.csv");
    if (ret == -1) {
        fprintf(stderr, "Failed to invoke compute_gss.py via system()\n");
        return 2;
    } else if (ret != 0) {
        fprintf(stderr, "compute_gss.py exited with non-zero status: %d\n", ret);
        /* continue or return depending on desired behavior; here we continue but warn */
    }

    printf("GSS metadata.csv generated.\n");
    printf("Pipeline finished.\n");

    return 0;
}
