/*
 * IFF SDK samples (https://mr-te.ch/iff-sdk) are licensed under MIT License.
 *
 * Copyright (c) 2022-2025 MRTech SK, s.r.o.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// std
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// json
#include <nlohmann/json.hpp>

// IFF SDK
#include <iff.h>

#ifdef __aarch64__
#pragma message("Make sure that configuration file uses YV12 output format instead of default NV12")
#endif


constexpr char CONFIG_FILENAME[] = "farsight.json";

int main()
{
    nlohmann::json config;
    try
    {
        config = nlohmann::json::parse(std::ifstream(CONFIG_FILENAME), nullptr, true, true);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Invalid configuration provided: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    const auto it_chains = config.find("chains");
    if(it_chains == config.end())
    {
        std::cerr << "Invalid configuration provided: missing `chains` section\n";
        return EXIT_FAILURE;
    }
    if(!it_chains->is_array())
    {
        std::cerr << "Invalid configuration provided: section `chains` must be an array\n";
        return EXIT_FAILURE;
    }
    if(it_chains->empty())
    {
        std::cerr << "Invalid configuration provided: section `chains` must not be empty\n";
        return EXIT_FAILURE;
    }
    const auto it_iff = config.find("IFF");
    if(it_iff == config.end())
    {
        std::cerr << "Invalid configuration provided: missing `IFF` section\n";
        return EXIT_FAILURE;
    }

    iff_initialize(it_iff->dump().c_str());

    std::vector<iff_chain_handle_t> chain_handles;
    for(const auto& chain_config : *it_chains)
    {
        const auto chain_handle = iff_create_chain(chain_config.dump().c_str(),
                                                   [](const char* element_name, int error_code, void*)
                                                   {
                                                       std::ostringstream message;
                                                       message << "Chain element `" << element_name << "` reported an error: " << error_code;
                                                       iff_log(IFF_LOG_LEVEL_ERROR, "farsight", message.str().c_str());
                                                   },
                                                   nullptr);
        chain_handles.push_back(chain_handle);
    }

    iff_log(IFF_LOG_LEVEL_INFO, "farsight", "Press Enter to terminate the program");
    std::getchar();

    for(const auto chain_handle : chain_handles)
    {
        iff_release_chain(chain_handle);
    }

    iff_finalize();

    return EXIT_SUCCESS;
}
