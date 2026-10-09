#ifndef MANGOS_PACKET_COMPRESSION_CONTEXT_H
#define MANGOS_PACKET_COMPRESSION_CONTEXT_H

#include "libdeflate.h"
#include <memory>
#include <new>

// A context belongs to one sending thread, never to a world object. Reusing
// libdeflate's scratch storage avoids allocating it twice for every NPC move.
class PacketCompressionContext
{
public:
    libdeflate_compressor* Get(int level)
    {
        if (!m_compressor || level != m_level)
        {
            Pointer replacement(libdeflate_alloc_compressor(level), libdeflate_free_compressor);
            if (!replacement)
                throw std::bad_alloc();
            m_compressor = std::move(replacement);
            m_level = level;
        }
        return m_compressor.get();
    }

private:
    using Pointer = std::unique_ptr<libdeflate_compressor, decltype(&libdeflate_free_compressor)>;
    Pointer m_compressor{nullptr, libdeflate_free_compressor};
    int m_level = -1;
};

#endif
