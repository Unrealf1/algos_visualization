#include "parameters.hpp"
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/ranges.h>

#include <fstream>

#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>
#include <magic_enum.hpp>
#include <util.hpp>


ApplicationParams load_application_params(const std::filesystem::path& path) {
    using json = nlohmann::json;
    static_assert(
        std::size(ApplicationParams::s_field_names_in_order) == boost::pfr::tuple_size<ApplicationParams>::value,
        "field names have to correspond to fields 1 to 1"
    );
    std::ifstream file(path);
    json data = json::parse(file, nullptr, true, true);
    static ApplicationParams parameters{};
    boost::pfr::for_each_field(parameters, [&]<typename T>(T& field, size_t idx) {
        const auto& name = ApplicationParams::s_field_names_in_order[idx];
        if constexpr (std::is_enum_v<T>) {
            auto value_name = data[name].get<std::string>();
            auto opt = magic_enum::enum_cast<T>(value_name);
            if (!opt.has_value()) {
                auto message = fmt::format("{} is not a valid option for \"{}\". Acceptable: {}",
                        value_name, name, magic_enum::enum_names<T>());
                throw std::logic_error(message);
            }
            field = opt.value();
        } else {
            field = data[name];
        }
    });
    return parameters;
}

void save_application_parameters(const ApplicationParams& parameters, const std::filesystem::path& path) {
    using json = nlohmann::json;
    json data;
    boost::pfr::for_each_field(parameters, [&]<typename T>(T& field, size_t idx) {
        const auto& name = ApplicationParams::s_field_names_in_order[idx];
        if constexpr (std::is_enum_v<T>) {
            data[name] = magic_enum::enum_name(field);
        } else {
            data[name] = field;
        }
    });
    std::ofstream file(path);
    const int pretty_tabulation = 4;
    file << data.dump(pretty_tabulation);
}
