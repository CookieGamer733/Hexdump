#include <conio.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define VERSION "v1.1.0"

#define OFFSET_COLOR "\033[38;2;0;144;255m"
#define SIDEBAR_COLOR "\033[38;2;0;144;48m"
#define RESET_FORMATING "\033[0m"

static int help_flag;
static int version_flag;
static int ascii_flag;
static char *filename;

struct file_data {
  unsigned char *content;
  long size;
};

void handle_options(int argc, char *argv[]);
void help(void);
struct file_data read_file(char *$filename);
char *format_text(char *text, char *format);

int main(int argc, char *argv[]) {
  handle_options(argc, argv);

  if (help_flag) {
    printf("Hexdump %s\nUsage: hexdump [OPTION]... [FILE]\n\n", VERSION);
    exit(0);
  } else if (version_flag) {
    printf("Hexdump %s\n", VERSION);
    exit(0);
  } else if (!filename) {
    fprintf(stderr, "Missing File\n");
    printf("Hexdump %s\nUsage: hexdump [OPTION]... [FILE]\n\n", VERSION);
    exit(1);
  }

  struct file_data filedata = read_file(filename);

  DWORD l_mode;
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

  GetConsoleMode(hStdout, &l_mode);
  SetConsoleMode(hStdout, l_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING |
                              DISABLE_NEWLINE_AUTO_RETURN);

  printf("Hexdump of %s\n", filename);
  printf(OFFSET_COLOR);
  printf("  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
  printf(RESET_FORMATING);

  for (int i = 0; i < (((filedata.size) / 16) + 1); i++) {
    printf(OFFSET_COLOR);
    printf("%08X: ", i * 16);
    printf(RESET_FORMATING);
    for (int j = 0; j < 16; j++) {
      if (i * 16 + j < filedata.size) {
        printf("%02X ", filedata.content[i * 16 + j]);
      } else {
        printf("   ");
      }
    }

    if (ascii_flag) {
      printf("   ");

      printf(SIDEBAR_COLOR);

      for (int k = 0; k < 16; k++) {
        if (i * 16 + k < filedata.size) {
          char c = filedata.content[i * 16 + k];
          if (c < 33 || c > 126) {
            printf(".");
          } else {
            printf("%c", c);
          }
        } else {
          printf(" ");
        }
      }

      printf(RESET_FORMATING);
    }

    printf("\n");
  }
}

void handle_options(int argc, char *argv[]) {
  int c;
  int loop = 1;
  int index = 0;

  while (loop) {
    struct option long_options[] = {
        {"help", no_argument, &help_flag, 'h'},
        {"version", no_argument, &version_flag, 'v'},
        {"ascii", no_argument, &ascii_flag, 'a'}};

    c = getopt_long(argc, argv, "ghv", long_options, &index);

    if (c == -1) {
      loop = false;
      break;
    };

    switch (c) {
      case 'h':
        help_flag = 1;
        break;
      case 'v':
        version_flag = 1;
        break;
      case 'a':
        ascii_flag = 1;
        break;
      default:
        if (help_flag == 0 && version_flag == 0) exit(1);
    }
  }

  if (optind < argc) filename = argv[optind];
}

struct file_data read_file(char *$filename) {
  struct file_data filedata = {size : 0, content : ""};

  FILE *file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "Could not open file: %s", filename);
    exit(2);
  }

  fseek(file, 0, SEEK_END);
  filedata.size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (filedata.size >= 0xFFFFFFFF) {
    fprintf(stderr, "File is too large");
    exit(3);
  } else if (filedata.size >= 0x3000) {
    printf("File is very large meaning the operation can take a long time\n");
    printf("Are you sure you want to continue? (y/n) > ");
    char c = getch();
    if (tolower(c) != 'y') {
      fprintf(stderr, "Aborted");
      exit(4);
    }
  }

  filedata.content = malloc(filedata.size + 1);
  if (!filedata.content) {
    fprintf(stderr, "Could not allocate memory");
    exit(5);
  }

  size_t read = fread(filedata.content, 1, filedata.size, file);
  if (read != filedata.size) {
    fprintf(stderr, "Could not read file");
    free(filedata.content);
    exit(6);
  }

  fclose(file);

  filedata.content[filedata.size] = '\0';

  return filedata;
}