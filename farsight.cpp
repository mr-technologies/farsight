// std
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
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
    const auto config = nlohmann::json::parse(std::ifstream(CONFIG_FILENAME), nullptr, true, true);
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
                [](const char* const element_name, const int error_code)
                {
                    std::ostringstream message;
                    message << "Chain element `" << element_name << "` reported an error: " << error_code;
                    iff_log(IFF_LOG_LEVEL_ERROR, message.str().c_str());
                });
        chain_handles.push_back(chain_handle);
    }

    iff_log(IFF_LOG_LEVEL_INFO, "Press Enter to terminate the program");
    std::getchar();

    for(const auto chain_handle : chain_handles)
    {
        iff_release_chain(chain_handle);
    }

    iff_finalize();

    return EXIT_SUCCESS;
}
