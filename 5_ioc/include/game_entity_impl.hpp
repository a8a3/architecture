#pragma once

#include "game_entity_interface.hpp"

namespace ioc::entity {

class SpaceShip : public IGameEntity
{};

using SpaceShipUPtr = std::unique_ptr<SpaceShip>;

} // namespace ioc::entity