from dataclasses import dataclass, field


@dataclass
class ComponentCodeInfo:
    is_component: bool = True
    name: str = ""
    attributes_code: str = ""
    begin: int = 0
    begin_body: int = 0
    end: int = 0
    namespace: str = ""
    parent_names: list[str] = field(default_factory=list)


@dataclass
class EnumCodeInfo:
    name: str = ""
    attributes_code: str = ""
    begin: int = 0
    begin_body: int = 0
    end: int = 0
    namespace: str = ""
