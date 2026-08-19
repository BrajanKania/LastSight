import argparse
import re
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class PropertyData:
    type: str
    name: str
    attributes: dict[str, str] = field(default_factory=dict)


@dataclass
class StructData:
    name: str
    namespace: str = ""
    parent_prefix: str = ""
    properties: list[PropertyData] = field(default_factory=list)
    attributes: dict[str, str] = field(default_factory=dict)

    @property
    def full_name(self) -> str:
        parts = []
        if self.namespace:
            parts.append(self.namespace)
        if self.parent_prefix:
            parts.append(self.parent_prefix)
        parts.append(self.name)
        return "::".join(parts)


@dataclass
class EnumMemberData:
    name: str


@dataclass
class EnumData:
    name: str
    namespace: str = ""
    parent_prefix: str = ""
    members: list[EnumMemberData] = field(default_factory=list)
    attributes: dict[str, str] = field(default_factory=dict)

    @property
    def full_name(self) -> str:
        parts = []
        if self.namespace:
            parts.append(self.namespace)
        if self.parent_prefix:
            parts.append(self.parent_prefix)
        parts.append(self.name)
        return "::".join(parts)


@dataclass
class HeaderInfo:
    include_path: str
    structs: list[StructData]
    enums: list[EnumData] = field(default_factory=list)


def remove_comments(code: str) -> str:
    code = re.sub(r"/\*.*?\*/", "", code, flags=re.DOTALL)
    code = re.sub(r"//.*", "", code)
    return code


def parse_attributes(attr_str: str) -> dict[str, str]:
    attributes = {}
    if not attr_str or not attr_str.strip():
        return attributes

    tokens = re.findall(r'(?:[^,"]|"(?:\\.|[^"])*")+', attr_str)
    for token in tokens:
        token = token.strip()
        if not token:
            continue

        if "=" in token:
            key, val = token.split("=", 1)
            attributes[key.strip()] = val.strip().strip("\"'")
        else:
            attributes[token] = "true"

    return attributes


def find_enclosing_namespace(code: str, pos: int) -> str:
    prefix = code[:pos]
    matches = re.findall(r"namespace\s+([\w:]+)\s*\{", prefix)
    return matches[-1] if matches else ""


def strip_nested_definitions(body: str) -> str:
    pattern = re.compile(
        r"(?:\bSTRUCT\s*\([^)]*\)\s*)?\b(?:struct|class|enum)\s+\w+\s*\{|"
        r"\b\w[\w\s:*&<>]*\s+\w+\s*\([^)]*\)\s*\{"
    )

    while True:
        m = pattern.search(body)
        if not m:
            break
        start_brace = m.end() - 1
        brace_count = 0
        end_brace = -1
        for i in range(start_brace, len(body)):
            if body[i] == "{":
                brace_count += 1
            elif body[i] == "}":
                brace_count -= 1
                if brace_count == 0:
                    end_brace = i
                    break
        if end_brace != -1:
            after = end_brace + 1
            while after < len(body) and body[after].isspace():
                after += 1
            if after < len(body) and body[after] == ";":
                after += 1
            body = body[: m.start()] + " " + body[after:]
        else:
            break
    return body


var_pattern = re.compile(
    r"^\s*(?:PROPERTY\((?P<attr>.*?)\)\s*)?"
    r"(?P<type>[\w+::<>,\s*&]+?)\s+"
    r"(?P<name>\w+)"
    r"\s*(?:=\{[^}]*\}|=\s*[^;]+|\{[^}]*\}|\([^)]*\))?\s*$"
)


def process_code(
    code: str, namespace: str = "", parent_prefix: str = ""
) -> list[StructData]:
    clean_code = remove_comments(code) if not parent_prefix else code
    found_structs: list[StructData] = []

    struct_pattern = re.compile(
        r"STRUCT\((?P<attr>.*?)\)\s*(?:struct|class)\s+(?P<name>\w+)\s*\{"
    )

    pos = 0
    while pos < len(clean_code):
        match = struct_pattern.search(clean_code, pos)
        if not match:
            break

        struct_attr_raw = match.group("attr")
        struct_name = match.group("name")
        start_pos = match.end() - 1

        brace_count = 0
        end_pos = -1
        for i in range(start_pos, len(clean_code)):
            if clean_code[i] == "{":
                brace_count += 1
            elif clean_code[i] == "}":
                brace_count -= 1
                if brace_count == 0:
                    end_pos = i
                    break

        if end_pos == -1:
            pos = match.end()
            continue

        raw_body = clean_code[start_pos + 1 : end_pos]
        curr_ns = (
            namespace
            if parent_prefix
            else find_enclosing_namespace(clean_code, match.start())
        )

        current_struct = StructData(
            name=struct_name,
            namespace=curr_ns,
            parent_prefix=parent_prefix,
            attributes=parse_attributes(struct_attr_raw),
        )

        next_parent_prefix = (
            f"{parent_prefix}::{struct_name}" if parent_prefix else struct_name
        )

        nested_structs = process_code(raw_body, curr_ns, next_parent_prefix)
        found_structs.extend(nested_structs)

        body_for_props = strip_nested_definitions(raw_body)

        for stmt in body_for_props.split(";"):
            stmt_clean = stmt.strip()
            if not stmt_clean:
                continue

            if any(
                stmt_clean.startswith(kw)
                for kw in ["using ", "typedef ", "friend ", "static_assert"]
            ):
                continue

            v_match = var_pattern.match(stmt_clean)
            if v_match:
                p_attr_raw = v_match.group("attr") or ""
                p_type = v_match.group("type").strip()
                p_name = v_match.group("name").strip()

                current_struct.properties.append(
                    PropertyData(
                        type=p_type,
                        name=p_name,
                        attributes=parse_attributes(p_attr_raw),
                    )
                )

        found_structs.append(current_struct)
        pos = end_pos + 1

    return found_structs


def parse_enum_body(body: str) -> list[EnumMemberData]:
    members = []
    clean_body = remove_comments(body)
    for item in clean_body.split(","):
        item_clean = item.strip()
        if not item_clean:
            continue
        name_part = item_clean.split("=")[0].strip()
        m = re.search(r"\b([a-zA-Z_]\w*)\b", name_part)
        if m:
            members.append(EnumMemberData(name=m.group(1)))
    return members


def process_enums(
    code: str, namespace: str = "", parent_prefix: str = ""
) -> list[EnumData]:
    clean_code = remove_comments(code) if not parent_prefix else code
    found_enums: list[EnumData] = []

    combined_pattern = re.compile(
        r"(?:(?P<is_struct>STRUCT\((?P<struct_attr>.*?)\)\s*(?:struct|class)\s+(?P<struct_name>\w+))|"
        r"(?P<is_enum>ENUM\((?P<enum_attr>.*?)\)\s*enum\s+(?:class\s+)?(?P<enum_name>\w+)\s*(?::\s*[\w:]+\s*)?))\s*\{"
    )

    pos = 0
    while pos < len(clean_code):
        match = combined_pattern.search(clean_code, pos)
        if not match:
            break

        start_pos = match.end() - 1

        brace_count = 0
        end_pos = -1
        for i in range(start_pos, len(clean_code)):
            if clean_code[i] == "{":
                brace_count += 1
            elif clean_code[i] == "}":
                brace_count -= 1
                if brace_count == 0:
                    end_pos = i
                    break

        if end_pos == -1:
            pos = match.end()
            continue

        raw_body = clean_code[start_pos + 1 : end_pos]
        curr_ns = (
            namespace
            if parent_prefix
            else find_enclosing_namespace(clean_code, match.start())
        )

        if match.group("is_struct"):
            struct_name = match.group("struct_name")
            next_parent_prefix = (
                f"{parent_prefix}::{struct_name}" if parent_prefix else struct_name
            )
            nested_enums = process_enums(raw_body, curr_ns, next_parent_prefix)
            found_enums.extend(nested_enums)
        elif match.group("is_enum"):
            enum_name = match.group("enum_name")
            enum_attr_raw = match.group("enum_attr")
            current_enum = EnumData(
                name=enum_name,
                namespace=curr_ns,
                parent_prefix=parent_prefix,
                attributes=parse_attributes(enum_attr_raw),
                members=parse_enum_body(raw_body),
            )
            found_enums.append(current_enum)

        pos = end_pos + 1

    return found_enums


def resolve_include_path(header_path: Path, input_dirs: list[str]) -> str:
    path_str = header_path.as_posix()
    for root in ["src/", "include/", "engine/"]:
        if root in path_str:
            idx = path_str.find(root)
            if root in ["src/", "include/"]:
                return path_str[idx + len(root) :]
            return path_str[idx:]

    for input_dir in input_dirs:
        try:
            rel = header_path.relative_to(Path(input_dir).resolve())
            return rel.as_posix()
        except ValueError:
            pass

    return header_path.name


def generate_cpp_code(headers_info: list[HeaderInfo]) -> str:
    includes: list[str] = []
    blocks: list[str] = []

    for info in headers_info:
        if not info.structs and not info.enums:
            continue

        includes.append(f'#include "{info.include_path}"')

        for s in info.structs:
            lines = [
                f"    entt::meta_factory<{s.full_name}>{{}}",
                f'        .type(entt::type_id<{s.full_name}>().hash(), "{s.name}")',
                f'        .func<&ls::ecs::Registry::ensureSparseSet<{s.full_name}>>("ensureSparseSet"_hs)',
            ]

            for p in s.properties:
                lines.append(
                    f'        .data<&{s.full_name}::{p.name}>("{p.name}"_hs, "{p.name}")'
                )

                display_name = p.attributes.get("Name", p.name)
                is_readonly = (
                    "true"
                    if (
                        "ReadOnly" in p.attributes
                        and p.attributes["ReadOnly"].lower() in ("true", "1")
                    )
                    else "false"
                )

                is_transient = "true" if "Transient" in p.attributes else "false"

                lines.append(
                    f'        .custom<ls::reflection_system::PropertyInfo>("{display_name}", {is_readonly}, {is_transient})'
                )

            if len(lines) > 1:
                lines[-1] += ";"
            blocks.append("\n".join(lines))

        for e in info.enums:
            lines = [
                f"    entt::meta_factory<{e.full_name}>{{}}",
                f'        .type(entt::type_id<{e.full_name}>().hash(), "{e.name}")',
            ]
            for m in e.members:
                lines.append(
                    f'        .data<{e.full_name}::{m.name}>("{m.name}"_hs, "{m.name}")'
                )
            if len(lines) > 1:
                lines[-1] += ";"
            blocks.append("\n".join(lines))

    unique_includes = list(dict.fromkeys(includes))

    cpp_content = f"""// AUTOMATICALLY GENERATED FILE - DO NOT EDIT MANUALLY
#include "engine/reflection/reflection_system.hpp"
#include "engine/ecs/registry.hpp"
#include <entt/meta/factory.hpp>

{"\n".join(unique_includes)}

namespace ls::reflection_system {{

  void registerGeneratedTypes() {{
    using namespace entt::literals;

{"\n\n".join(blocks)}
  }}

}} // namespace ls::reflection_system
"""
    return cpp_content


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input-dir", nargs="+", required=True)
    parser.add_argument("--output-file", required=True)

    args = parser.parse_args()
    headers_info: list[HeaderInfo] = []

    for dir_path_str in args.input_dir:
        dir_path = Path(dir_path_str)
        if not dir_path.exists():
            continue

        for header_path in dir_path.rglob("*.hpp"):
            code = header_path.read_text(encoding="utf-8")
            structs = process_code(code)
            enums = process_enums(code)

            if structs or enums:
                inc_path = resolve_include_path(header_path.resolve(), args.input_dir)
                headers_info.append(
                    HeaderInfo(include_path=inc_path, structs=structs, enums=enums)
                )

    cpp_content = generate_cpp_code(headers_info)

    output_path = Path(args.output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(cpp_content, encoding="utf-8")


if __name__ == "__main__":
    main()
