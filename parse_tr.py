import os
import re

strings = set()
pattern = re.compile(r'tr\s*\(\s*"((?:[^"\\]|\\.)*)"\s*\)')

for dirpath, _, filenames in os.walk('src'):
    for filename in filenames:
        if filename.endswith('.cpp') or filename.endswith('.h'):
            filepath = os.path.join(dirpath, filename)
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
                matches = pattern.findall(content)
                for match in matches:
                    strings.add(match)

# Clean up escaping if needed, but keeping exactly as literal is good for C++ matching
with open('strings.txt', 'w', encoding='utf-8') as f:
    for s in sorted(strings):
        f.write(f"{s}\n")
