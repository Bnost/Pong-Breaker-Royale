/*
 * check_stun_sprite.c
 * Replaces check_grid.py and check_grid2.py
 *
 * Analyzes stuneffect.png to determine its sprite-sheet grid layout
 * by reading the PNG file header directly (no external libs needed).
 *
 * Compile: gcc check_stun_sprite.c -o check_stun_sprite
 * Run:     ./check_stun_sprite [filename.png]  (defaults to stuneffect.png)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Read a 4-byte big-endian unsigned int from a buffer */
static uint32_t read_be32(const uint8_t *buf)
{
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] <<  8) |
           ((uint32_t)buf[3]);
}

int main(int argc, char *argv[])
{
    const char *filename = (argc > 1) ? argv[1] : "stuneffect.png";

    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open '%s'\n", filename);
        return 1;
    }

    /* --- Verify PNG signature (first 8 bytes) --- */
    static const uint8_t PNG_SIG[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    uint8_t sig[8];
    if (fread(sig, 1, 8, f) != 8 || memcmp(sig, PNG_SIG, 8) != 0) {
        fprintf(stderr, "Error: '%s' is not a valid PNG file\n", filename);
        fclose(f);
        return 1;
    }

    /*
     * IHDR chunk layout (immediately after signature):
     *   4 bytes  - chunk data length
     *   4 bytes  - chunk type ("IHDR")
     *   4 bytes  - image width  (big-endian)
     *   4 bytes  - image height (big-endian)
     *   ...
     */
    fseek(f, 16, SEEK_SET);   /* skip sig(8) + length(4) + "IHDR"(4) */

    uint8_t whbuf[8];
    if (fread(whbuf, 1, 8, f) != 8) {
        fprintf(stderr, "Error: Could not read image dimensions\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    uint32_t w = read_be32(whbuf);
    uint32_t h = read_be32(whbuf + 4);

    printf("=== Sprite Sheet Analyzer ===\n");
    printf("File      : %s\n", filename);
    printf("Dimensions: %u x %u px\n\n", w, h);

    /* Check common square frame sizes */
    int frame_sizes[] = {64, 128, 256, 512};
    int found = 0;

    printf("--- Grid Candidates ---\n");
    for (int i = 0; i < (int)(sizeof(frame_sizes)/sizeof(frame_sizes[0])); i++) {
        int s = frame_sizes[i];
        if (w % s == 0 && h % s == 0) {
            int cols  = (int)(w / s);
            int rows  = (int)(h / s);
            int total = cols * rows;
            printf("Frame %4dx%-4d -> %d col(s) x %d row(s) = %d frame(s)\n",
                   s, s, cols, rows, total);
            found = 1;
        }
    }

    if (!found) {
        printf("No standard square-frame grid matched.\n");
    }

    /* Suggest the most likely grid for the in-game constants */
    printf("\n--- Recommended C constants ---\n");
    /* Use the largest frame size that fits evenly */
    int best = 0;
    for (int i = 0; i < (int)(sizeof(frame_sizes)/sizeof(frame_sizes[0])); i++) {
        int s = frame_sizes[i];
        if (w % s == 0 && h % s == 0) best = s;
    }
    if (best > 0) {
        int cols = (int)(w / best);
        int rows = (int)(h / best);
        printf("int stunCols      = %d;\n", cols);
        printf("int stunRows      = %d;\n", rows);
        printf("int stunMaxFrames = %d;\n", cols * rows);
    }

    return 0;
}
