/* A special header which is used together with a resource generator */

#include <stddef.h>

/**
 * @brief Get memory pointer and block size to resource file
 * @param [IN] file name of resource file
 * @param [OUT] mem reference to null pointer
 * @param [OUT] size reference to size variable
 */
extern bool RES_getMem(const char* file, unsigned char* &mem, size_t &size);

