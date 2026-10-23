"""Package the v16 candidate: v15 base with the full anti-ban port in TssGuard."""
from pathlib import Path
import copy
import hashlib
import json
import struct
import zipfile

ROOT = Path(__file__).resolve().parent.parent
BASE = ROOT / 'KHTN-SMLnobita-v15.ipa'
MODULE = ROOT / 'build/Clone6-khtn'
OUTPUT = ROOT / 'KHTN-SMLnobita-v16.ipa'
TEMP = OUTPUT.with_suffix('.tmp.ipa')
ENTRY = 'Payload/kgvn.app/Frameworks/SMLnobita.framework/SMLnobita'
BASE_SHA = 'fc8b9f91e25b82bbb3abd5e7ddb5d18ac49b0c2ae227586dc620ec1d55ac0a54'


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


assert sha(BASE.read_bytes()) == BASE_SHA, 'Unexpected v15 base'
assert BASE != OUTPUT and BASE != TEMP
module = MODULE.read_bytes()
assert dylib_id(module) == '@rpath/SMLnobita.framework/SMLnobita'
replacements = {ENTRY: module}

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
    assert target.read(ENTRY) == module
    assert dylib_id(target.read(ENTRY)) == '@rpath/SMLnobita.framework/SMLnobita'

TEMP.replace(OUTPUT)
report = {
    'ipa': str(OUTPUT), 'sha256': sha(OUTPUT.read_bytes()), 'size': OUTPUT.stat().st_size,
    'base_ipa': str(BASE), 'base_sha256': BASE_SHA,
    'module_sha256': sha(module), 'module_dylib_id': dylib_id(module),
    'changed_entries': changed,
    'source_sha256': {str(path.relative_to(ROOT)): sha(path.read_bytes()) for path in
                      [ROOT / 'src/TssGuard.mm', ROOT / 'build-khtn.ps1', ROOT / 'menu/ImGuiDrawView.mm']},
    'decoded_blocklist': {
        'url': ['anticheatexpert', 'anticheat', 'anogs', 'anogreport', 'anosdk', 'tdatamaster',
                'tdm.qq', 'mdt.qq', 'mna.qq', 'beacon.qq', 'beacon-tj.qq', 'snowflake.qq',
                'bugly', 'dd.garena.com', 'itop.kg.garena', 'bugly-aos.cros.garena',
                'kg.midas.garena', 'oth.eve.mdt', 'tdm_report', 'corereport', 'scan_result',
                'heartbeat_check', 'hbcheck'],
        'body': ['tdm_report', 'COREREPORT', 'HBCheck', 'black_module_macho', 'cheat_open_id',
                 'shell_report', 'ms_scan_start', 'ms_curr_mod', 'ts2_mod', 'ScanCast',
                 'AnogsReport', 'AnoSDK'],
        'decoy_url': 'http://127.0.0.1:1/void',
        'fake_response_url_fallback': 'https://report.local/',
        'hidden_directory_entry': 'netdata.bin',
        'source': 'AWSS3 sub_2F0BC44 (URL list), sub_2F0D824 (body list), sub_2F0B570 (decoy), sub_2F0E768 (fake response), sub_2F172C4 (directory filter)',
    },
    'verification': 'ZIP CRC, unchanged entry list, only the SMLnobita module replaced; all other entries byte-identical to the v15 candidate. Host suites test_frame/test_imgui_bridge/test_khtn/test_logic/test_map/test_menu_layout/test_selected exited 0 and the ARM64 iOS module link passed before packaging.',
    'signing': 'Ad-hoc module container only (7df87652f3aba8e2728fd388f8c5f013410f361c9c2206a9c2408d9504e6c895, 211 pages). Complete certificate re-signing of the IPA/frameworks required before installation. Candidate untested on device and against the live server.',
    'notes': 'Full AWSS3 anti-ban parity: URL blocklist on lowercased absoluteString, body filter (ano_/tss_/E5AC both byte orders/12 UTF-8 fragments in first 1024 bytes), NSURLSession dataTaskWithRequest:, dataTaskWithRequest:completionHandler:, dataTaskWithURL:completionHandler:, uploadTaskWithRequest:fromData:completionHandler:, NSURLConnection sendSynchronousRequest, NSFileManager contentsOfDirectoryAtPath:error: hiding netdata.bin. Blocked callbacks receive an async fake HTTP 200 with empty data; blocked sync calls return empty data with a fake 200 response; decoy target is http://127.0.0.1:1/void.',
}
(ROOT / 'build/KHTN-SMLnobita-v16.report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False, indent=2))
