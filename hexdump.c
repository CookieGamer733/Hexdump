#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
#endif

typedef int bool;

#define false 0
#define true 1

#define VERSION "v3.0.1"

/* File Data Structure */
typedef struct filedata_t {
  unsigned char *content; /* content of the file */
  long size; /* size of the file */
} filedata_t;

int main(int argc, char *argv[]) {
  int index = 0; /* Index of the option in the long_options array */
  bool option_loop = true; /* Flag to indicate if we should continue */

  /* Flags to indicate if the user has specified the corresponding option */
  bool help_flag         = false;  /* Flag to indicate if we should print help message */
  bool version_flag      = false;  /* Flag to indicate if we should print version message */
  bool ascii_flag        = false;  /* Flag to indicate if we should print ascii */
  bool no_color_flag     = false;  /* Flag to indicate if we should print without color */
  bool output_file_flag  = false;  /* Flag to indicate if we should save to file */
  bool output_color_flag = false;  /* Flag to indicate if we should save to file with color */

  char *filename    = "";  /* Name of the file to be dumped */
  char *output_file = "";  /* Name of the file to be printed to */

  /* Supported options */
  const struct option options[] = {
    { "help",         no_argument,       &help_flag,         1 },
    { "version",      no_argument,       &version_flag,      1 },
    { "show-ascii",   no_argument,       &ascii_flag,        1 },
    { "no-color",     no_argument,       &no_color_flag,     1 },
    { "output",       required_argument, &output_file_flag,  1 },
    { "output-color", no_argument,       &output_color_flag, 1 }
  };

  while (option_loop) {
    /* Get the next option */
    int c = getopt_long(argc, argv, "hvo:", options, &index);

    /* If we have reached the end of the options, break */
    if (c == -1) {
      option_loop = false;
      break;
    }

    /* Set flags if short option is used */
    switch (c) {
      case 'h':
        help_flag = true;
        break;
      case 'v':
        version_flag = true;
        break;
      case 'o':
        output_file_flag = true;

        /* If the output file is not specified, print error message */
        if (optarg != NULL) {
          output_file = optarg;
        } else {
          printf("Error: No output file specified.\n");
          exit(1);
        }
        break;
      default:
        break;
    }
  }

  /* Only check for color priority if output option is used */
  if (output_file_flag) {
    /* Check for color priority after long options are parsed */
    if (!output_color_flag) no_color_flag = true;
    else no_color_flag = false;
  } else if (output_color_flag) {
    printf("Error: Output color option can only be used with output option.\n");
    exit(1);
  }

  /* Get the filename */
  if (optind < argc) filename = argv[optind];

  if (help_flag) {
    printf("Usage: hexdump [option]... [file]\n");
    printf("Dump the contents of FILE in hex format.\n");
    printf("\n");
    printf("  -h, --help\t\tdisplay this help and exit\n");
    printf("  -v, --version\t\toutput version information and exit\n");
    printf("  -o, --output [FILE]\tprint to FILE instead of stdout\n");
    printf("      --output-color\twhen writing to file, write color escape sequences as well\n");
    printf("      --show-ascii\tdisplay the ASCII characters\n");
    printf("      --no-color\tdisable color output\n");
    printf("\n");

    exit(0); /* Exit with success */
  } else if (version_flag) {
    printf("Hexdump %s\n", VERSION);

    exit(0); /* Exit with success */
  } else if (filename[0] == '\0') {
    printf("Error: no file specified\n");
    printf("\n");
    printf("Usage: hexdump [option]... [file]\n");
    printf("Try 'hexdump --help' for more information.\n");

    exit(1); /* Exit with error */
  }

  /* Structure to store the file data */
  filedata_t filedata = {
    size: 0,
    content: (unsigned char *)""
  };

  #ifdef _WIN32
    /* Enable VT100 escape sequences if color is enabled */
    if (no_color_flag == false) {
      DWORD mode; /* Console mode */
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); /* Get handle to stdout */

      GetConsoleMode(hStdout, &mode);
      SetConsoleMode(hStdout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    }
  #endif

  /* Open the file */
  FILE *file = fopen(filename, "rb");
  if (!file) {
    printf("Error: could not open file '%s'\n", filename);
    exit(1); /* Exit with error */
  }

  /* Get the size of the file */
  fseek(file, 0, SEEK_END);
  filedata.size = ftell(file);
  fseek(file, 0, SEEK_SET);

  /* Check if the file is too big */
  if (filedata.size >= 0xFFFFFFFF) {
    printf("Error: file is too big\n");
    exit(1);
  } else if (filedata.size == 0) {
    printf("Error: file is empty\n");
    exit(1);
  } else if (filedata.size >= 0x3000) {
    printf("Warning: file is very large. This may take a while.\n");
    printf("Do you want to continue? (y/n) ");
    if (tolower(getchar()) != 'y') {
      printf("Aborted\n");
      exit(1);
    }
  }

  /* Allocate memory for the file content */
  filedata.content = malloc(filedata.size);
  if (!filedata.content) {
    printf("Error: could not allocate memory\n");
    exit(1);
  }

  size_t read_size = fread(filedata.content, 1, filedata.size, file);
  if (read_size != filedata.size) {
    printf("Error: could not read file\n");
    free(filedata.content);
    exit(1);
  }

  /* Close the file */
  fclose(file);

  /* Set last byte to 0 */
  filedata.content[filedata.size] = '\0';

  /*
   * Define the colors used in the output.
   * However, if color is disabled, we don't use any colors
   *   so we define them as empty strings.
   */
  const char *ansi_reset = no_color_flag ? "" : "\x1b[0m"; /* Reset ANSI escape sequences */
  const char *offset_color = no_color_flag ? "" : "\x1b[38;2;0;144;255m"; /* Color for offset */
  const char *ascii_color = no_color_flag ? "" : "\x1b[38;2;0;144;48m"; /* Color for ASCII */

  FILE *stream = output_file_flag ? fopen(output_file, "w") : stdout;

  /* Dump hex values */

  fprintf(stream, "%s", offset_color);
  fprintf(stream, "  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
  fprintf(stream, "%s", ansi_reset);

  for (int i = 0; i < (((filedata.size - 1) / 16) + 1); i++) {
    /* Print offset */
    fprintf(stream, "%s", offset_color);
    fprintf(stream, "%08X: ", i * 16);
    fprintf(stream, "%s", ansi_reset);

    /* Print hex values */
    for (int j = 0; j < 16; j++) {
      if (i * 16 + j < filedata.size) {
        fprintf(stream, "%02X ", filedata.content[i * 16 + j]);
      } else {
        fprintf(stream, "   ");
      } 
    }

    /* Print ASCII values, if requested */
    if (ascii_flag) {
      fprintf(stream, "   ");

      fprintf(stream, "%s", ascii_color);

      for (int k = 0; k < 16; k++) {
        if (i * 16 + k < filedata.size) {
          char c = filedata.content[i * 16 + k];
          /* Check if the character is printable */
          if (c < 33 || c > 126) {
            fprintf(stream, ".");
          } else {
            fprintf(stream, "%c", c);
          }
        }
      }

      fprintf(stream, "%s", ansi_reset);
    }
  
    fprintf(stream, "%s\n", ansi_reset);
  }
  
  /* Free the memory allocated for the file content */
  free(filedata.content);

  return 0;
}