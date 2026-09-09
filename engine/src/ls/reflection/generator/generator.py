from macro_data import HeaderData
from reflection_info import ComponentInfo, EnumInfo, PropertyInfo


def bool_to_string(value: bool) -> str:
    return "true" if value else "false"


def string_to_bool(value: str) -> bool:
    return value.strip().lower() in ("true", "1")


def parse_float_literal(value: str) -> float:
    clean_value = value.strip()
    if clean_value.lower().endswith("f"):
        clean_value = clean_value[:-1]
    return float(clean_value)


def get_full_name(namespace: str, name: str) -> str:
    return f"{namespace}::{name}" if namespace else name


def generate_code(headers_data: list[HeaderData]) -> str:
    includes: list[str] = []
    component_factories_code: list[str] = []
    enum_factories_code: list[str] = []

    for header_data in headers_data:
        includes.append(f'#include "{header_data.path}"')

        for component_data in header_data.components:
            component_info = ComponentInfo()
            full_name = get_full_name(component_data.namespace, component_data.name)

            component_info.display_name = component_data.get_attribute_value(
                "DisplayName", default=component_data.name
            )
            component_info.is_component = component_data.is_component

            component_factory_code: list[str] = [
                f"        entt::meta_factory<{full_name}>{{}}",
                f'            .type(entt::type_id<{full_name}>().hash(), "{component_data.name}")',
                f'            .custom<ls::reflection_system::ComponentInfo>("{component_info.display_name}", {bool_to_string(component_info.is_component)})',
            ]

            if component_info.is_component:
                component_factory_code.append(
                    f'            .func<&ls::ecs::Registry::ensureSparseSet<{full_name}>>("ensureSparseSet"_hs)',
                )

            for component_property in component_data.properties:
                property_info = PropertyInfo()

                property_info.display_name = component_property.get_attribute_value(
                    "DisplayName", default=component_property.name
                )

                if component_property.has_attribute("ReadOnly"):
                    property_info.read_only = string_to_bool(
                        component_property.get_attribute_value("ReadOnly")
                    )

                if component_property.has_attribute("Transient"):
                    property_info.transient = string_to_bool(
                        component_property.get_attribute_value("Transient")
                    )

                if component_property.has_attribute("Color"):
                    property_info.color = string_to_bool(
                        component_property.get_attribute_value("Color")
                    )

                if component_property.has_attribute("Step"):
                    property_info.step = parse_float_literal(
                        component_property.get_attribute_value("Step")
                    )

                if component_property.has_attribute("Range"):
                    property_info.range = True
                    range_value = component_property.get_attribute_value("Range")
                    parts = [p.strip() for p in range_value.split(",")]
                    if len(parts) == 2:
                        property_info.range_min = parse_float_literal(parts[0])
                        property_info.range_max = parse_float_literal(parts[1])

                component_factory_code.append(
                    f'            .data<&{full_name}::{component_property.name}>("{component_property.name}"_hs, "{component_property.name}")'
                )

                component_factory_code.append(
                    f'            .custom<ls::reflection_system::PropertyInfo>("{property_info.display_name}", {bool_to_string(property_info.read_only)}, {bool_to_string(property_info.transient)}, {bool_to_string(property_info.color)}, {property_info.step}f, {bool_to_string(property_info.range)}, {property_info.range_min}f, {property_info.range_max}f)'
                )

            if component_factory_code:
                component_factory_code[-1] += ";"
                component_factories_code.append("\n".join(component_factory_code))

        for enum_data in header_data.enums:
            enum_info = EnumInfo()
            full_name = get_full_name(enum_data.namespace, enum_data.name)

            enum_info.display_name = enum_data.get_attribute_value(
                "DisplayName", default=enum_data.name
            )

            enum_factory_code: list[str] = [
                f"        entt::meta_factory<{full_name}>{{}}",
                f'            .type(entt::type_id<{full_name}>().hash(), "{enum_data.name}")',
                f'            .custom<ls::reflection_system::EnumInfo>("{enum_info.display_name}")',
            ]
            for enum_value_data in enum_data.values:
                enum_factory_code.append(
                    f'            .data<{full_name}::{enum_value_data.name}>("{enum_value_data.name}"_hs, "{enum_value_data.name}")'
                )

            if enum_factory_code:
                enum_factory_code[-1] += ";"
                enum_factories_code.append("\n".join(enum_factory_code))

    unique_includes = list(dict.fromkeys(includes))

    return f"""// Auto-generated by reflection generator, but reflection generator was written by .jan K.
#include "ls/reflection/reflection_system.hpp"
#include "ls/ecs/registry.hpp"
#include <entt/meta/factory.hpp>

{"\n".join(unique_includes)}

namespace ls::reflection_system {{

    void registerGeneratedTypes() {{
        using namespace entt::literals;

{"\n\n".join(component_factories_code)}

{"\n\n".join(enum_factories_code)}

    }}

}} // namespace ls::reflection_system
"""
