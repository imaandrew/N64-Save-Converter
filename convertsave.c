#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t bizhawk[0x48800];

static char* infile = NULL;
static char* outfile = NULL;
char* pj64_path = NULL;
char* m64p_path = NULL;
char* bizhawk_path = NULL;
static int directory = 0;

void byteswap(uint8_t* buf, size_t len) {
    uint8_t tmp;
    for (size_t x = 0; x < len; x += 4) {
        tmp = buf[x];
        buf[x] = buf[x + 3];
        buf[x + 3] = tmp;
        tmp = buf[x + 1];
        buf[x + 1] = buf[x + 2];
        buf[x + 2] = tmp;
    }
}

void create_save_bizhawk(uint8_t* buf, size_t len) {
    uint8_t init[] = {
        0x81,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b, 0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1a,0x1b, 0x1c,0x1d,0x1e,0x1f,
        0xff,0xff,0xff,0xff, 0x05,0x1a,0x5f,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0x01,0xff, 0x66,0x25,0x99,0xcd,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0x05,0x1a,0x5f,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0x01,0xff, 0x66,0x25,0x99,0xcd,
        0xff,0xff,0xff,0xff, 0x05,0x1a,0x5f,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0x01,0xff, 0x66,0x25,0x99,0xcd,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0x05,0x1a,0x5f,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0x01,0xff, 0x66,0x25,0x99,0xcd,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x71,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03
    };

    memset(bizhawk, 0, 0x800);
    for (int i = 0x800; i < 0x20000; i += 0x8000) {
        for (int j = 0; j < 0x8000; j += 2) {
            bizhawk[i + j] = 0;
            bizhawk[i + j + 1] = 0x03;
        }
        memcpy(&bizhawk[i], init, 272);
    }
    memset(bizhawk + 0x20800, 0xff, 0x20000);
    memset(bizhawk + 0x40800, 0, 0x8000);
    memcpy(bizhawk + (0x800 + 4 * 0x8000), buf, len);
}

void create_dirs() {
    struct stat sb;

    pj64_path = malloc(strlen(outfile) + strlen("Project64") + 3);
    m64p_path = malloc(strlen(outfile) + strlen("m64p") + 3);
    bizhawk_path = malloc(strlen(outfile) + strlen("BizHawk") + 3);

    if (!pj64_path || !m64p_path || !bizhawk_path) {
        perror("Could not create directory for saves");
        exit(1);
    }

    sprintf(pj64_path, "%s/%s", outfile, "Project64");
    sprintf(m64p_path, "%s/%s", outfile, "m64p");
    sprintf(bizhawk_path, "%s/%s", outfile, "BizHawk");

    if (stat(outfile, &sb) == -1) {
        mkdir(outfile, 0700);
    }

    if (stat(pj64_path, &sb) == -1) {
        mkdir(pj64_path, 0700);
    }

    if (stat(m64p_path, &sb) == -1) {
        mkdir(m64p_path, 0700);
    }

    if (stat(bizhawk_path, &sb) == -1) {
        mkdir(bizhawk_path, 0700);
    }

}

void create_save(char* name, uint8_t* buf, size_t fsize) {
    char* pj64_file = malloc(strlen(pj64_path) + strlen(name) + 6);
    char* m64p_file = malloc(strlen(m64p_path) + strlen(name) + 6);
    char* bizhawk_file = malloc(strlen(bizhawk_path) + strlen(name) + 10);
    char* orig_name = malloc(strlen(name) + 10);
    FILE* fp;

    strcpy(orig_name, name);

    sprintf(pj64_file, "%s/%s", pj64_path, strcat(name, ".fla"));
    sprintf(m64p_file, "%s/%s", m64p_path, name);
    sprintf(bizhawk_file, "%s/%s", bizhawk_path, strcat(orig_name, ".SaveRAM"));

    fp = fopen(pj64_file, "wb");

    fwrite(buf, 1, fsize, fp);
    fclose(fp);

    fp = fopen(m64p_file, "wb");

    fwrite(buf, 1, fsize, fp);
    fclose(fp);

    fp = fopen(bizhawk_file, "wb");

    fwrite(bizhawk, 1, 0x48800, fp);
    fclose(fp);
    free(pj64_file);
    free(m64p_file);
    free(bizhawk_file);
    free(orig_name);
}

void parse_file(char* in) {
    FILE* fp = fopen(in, "rb");
    size_t fsize;
    uint8_t* bytes;
    char* fname;
    char* dot;
    
    (fname = strrchr(in, '/')) ? fname++ : (fname = in);
    dot = strchr(fname, '.');
    fname[dot - fname] = '\0';

    if (!fp) {
        perror("Could not open input file");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    bytes = malloc(fsize);

    if (!bytes) {
        perror("Could not allocate buffer for input file");
        exit(1);
    }

    fread(bytes, 1, fsize, fp);
    fclose(fp);

    byteswap(bytes, fsize);
    create_save_bizhawk(bytes, fsize);
    create_save(fname, bytes, fsize);
    free(bytes);
}

int parse_directory(const char* a, const struct stat* b, int c) {

    if (c == FTW_F) {
        parse_file((char*)a);
    }
    return 0;
}

void print_usage() {
    puts("Usage:\n"
            "  convertsave -i <save file> -o <output directory>\n"
            "  convertsave -d -i <input directory> -o <output directory>\n"
            "  convertsave -h\n");
}

int main(int argc, char** argv) {
    int c;

    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    while ((c = getopt(argc, argv, "i:o:dh")) != -1) {
        switch (c) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'd':
                directory = 1;
                break;
            case 'h':
            case '?':
                print_usage();
                return 1;
            default:
                break;
        }
    }

    if (!infile) {
        perror("Input file must be specified");
        exit(1);
    }

    if (!outfile) {
        perror("Output file must be specified");
        exit(1);
    }

    create_dirs();
    if (directory) {
        ftw(infile, parse_directory, 100);    
    } else {
        parse_file(infile);
    }
    
    free(pj64_path);
    free(m64p_path);
    free(bizhawk_path);
}
