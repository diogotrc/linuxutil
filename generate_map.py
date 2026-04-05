import json

with open('strings.txt', 'r', encoding='utf-8') as f:
    strings = [list.strip() for list in f.readlines() if list.strip()]

import urllib.request

# Since I know the exact translations directly, I will just output them here within the script.
# Actually I can read the existing translator.h to find where to inject it.
