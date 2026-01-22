// cli/src/util/output.h
#ifndef OPENDE_OUTPUT_H
#define OPENDE_OUTPUT_H

// ANSI color codes (disabled if not a TTY)
void output_init(void);

// Formatted output functions
void print_info(const char *fmt, ...);
void print_error(const char *fmt, ...);
void print_warn(const char *fmt, ...);
void print_success(const char *fmt, ...);

// Status display helpers
void print_setting(const char *name, const char *value, int enabled);
void print_header(const char *title);

#endif
