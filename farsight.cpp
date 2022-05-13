// std
#include <stack>
#include <vector>
#include <fstream>

// json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// IFF SDK
#include "iff.h"

int main()
{
    std::ifstream cfg_file("farsight.json");
    std::string config_str = { std::istreambuf_iterator<char>(cfg_file), std::istreambuf_iterator<char>() };

    auto config = json::parse(config_str, nullptr, true, true);
    auto it_chains = config.find("chains");
    if(it_chains == config.end())
    {
        printf("Invalid configuration provided: chains not found\n");
        return 1;
    }
    if(!it_chains->is_array())
    {
        printf("Invalid configuration provided: section 'chains' must be an array\n");
        return 1;
    }

    auto it_iff = config.find("IFF");
    if(it_iff == config.end())
    {
        printf("Unable to find IFF configuration section in config file\n");
        return 1;
    }
    iff_initialize(it_iff.value().dump().c_str());

    std::vector<iff_chain_handle_t> chains;
    for(json& chain_config : it_chains.value())
    {
        auto chain_handle = iff_create_chain(chain_config.dump().c_str(), [](const char* element_name, int error_code)
        {
            printf("Chain element %s reported an error: %d\n", element_name, error_code);
        });
        chains.push_back(chain_handle);
    }

    auto current_chain = chains[0];

    printf("Press Esc key to terminate program\n");
    getchar();

    iff_release_chain(current_chain);
    iff_finalize();

    return 0;
}
