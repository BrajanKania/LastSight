from dataclasses import dataclass


@dataclass
class ComponentInfo:
    display_name: str = ""
    is_component: bool = True


@dataclass
class PropertyInfo:
    display_name: str = ""
    read_only: bool = False
    transient: bool = False
    color: bool = False
    step: float = 0.05
    range: bool = False
    range_min: float = 0.0
    range_max: float = 1.0


@dataclass
class EnumInfo:
    display_name: str = ""
