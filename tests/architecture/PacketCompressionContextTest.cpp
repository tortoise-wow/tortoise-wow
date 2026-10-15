#include "PacketCompressionContext.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <cstdint>

static void Check(bool value) { if (!value) throw std::runtime_error("compression mismatch"); }
static void RoundTrip()
{
    PacketCompressionContext context;
    std::unique_ptr<libdeflate_decompressor, decltype(&libdeflate_free_decompressor)>
        decoder(libdeflate_alloc_decompressor(), libdeflate_free_decompressor);
    Check(bool(decoder));
    for (int level : {0,1,6,12,6,1})
    {
        auto* compressor = context.Get(level);
        Check(context.Get(level) == compressor);
        for (size_t size : {size_t(0),size_t(16),size_t(255),size_t(32768)})
        {
            std::vector<uint8_t> input(size), decoded(size), output(libdeflate_zlib_compress_bound(nullptr,size));
            uint32_t state = 987123;
            for (auto& value : input) { state = state * 1664525 + 1013904223; value = uint8_t(state >> 24); }
            size_t bytes = libdeflate_zlib_compress(compressor,input.data(),size,output.data(),output.size());
            Check(bytes > 0);
            size_t decodedSize = 0;
            Check(libdeflate_zlib_decompress(decoder.get(),output.data(),bytes,decoded.data(),size,&decodedSize) == LIBDEFLATE_SUCCESS);
            Check(decodedSize == size && decoded == input);
        }
    }
    bool failed = false;
    try { context.Get(99); } catch (std::bad_alloc const&) { failed = true; }
    Check(failed && context.Get(1) != nullptr);
}
int main()
{
    std::vector<std::thread> workers;
    for (int n=0; n<4; ++n) workers.emplace_back(RoundTrip);
    for (auto& worker : workers) worker.join();
    // Comparative workload only, never an assertion about whole-server speed.
    constexpr int iterations = 10000;
    std::vector<uint8_t> input(96, 7), output(libdeflate_zlib_compress_bound(nullptr,96));
    auto measure = [&](bool reuse) {
        PacketCompressionContext context;
        auto start = std::chrono::steady_clock::now();
        for (int n=0; n<iterations; ++n)
        {
            if (reuse)
                Check(libdeflate_zlib_compress(context.Get(1),input.data(),input.size(),output.data(),output.size()) != 0);
            else
            {
                auto* bound = libdeflate_alloc_compressor(1);
                Check(bound != nullptr);
                Check(libdeflate_zlib_compress_bound(bound,input.size()) <= output.size());
                libdeflate_free_compressor(bound);
                auto* compressor = libdeflate_alloc_compressor(1);
                Check(compressor != nullptr);
                Check(libdeflate_zlib_compress(compressor,input.data(),input.size(),output.data(),output.size()) != 0);
                libdeflate_free_compressor(compressor);
            }
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
    };
    std::cout << "PASS: zlib roundtrip, all size boundaries, level reload, concurrency, allocation failure\n";
    std::cout << "10000 small packets: old_us=" << measure(false) << " reused_us=" << measure(true) << '\n';
}
