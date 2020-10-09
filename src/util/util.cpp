#include "util.hpp"

int Util::read_file(char **buffer, size_t *size, const char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        nm_log::log(LOG_ERROR, "failed to open file \"%s\"\n", file_name);

        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    *buffer = new char[*size + 1]; // +1 for null terminator
    if (!(*buffer)) {
        nm_log::log(LOG_ERROR, "failed to allocate %d bytes\n", *size);
        fclose(file);

        return EXIT_FAILURE;
    }

    size_t read_size = fread(*buffer, sizeof(char), *size, file);
    fclose(file);

    if (read_size != *size) {
        nm_log::log(LOG_ERROR, "failed to read all bytes in file\n", *size);

        return EXIT_FAILURE;
    }

    (*buffer)[*size] = '\0';

    return EXIT_SUCCESS;
}