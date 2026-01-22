// cli/src/util/config.h
#ifndef OPENDE_CONFIG_H
#define OPENDE_CONFIG_H

// Get path to user config file, creating directory if needed
// Returns allocated string (caller must free) or NULL on error
char *config_get_user_path(const char *filename);

// Get path to system config file
// Returns allocated string (caller must free) or NULL on error
char *config_get_system_path(const char *filename);

// Ensure directory exists for a file path
// Returns 0 on success, -1 on error
int config_ensure_dir(const char *filepath);

// Check if file exists and is readable
int config_file_exists(const char *path);

#endif
