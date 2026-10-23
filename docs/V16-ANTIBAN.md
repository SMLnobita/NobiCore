# v16 — port anti-ban từ AWSS3 donor

Ngày 23/10/2026. Phạm vi: chặn báo cáo anti-cheat trong SMLnobita, không thay đổi menu/Trừng trị.

## Đã port

- Stub 7 lớp: `PluginTssSDKLifecycle`, `TssSDKLifecycle`, `TssReportManager`, `AnoSDKCore`, `AnoSDKReport`, `AntiCheatReporter`, `AntiCheatLifecycle`; thêm quét tên lớp chứa `anosdk`/`anticheat`/`tssdk` hoặc bắt đầu bằng `Tss`.
- 6 hook site: `NSURLSession` `dataTaskWithRequest:`, `dataTaskWithRequest:completionHandler:`, `dataTaskWithURL:completionHandler:`, `uploadTaskWithRequest:fromData:completionHandler:`; `NSURLConnection sendSynchronousRequest:returningResponse:error:`; `NSFileManager contentsOfDirectoryAtPath:error:`.
- 23 fragment URL: `anticheatexpert`, `anticheat`, `anogs`, `anogreport`, `anosdk`, `tdatamaster`, `tdm.qq`, `mdt.qq`, `mna.qq`, `beacon.qq`, `beacon-tj.qq`, `snowflake.qq`, `bugly`, `dd.garena.com`, `itop.kg.garena`, `bugly-aos.cros.garena`, `kg.midas.garena`, `oth.eve.mdt`, `tdm_report`, `corereport`, `scan_result`, `heartbeat_check`, `hbcheck`.
- 12 fragment body: `tdm_report`, `COREREPORT`, `HBCheck`, `black_module_macho`, `cheat_open_id`, `shell_report`, `ms_scan_start`, `ms_curr_mod`, `ts2_mod`, `ScanCast`, `AnogsReport`, `AnoSDK`; kèm prefix `ano_`/`tss_`, cặp byte `E5 AC` hoặc `AC E5`, quét 1024 byte đầu.
- Request bị chặn được đưa tới `http://127.0.0.1:1/void`; callback nhận data rỗng + HTTP 200 giả + error nil; `sendSynchronousRequest` trả data rỗng + response 200 giả + error nil; `netdata.bin` bị loại khỏi directory listing.

## Không port (cố ý)

Các initializer còn lại của AWSS3 là SDK/license/self-protection của donor, không phải suppression: `ptrace(PT_DENY_ATTACH)` qua dlsym, `task_set_exception_ports`, `sysctl` P_TRACED, xoá keychain + `apiclient_set_token`/`apiclient_paid`, timer 30 giây, observer `UIApplicationDidFinishLaunching`, NSLog `[ANHVU]`/`[AOVCHEAT]`, self-CRC `__text`, dữ liệu AVSpeechSynthesizer. Bộ đếm request bị chặn cũng bỏ qua.

## Xác minh

- Build ARM64 iOS PASS; 7/7 test host PASS (`test_menu_layout`, `test_khtn`, `test_frame`, `test_selected`, `test_map`, `test_logic`, `test_imgui_bridge`).
- Module `7df87652f3aba8e2728fd388f8c5f013410f361c9c2206a9c2408d9504e6c895`, 211 trang chữ ký ad-hoc; IPA `4db6755ef8bb2e84157070605bf2aa84bf9127ba3091b634891a94824eb66c06`.
- Người dùng đã ký, cài, chơi một ván; không báo crash/treo. Chưa test shop/nạp; chưa có checklist riêng từng chức năng menu/Trừng trị.
- Traffic capture một phần (hotspot + Wireshark): không thấy các domain anti-cheat đặc trưng; còn thấy `itop.kg.garena.vn`, `sg.tdatamaster.com`, `mdt.qq`, `mna.qq` qua đường native/port riêng ngoài hook. Không chạy donor A/B; capture không chứng minh tuyệt đối việc chặn phía server.

## Tái tạo

Từ `D:\rea\lienquan\project`:

```powershell
.\build-khtn.ps1 -InstallName "@rpath/SMLnobita.framework/SMLnobita"
python build\prepare_module_khtn.py
python build\package_menu_v16.py
```

Cần giữ `project/KHTN-SMLnobita-v15.ipa` đúng SHA256 `fc8b9f91e25b82bbb3abd5e7ddb5d18ac49b0c2ae227586dc620ec1d55ac0a54`; script kiểm tra hash và chỉ thay entry SMLnobita. Toolchain/SDK và IPA nền không nằm trong source snapshot.
