#include "engine/reflection/reflection_system.hpp"

#include <entt/core/type_info.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/fwd.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>

#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"

namespace ls::reflection {

  void registerTypes() {
    using namespace entt::literals;
    entt::meta_factory<ls::component::Transform>{}
        .type(entt::type_id<ls::component::Transform>().hash(), "Transform")
        .data<&ls::component::Transform::scale>("scale"_hs, "Scale")
        .data<&ls::component::Transform::position>("position"_hs, "Position")
        .data<&ls::component::Transform::rotation>("rotation"_hs, "Rotation");

    entt::meta_factory<ls::component::Sprite>{}
        .type(entt::type_id<ls::component::Sprite>().hash(), "Sprite")
        .data<&ls::component::Sprite::color>("color"_hs, "Color")
        .data<&ls::component::Sprite::uvScale>("uvScale"_hs, "UV scale")
        .data<&ls::component::Sprite::textureId>("textureId"_hs, "Texture id")
        .data<&ls::component::Sprite::angleOffset>("angleOffset"_hs, "Angle offset")
        .data<&ls::component::Sprite::zIndex>("zIndex"_hs, "z-index");
  }

}  // namespace ls::reflection
