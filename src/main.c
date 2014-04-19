#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, const char * argv[])
{
	FILE    *ifp, *ofp;
    uint16_t    magic;      /*  0 */
    uint32_t    offset;     /* 10 */
    int32_t     width;      /* 18 */
    int32_t     height;     /* 22 */
    uint16_t    bpp;       /* 28 */
    char        *row;

    switch (argc) {
        case 2:
            ofp = stdout;
            break;
        case 3:
            ofp = fopen(argv[2], "rw+");
            if ( ofp == NULL) {
                perror("Can't open file for writing:");
                exit(EXIT_FAILURE);
            }
            break;

        default:
                printf("Usage: %s <infile> <outfile>\n", PACKAGE_NAME);
                exit(EXIT_SUCCESS);
            break;
    }

    ifp = fopen(argv[1], "rb");

    if (ifp == NULL) {
        perror("Can't open file:");
        exit(EXIT_FAILURE);
    }

    fread(&magic, sizeof(magic), 1, ifp);
    if ( magic != 0x4D42) {
        fclose(ifp);
        fprintf(stderr, "This is not a BMP file");
        exit(EXIT_FAILURE);
    }

    fseek(ifp, 10, SEEK_SET);
    fread(&offset, sizeof(offset), 1, ifp);

    fseek(ifp, 18, SEEK_SET);
    fread(&width, sizeof(width), 1, ifp);
    fread(&height, sizeof(height), 1, ifp);

    fseek(ifp, 28, SEEK_SET);
    fread(&bpp, sizeof(bpp), 1, ifp);

    #ifdef DEBUG
        fprintf(stderr, "Magic:  0x%x\n", magic);
        fprintf(stderr, "Offset: %u\n", offset);
        fprintf(stderr, "Width:  %i\n", width);
        fprintf(stderr, "Height: %i\n", height);
        fprintf(stderr, "Bits:   %i\n", bpp);
    #endif

    if (bpp != 1) {
        fprintf(stderr, "Error: Expected 1-bit BMP file, not %i-bit.\n", bpp);
        fclose(ifp);
        fclose(ofp);
        exit(EXIT_FAILURE);
    }

    /* 1-bit per pixel */

    int rsize = ((bpp*abs(width)+31)/32)*4;

    row = malloc (rsize);
    if (row == NULL) {
        perror("Can't allocate memory");
        exit(EXIT_FAILURE);
    }

    fseek(ifp, offset, SEEK_SET);

    for (int i = 0; i < abs(height); i++) {
        fread(row, rsize, 1, ifp);
        char * p = row;
        for (int j = 0; j < width; j+=8) {

            /* print bits */
            for (int k = 0; k < (width - j < 8 ? width - j : 8); k++) {
                fprintf(ofp, "%u", *p & 128 ? 1 : 0);
                *p = *p << 1;
            }
            /* next byte */
            p++;
        }
        fprintf(ofp, "\n");
    }
    free(row);
    fclose(ofp);
    fclose(ifp);
    return 0;

}

