#pragma once

#include <any>
#include <format>
#include <functional>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <array>
#include <string_view>
#include <cstdint>
#include <unordered_map>

#include "command_interface.hpp"
#include "game_entity_interface.hpp"

namespace ioc {

enum class ObjType : uint16_t {
    Unknown = 0,

    Register = 1,
    Unregister = 2,

    Inc = 3,
    IncNTimes = 4,

    SpaceShip = 5,
    Comet = 6
};
} // namespace ioc

namespace std {

template<>
struct formatter<ioc::ObjType> : formatter<string_view> {
    auto format(ioc::ObjType t, std::format_context& ctx) const {
        static constexpr array<string_view, 7> names = {
            "Unknown",
            "Register",
            "Unregister",
            "Inc",
            "IncNTimes",
            "SpaceShip",
            "Comet"
        };
        return formatter<string_view>::format(names[static_cast<size_t>(t)], ctx);
    }
};

} // namespace std

namespace ioc {

class Container {
public:
    static Container& getInstance() {
        static thread_local Container c;
        return c;
    }

    // service commands
    class ServiceCommand : public command::ICommand {
    public:
        using ServiceFunc = std::function<void()>;
        explicit ServiceCommand(ServiceFunc rf) : servFunc_{rf} {}

        void Execute() override {
            servFunc_();
        }
    private:
        ServiceFunc servFunc_;
    };

    template<typename T, typename... Args>
    using Generator = std::function<std::unique_ptr<T>(Args...)>;

    template<typename T, typename... Args>
    std::unique_ptr<T> Resolve(ObjType type, Args&&... args) {
        auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);

        if constexpr (sizeof...(Args) >= 2) {
            if (ObjType::Register == type) {
                auto subtype = std::get<0>(args_tuple);
                auto genFunc = std::get<1>(args_tuple);

                return std::make_unique<ServiceCommand>([this, subtype, genFunc] () {
                    auto typeIndexIter = typeRegistry_.find(subtype);
                    if (std::end(typeRegistry_) == typeIndexIter) {
                        throw std::runtime_error(std::format("Could not find object type for [{}] type", subtype));
                    }
                    registry_[typeIndexIter->second][subtype] = std::move(genFunc);
                });
            }
            throw std::runtime_error(std::format("Unexpected ServiceCommand [{}]", type));
        } else {
            auto typeIter = registry_.find(typeid(T));
            if (std::end(registry_) == typeIter) {
                throw std::runtime_error(std::format("Could not find generator for [{}] type", typeid(T).name()));
            }
    
            auto genIter = typeIter->second.find(type);
            if (std::end(typeIter->second) == genIter) {
                throw std::runtime_error(std::format("Could not find generator for [{}] command type", type));
            }
    
            using GenType = Generator<T, std::remove_cvref_t<Args>...>;
            auto gen = std::any_cast<GenType>(genIter->second);
            return gen(std::forward<Args>(args)...);
        }
    }

private:
    Container() = default;

    using Actions = std::unordered_map<ObjType, std::any>;
    std::unordered_map<std::type_index, Actions> registry_;

    static inline std::unordered_map<ObjType, std::type_index> typeRegistry_ {
         {ObjType::Register,   typeid(command::ICommand)}
        ,{ObjType::Unregister, typeid(command::ICommand)}
        ,{ObjType::Inc,        typeid(command::ICommand)}
        ,{ObjType::IncNTimes,  typeid(command::ICommand)}
        ,{ObjType::SpaceShip,  typeid(entity::IGameEntity)}
    };
};

} // namespace ioc