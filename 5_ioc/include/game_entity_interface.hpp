#pragma once

#include <memory>

namespace ioc::entity {

class IGameEntity {
public:
    virtual ~IGameEntity() = default;
};

using IGameEntityUPtr = std::unique_ptr<IGameEntity>;
using IGameEntitySPtr = std::shared_ptr<IGameEntity>;

} // namespace ioc::entity