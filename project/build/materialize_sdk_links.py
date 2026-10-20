from pathlib import Path
import subprocess,json
r=Path(__file__).resolve().parent/'apple-sdks';sdk=(r/'iPhoneOS16.5.sdk').resolve()
names=json.loads((r/'sdk-link-paths.json').read_text(encoding='utf-8'))
links=[r/name for name in names if not (r/name).read_bytes().startswith(b'---')]
original={p:p.read_bytes() for p in links}
def contents(p,seen=None):
 if seen is None:seen=set()
 if p in seen:raise ValueError('SDK link cycle')
 if p not in original:return p.read_bytes()
 seen.add(p);target=(p.parent/original[p].decode().strip()).resolve();assert target.is_relative_to(sdk)
 return contents(target,seen)
for p in links:p.write_bytes(contents(p))
print('Materialized SDK tbd file links:',len(links))
