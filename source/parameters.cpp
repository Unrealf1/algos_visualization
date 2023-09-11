#include "parameters.hpp"
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/spdlog.h>

#include <fstream>

#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif // __GNUC__

#include <magic_enum.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__


#include <util.hpp>


static std::unordered_map<std::size_t, ApplicationParams> s_params_cache;

ApplicationParams& get_cached_application_params(const std::filesystem::path& path, bool force_update) {
    auto hash = std::filesystem::hash_value(path);
    auto existing = s_params_cache.find(hash);
    bool load = force_update || existing == s_params_cache.end();
    if (load) {
        auto assign_result = s_params_cache.insert_or_assign(hash, load_application_params(path));
        return assign_result.first->second;
    }
    return existing->second;
}

ApplicationParams load_application_params(const std::filesystem::path& path) {
    using json = nlohmann::json;
    static_assert(
        std::size(ApplicationParams::s_field_names_in_order) == boost::pfr::tuple_size<ApplicationParams>::value,
        "field names have to correspond to fields 1 to 1"
    );
    if (!std::filesystem::exists(path)) {
        auto message = fmt::format("configuration file \"{}\" does not exist", path.string());
        throw std::logic_error(message);
    }
    std::ifstream file(path);
    json data = json::parse(file, nullptr, true, true);
    ApplicationParams parameters{};
    boost::pfr::for_each_field(parameters, [&]<typename T>(T& field, size_t idx) {
        const auto& name = ApplicationParams::s_field_names_in_order[idx];
        if (!data.contains(name)) {
            auto errmsg = fmt::format("{} does not contain {} parameter", path.string(), name);
            spdlog::error(errmsg);
            throw std::logic_error(errmsg);
        }
        if constexpr (std::is_enum_v<T>) {
            auto value_name = data[name].get<std::string>();
            auto opt = magic_enum::enum_cast<T>(value_name);
            if (!opt.has_value()) {
                auto message = fmt::format("{} is not a valid option for \"{}\". Acceptable: {}",
                        value_name, name, magic_enum::enum_names<T>());
                spdlog::error(message);
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
