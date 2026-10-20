"""Package the reviewed menu layout and the user's anhnew app icon assets."""
from pathlib import Path
import copy
import hashlib
import json
import struct
import zipfile

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
BASE = ROOT / 'KHTN-SMLnobita-v12.ipa'
ASSETS = ROOT.parent / 'anhnew'
MODULE = ROOT / 'build/Clone6-khtn'
OUTPUT = ROOT / 'KHTN-SMLnobita-v13.ipa'
TEMP = OUTPUT.with_suffix('.tmp.ipa')
ENTRY = 'Payload/kgvn.app/Frameworks/SMLnobita.framework/SMLnobita'
ICON_SIZES = {'AppIcon60x60@2x.png': (120, 120), 'AppIcon76x76@2x~ipad.png': (152, 152)}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def dylib_id(data):
    assert data[:4] == bytes.fromhex('cffaedfe')
    pos = 32
    for _ in range(struct.unpack_from('<I', data, 16)[0]):
        command, size = struct.unpack_from('<II', data, pos)
        if command == 0xd:
            start = pos + struct.unpack_from('<I', data, pos + 8)[0]
            return data[start:data.index(b'\0', start)].decode()
        pos += size
    return None


assert sha(BASE.read_bytes()) == 'c58e0e3c80b0e1b962c74ae3612e484da14b80fb8981b4046af9df3715efd70f', 'Unexpected v12 base'
assert BASE != OUTPUT and BASE != TEMP
module = MODULE.read_bytes()
assert dylib_id(module) == '@rpath/SMLnobita.framework/SMLnobita'
replacements = {ENTRY: module}
asset_hashes = {}
for name, dimensions in ICON_SIZES.items():
    path = ASSETS / name
    with Image.open(path) as image:
        assert image.format == 'PNG' and image.size == dimensions
        image.verify()
    data = path.read_bytes()
    replacements['Payload/kgvn.app/' + name] = data
    asset_hashes[name] = sha(data)
catalog = (ASSETS / 'Assets.car').read_bytes()
assert catalog.startswith(b'BOMStore') and b'AppIcon\0' in catalog
replacements['Payload/kgvn.app/Assets.car'] = catalog
asset_hashes['Assets.car'] = sha(catalog)

with zipfile.ZipFile(BASE) as source:
    assert set(replacements).issubset(source.namelist())
    with zipfile.ZipFile(TEMP, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=4) as target:
        for member in source.infolist():
            target.writestr(copy.copy(member), replacements.get(member.filename, source.read(member.filename)))

with zipfile.ZipFile(BASE) as source, zipfile.ZipFile(TEMP) as target:
    assert target.testzip() is None
    assert source.namelist() == target.namelist()
    changed = [name for name in source.namelist() if source.read(name) != target.read(name)]
    assert set(changed) == set(replacements), changed
    for name, expected in replacements.items():
        assert target.read(name) == expected
    assert dylib_id(target.read(ENTRY)) == '@rpath/SMLnobita.framework/SMLnobita'

TEMP.replace(OUTPUT)
report = {
    'ipa': str(OUTPUT), 'sha256': sha(OUTPUT.read_bytes()), 'size': OUTPUT.stat().st_size,
    'base_ipa': str(BASE), 'base_sha256': sha(BASE.read_bytes()),
    'module_sha256': sha(module), 'module_dylib_id': dylib_id(module),
    'asset_source_directory': str(ASSETS), 'asset_sha256': asset_hashes,
    'changed_entries': changed,
    'source_sha256': {str(path.relative_to(ROOT)): sha(path.read_bytes()) for path in
                      [ROOT / 'menu/ImGuiDrawView.mm', ROOT / 'menu/Clone6MenuContent.h', ROOT / 'menu/KHTNMenuLayout.h']},
    'verification': 'ZIP CRC, unchanged entry list, exactly one module plus the three supplied icon assets replaced; all other entries byte-identical to v12.',
    'signing': 'Ad-hoc module only. Complete certificate re-signing of the IPA/frameworks required before installation.',
}
(ROOT / 'build/KHTN-SMLnobita-v13.report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False, indent=2))
