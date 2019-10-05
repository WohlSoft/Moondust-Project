#define LUABIND_BUILDING

#include <luabind/config.hpp>
#include <luabind/memory.hpp>
#include <memory>

static void* __cdecl luabind_allocator(void* /*context*/, const void* pointer, size_t const size)
{
    if (!size)
    {
        void* non_const_pointer = const_cast<void*>(pointer);
        std::free(non_const_pointer);
        return nullptr;
    }
    if (!pointer)
    {
        return std::malloc(size);
    }
    void* non_const_pointer = const_cast<void*>(pointer);
    return std::realloc(non_const_pointer, size);
}

namespace luabind
{
    allocator_func allocator = &luabind_allocator;
    void* allocator_context = nullptr;
}
