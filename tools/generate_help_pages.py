"""Regenerate Shroudfire's HUD guides from the project's canonical page content."""

import json
import re
from pathlib import Path


root = Path(__file__).resolve().parents[1]
plugin = root / 'src' / 'plugin.cpp'
pages_by_name = json.loads((root / 'guides.json').read_bytes().decode('utf-8'))
source = plugin.read_bytes()

for name, pages in pages_by_name.items():
    assert re.fullmatch(r'k\w+Pages', name) and pages, name
    rendered_pages = []
    for page in pages:
        lines = page.split('<br>')
        assert 1 < len(lines) <= 6, (name, lines)
        rendered_pages.append('\n'.join(
            '    ' + json.dumps(line + ('<br>' if index < len(lines) - 1 else ''), ensure_ascii=False)
            for index, line in enumerate(lines)))
    generated = (f'constexpr std::array<const char *, {len(pages)}> {name} {{{{\n'
                 + ',\n\n'.join(rendered_pages) + '\n}};').encode('utf-8')
    pattern = rb'constexpr std::array<const char \*, \d+> ' + name.encode('ascii') + rb' \{\{.*?\n\}\};'
    source, count = re.subn(pattern, lambda _: generated, source, flags=re.S)
    assert count == 1, name
    print(f'{name}: {len(pages)} pages, at most five body lines')

plugin.write_bytes(source)
