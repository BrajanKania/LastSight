from dataclasses import dataclass, field


@dataclass
class AttributeData:
    name: str = ""
    value: str = ""


@dataclass
class PropertyData:
    name: str = ""
    type: str = ""
    attributes: list[AttributeData] = field(default_factory=list)

    def has_attribute(self, attribute_name: str) -> bool:
        return any(attribute.name == attribute_name for attribute in self.attributes)

    def get_attribute_value(self, attribute_name: str, default: str = "") -> str:
        return next(
            (
                attribute.value
                for attribute in self.attributes
                if attribute.name == attribute_name
            ),
            default,
        )


@dataclass
class ComponentData:
    is_component: bool = True
    name: str = ""
    namespace: str = ""
    attributes: list[AttributeData] = field(default_factory=list)
    properties: list[PropertyData] = field(default_factory=list)

    def has_attribute(self, attribute_name: str) -> bool:
        return any(attribute.name == attribute_name for attribute in self.attributes)

    def get_attribute_value(self, attribute_name: str, default: str = "") -> str:
        return next(
            (
                attribute.value
                for attribute in self.attributes
                if attribute.name == attribute_name
            ),
            default,
        )


@dataclass
class EnumValueData:
    name: str = ""


@dataclass
class EnumData:
    name: str = ""
    namespace: str = ""
    attributes: list[AttributeData] = field(default_factory=list)
    values: list[EnumValueData] = field(default_factory=list)

    def has_attribute(self, attribute_name: str) -> bool:
        return any(attribute.name == attribute_name for attribute in self.attributes)

    def get_attribute_value(self, attribute_name: str, default: str = "") -> str:
        return next(
            (
                attribute.value
                for attribute in self.attributes
                if attribute.name == attribute_name
            ),
            default,
        )


@dataclass
class HeaderData:
    path: str = ""
    components: list[ComponentData] = field(default_factory=list)
    enums: list[EnumData] = field(default_factory=list)
