import re
from pathlib import Path

from code_info import ComponentCodeInfo, EnumCodeInfo
from macro_data import (
    AttributeData,
    ComponentData,
    EnumData,
    EnumValueData,
    PropertyData,
)


def resolve_include_path(header_path: Path, input_dir: Path) -> str:
    abs_header = header_path.resolve()
    abs_input_dir = input_dir.resolve()

    try:
        relative_path = abs_header.relative_to(abs_input_dir)
        return relative_path.as_posix()
    except ValueError:
        return abs_header.name


def remove_comments(code: str) -> str:
    code = re.sub(r"/\*.*?\*/", "", code, flags=re.DOTALL)
    code = re.sub(r"//.*", "", code)
    return code


def find_closing_brace_index(code: str, begin: int) -> int:
    depth = 1
    for i in range(begin, len(code)):
        if code[i] == "{":
            depth += 1
        elif code[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    return -1


namespace_pattern = re.compile(r"namespace\s+([\w:]+)?\s*\{")


def get_namespace(code: str, end: int) -> str:
    code = code[:end]

    active_namespaces: list[tuple[int, str]] = []
    depth = 0
    i = 0

    namespace_matches = {
        m.start(): (m.end(), m.group(1)) for m in namespace_pattern.finditer(code)
    }

    while i < len(code):
        if i in namespace_matches:
            end_position, namespace_name = namespace_matches[i]
            if namespace_name:
                active_namespaces.append((depth, namespace_name))
            depth += 1
            i = end_position
            continue

        char = code[i]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            active_namespaces = [
                namespace for namespace in active_namespaces if namespace[0] < depth
            ]
        i += 1

    return "::".join(namespace[1] for namespace in active_namespaces)


attribute_content_pattern = r'(?:[^()"]|"[^"]*"|\((?:[^()"]|"[^"]*")*\))*'

component_pattern = re.compile(
    r"(?i:(?P<Macro>COMPONENT|STRUCT))\s*\(\s*(?P<Attributes>"
    + attribute_content_pattern
    + r")\s*\)\s*struct\s*(?P<Name>\w+)\s*\{"
)

enum_pattern = re.compile(
    r"(?i:ENUM)\s*\(\s*(?P<Attributes>"
    + attribute_content_pattern
    + r")\s*\)\s*enum\s+(?:class\s+)?(?P<Name>\w+)\s*(?::\s*[\w:]+)?\s*\{"
)

property_pattern = re.compile(
    r"(?i:PROPERTY)\s*\(\s*(?P<Attributes>" + attribute_content_pattern + r")\s*\)\s*"
    r"(?P<Type>[\w:]+(?:<.*?>)?)\s+"
    r"(?P<Name>\w+)"
    r"(?:\s*(?:=\s*)?\{?\s*(?P<Value>[^};]+)\s*\}?)?;"
)


def split_attributes(code: str) -> list[str]:
    tokens: list[str] = []
    current: list[str] = []
    depth = 0
    in_quotes = False
    escaped = False

    for char in code:
        if char == '"' and not escaped:
            in_quotes = not in_quotes
            current.append(char)
        elif char == "(" and not in_quotes:
            depth += 1
            current.append(char)
        elif char == ")" and not in_quotes:
            depth -= 1
            current.append(char)
        elif char == "," and depth == 0 and not in_quotes:
            tokens.append("".join(current).strip())
            current = []
        else:
            current.append(char)

        escaped = char == "\\" and not escaped

    if current:
        tokens.append("".join(current).strip())

    return [token for token in tokens if token]


def parse_attributes(code: str) -> list[AttributeData]:
    attributes: list[AttributeData] = []
    tokens = split_attributes(code)

    for token in tokens:
        token = token.strip()
        if not token:
            continue

        if "(" in token and token.endswith(")"):
            name, val = token.split("(", 1)
            value = val[:-1]
        elif "=" in token:
            name, val = token.split("=", 1)
            value = val
        else:
            name = token
            value = "true"

        clean_name = name.strip()
        clean_value = value.strip().strip('"').strip("'")

        attributes.append(AttributeData(name=clean_name, value=clean_value))

    return attributes


def parse_properties(code: str) -> list[PropertyData]:
    properties: list[PropertyData] = []

    for property_match in re.finditer(property_pattern, code):
        property_attributes_code = property_match.group("Attributes")
        property_attributes = parse_attributes(property_attributes_code)

        property_type = property_match.group("Type")
        property_name = property_match.group("Name")

        properties.append(
            PropertyData(
                name=property_name,
                type=property_type,
                attributes=property_attributes,
            )
        )

    return properties


def parse_enum_values(body_code: str) -> list[EnumValueData]:
    values: list[EnumValueData] = []

    for item in body_code.split(","):
        item = item.strip()
        if not item:
            continue

        match = re.match(r"^(?P<Name>[a-zA-Z_]\w*)", item)

        if match:
            values.append(EnumValueData(name=match.group("Name")))

    return values


def process_header(code: str) -> tuple[list[ComponentData], list[EnumData]]:
    code = remove_comments(code)

    components_code_info: list[ComponentCodeInfo] = []

    for enum_match in re.finditer(component_pattern, code):
        macro_type = enum_match.group("Macro").upper()
        is_component = macro_type == "COMPONENT"
        name = enum_match.group("Name")
        attributes = enum_match.group("Attributes")
        begin_body = enum_match.end()
        end = find_closing_brace_index(code, begin_body)

        if end == -1:
            continue

        components_code_info.append(
            ComponentCodeInfo(
                is_component=is_component,
                name=name,
                attributes_code=attributes,
                begin=enum_match.start(),
                begin_body=begin_body,
                end=end,
            )
        )

    for component_code_info in components_code_info:
        component_code_info.namespace = get_namespace(code, component_code_info.begin)
        parents = [
            parent
            for parent in components_code_info
            if parent.begin < component_code_info.begin
            and component_code_info.end <= parent.end
        ]
        parents.sort(key=lambda parent: parent.begin)
        component_code_info.parent_names = [parent.name for parent in parents]

    components: list[ComponentData] = []

    for component_code_info in components_code_info:
        namespace_parts = []
        if component_code_info.namespace:
            namespace_parts.append(component_code_info.namespace)
        namespace_parts.extend(component_code_info.parent_names)

        full_namespace = "::".join(namespace_parts)

        body_code = list(code[component_code_info.begin_body : component_code_info.end])

        for inner_component_code_info in components_code_info:
            if (
                inner_component_code_info.begin >= component_code_info.begin_body
                and inner_component_code_info.end <= component_code_info.end
                and inner_component_code_info != component_code_info
            ):
                relative_begin = (
                    inner_component_code_info.begin - component_code_info.begin_body
                )
                relative_end = (
                    inner_component_code_info.end - component_code_info.begin_body + 1
                )

                for i in range(
                    max(0, relative_begin), min(len(body_code), relative_end)
                ):
                    body_code[i] = " "

        clean_body_code = "".join(body_code)

        component_attributes = parse_attributes(component_code_info.attributes_code)
        component_properties = parse_properties(clean_body_code)

        components.append(
            ComponentData(
                is_component=component_code_info.is_component,
                name=component_code_info.name,
                namespace=full_namespace,
                attributes=component_attributes,
                properties=component_properties,
            )
        )

    enums_code_info: list[EnumCodeInfo] = []

    for enum_match in re.finditer(enum_pattern, code):
        name = enum_match.group("Name")
        attributes = enum_match.group("Attributes")
        begin_body = enum_match.end()
        end = find_closing_brace_index(code, begin_body)

        if end == -1:
            continue

        enums_code_info.append(
            EnumCodeInfo(
                name=name,
                attributes_code=attributes,
                begin=enum_match.start(),
                begin_body=begin_body,
                end=end,
            )
        )

    enums: list[EnumData] = []
    for enum_code_info in enums_code_info:
        enum_namespace = get_namespace(code, enum_code_info.begin)

        parents = [
            parent
            for parent in components_code_info
            if parent.begin < enum_code_info.begin and enum_code_info.end <= parent.end
        ]
        parents.sort(key=lambda parent: parent.begin)
        parent_names = [parent.name for parent in parents]

        namespace_parts = []
        if enum_namespace:
            namespace_parts.append(enum_namespace)
        namespace_parts.extend(parent_names)

        full_namespace = "::".join(namespace_parts)

        body_code = code[enum_code_info.begin_body : enum_code_info.end]
        enum_attributes = parse_attributes(enum_code_info.attributes_code)
        enum_values = parse_enum_values(body_code)

        enums.append(
            EnumData(
                name=enum_code_info.name,
                namespace=full_namespace,
                attributes=enum_attributes,
                values=enum_values,
            )
        )

    return components, enums
