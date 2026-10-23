# Tạo lại đúng bản cuối

## Quy trình đã thực sự kiểm tra sau khi chuyển thư mục

Mở PowerShell trong `D:\rea\lienquan\project` rồi chạy:

```powershell
python build\fix_ui_on_signable_ipa.py
python build\rename_framework_smlnobita_pathfix.py
Get-FileHash -LiteralPath KHTN-SMLnobita-pathfix.ipa -Algorithm SHA256
```

SHA256 phải là:

```text
2fd9067574ead4a7805e0571a82f19563dc05262d8f26352d1f1b6ba3be4ccaf
```

Đã chạy lại hai bước này trong thư mục mới và so sánh byte qua SHA256 với IPA chốt. Kết quả giống hệt. Có thể dùng `TAO-LAI.ps1` ở thư mục gốc để chạy và kiểm tra tự động.

## Các đầu vào không được xóa

- `project/KHTN-menu-no-guide.ipa`: bản nền người dùng xác nhận ký được.
- `project/KHTN-source-no-guide.zip`: source snapshot tương ứng, cần cho script đóng sourceZIP.
- `project/build/fix_ui_on_signable_ipa.py`.
- `project/build/rename_framework_smlnobita_pathfix.py`.

Bước 1 sửa giao diện ngay trên module có bố cục đã ký được: màu tab, nhãn Mod skin; giữ nguyên kích thước module và bố cục load command/chữ ký, cập nhật hash trang. Script có kiểm tra hash bản nền, không áp dụng tùy tiện sang phiên bản khác.

Bước 2 đổi thư mục/file framework thành SMLnobita, cập nhật plist, install name của module và **cả hai đường nạp trong kgvn lẫn UnityFramework**. Định danh framework cuối là `local.smlnobita.framework`. Script kiểm tra các hash chữ ký nội bộ. Đây vẫn là bước chuẩn bị gói; cần ký bằng chứng chỉ thật trước khi cài.

Các file trung gian có thể được sinh lại: `KHTN-no-guide-ui-fixed.ipa`, các manifest và sourceZIP ở `project`. IPA được tạo ở `project/KHTN-SMLnobita-pathfix.ipa`; bản đã chốt sẵn nằm ở `release`.

## Nếu muốn biên dịch để chỉnh source tiếp

Source hiện hành ở `project/src` và `project/menu`. Compiler Zig 0.13 và SDK iPhoneOS16.5 được giữ trong `project/build`, vì vậy không cần tải lại để biên dịch trên máy này.

```powershell
Set-Location D:\rea\lienquan\project
.\build-khtn.ps1 -InstallName "@rpath/SMLnobita.framework/SMLnobita"
python build\prepare_module_khtn.py
```

Bản source hiện tại (v13) dùng thêm bước đóng gói module và icon trên IPA v12 đã lưu:

```powershell
python build\package_menu_v13.py
```

Giữ `project/KHTN-SMLnobita-v12.ipa` và ba file trong `anhnew` (AppIcon60x60@2x.png, AppIcon76x76@2x~ipad.png, Assets.car) để đóng v13. Hai bước đầu biên dịch/chuẩn bị module, **không tự thay thế quy trình tạo đúng IPA chốt ở phần trên**. Quy trình chốt dùng seed và patch để tái tạo đúng byte. Module biên dịch mới có thể có layout khác, phải đóng gói và kiểm tra lại riêng; không áp dụng các offset patch UI cố định vào module mới.

Các script đóng gói cũ nằm trong archive lịch sử, một số phụ thuộc IPA thử đã dọn và đường dẫn cũ. Không coi chúng là pipeline hiện hành. Có giữ các snapshot cũ để tra cứu.

## Ký và kiểm tra sau khi cài

Ký toàn bộ IPA và framework bằng chứng chỉ của người dùng. Không có chứng chỉ/P12/mật khẩu được bổ sung vào bộ lưu này. Sau cài kiểm tra tên app KHTN, mở/đóng bằng 3 ngón 2 lần, ba tab, chạm xuyên sau khi đóng, và các chức năng cần dùng trong trận.

Nếu ký báo 45, kiểm tra tất cả Mach-O trong Payload, không chỉ kgvn. Mọi tham chiếu `@executable_path/Frameworks/SMLnobita.framework/SMLnobita` phải trỏ tới file thật. Lỗi 45 không được coi là lúc nào cũng cùng nguyên nhân; lần này đã xác định một đường nạp Clone6 còn sót trong UnityFramework và người dùng xác nhận bản sửa sau đó ổn.


## Bổ sung ngày 20/10/2026 — root thực tế và nhật ký tiếp nối

Đường dẫn làm việc thực tế của bộ dự án hiện tại là `D:\rea\lienquan\lienquan\project`. Khi chạy các lệnh ở phần trên, dùng root thực tế này nếu ví dụ lịch sử còn ghi `D:\rea\lienquan\project`.

Pipeline mới nhất vẫn là build-khtn với install name SMLnobita → prepare_module_khtn.py → package_menu_v13.py. Cần giữ IPA v12 và đủ ba file anhnew. Xem `docs/V13-MENU-ICON.md` và `10_20_2026v2.md`; không ghi đè IPA v11/v12 bằng module hiện tại để tái tạo lịch sử.


## Bổ sung ngày 20/10/2026 — v13 được nâng lên release

Theo yêu cầu người dùng, **bản release hiện hành là `release/KHTN-SMLnobita-v13.ipa`**. Các đoạn phía trên gọi pathfix là bản chốt hoặc v13 là nhánh nâng cấp được giữ làm lịch sử; mốc này cập nhật trạng thái phát hành hiện hành.

V13 gồm menu tự xuống dòng, giữ cửa sổ trong safe area, bộ icon `anhnew` và logic/ID Trừng trị của v12. IPA trong release giống từng byte với IPA v13 đã kiểm tra; SHA256: `5cf5d6b1211ba2e82595a308ef7c20d77316f90d7a92b9f41482ea9921060e92`.

Đã kiểm lại CRC ZIP, hash IPA, ba source menu được ghi trong báo cáo và ba asset anhnew khi nâng release. Báo cáo xác minh trước đó gồm bảy bộ test PASS, build ARM64 iOS PASS và 205 hash trang chữ ký hợp lệ. Việc nâng release không bổ sung kết quả thử thiết bị: v13 chưa được xác nhận chạy trên iPhone trong phiên này, cần ký lại toàn bộ IPA/framework bằng chứng chỉ của người dùng trước khi cài.

Giữ nguyên bản pathfix và các IPA v11/v12. Bộ phát hành có source snapshot, preview host, báo cáo đóng gói, báo cáo validation và manifest/checksum tại `release`; xem `release/README-V13.md`. Source snapshot cần toolchain/SDK cùng IPA v12 đang lưu trong project để build/đóng gói lại; không kèm IPA nền hay toolchain.


## Bổ sung ngày 20/10/2026 — dọn IPA thừa theo yêu cầu người dùng

Đã chuyển vào Thùng rác hai file: `project/KHTN-SMLnobita-v11.ipa` (bản cũ, không là đầu vào pipeline hiện hành) và `project/KHTN-SMLnobita-v13.ipa` (trùng SHA256 với bản release). Tổng 443 844 067 byte, khoảng 423,28 MiB. Các ghi chú phía trên nói giữ v11/project v13 mô tả trạng thái trước mốc dọn này.

IPA v13 dùng hiện tại nằm ở `release/KHTN-SMLnobita-v13.ipa`. Script package_menu_v13.py vẫn xuất lại project/KHTN-SMLnobita-v13.ipa khi chạy; báo cáo đóng gói giữ đường dẫn nguồn lịch sử. Script promote_v13_release.py là nhật ký thao tác một lần, không cần chạy lại.

Giữ bốn IPA: release v13 để sử dụng; release pathfix làm đầu vào script v11/v12 và bản dự phòng; project v12 làm nền đóng gói v13; project no-guide làm seed tái tạo pathfix. Đã kiểm tra hash bốn file trước/sau dọn, không thay đổi. Không sửa artifact/checksum/source snapshot trong release. Báo cáo: `management/cleanup-ipa-v13-report.json`.

## Bổ sung ngày 23/10/2026 — pipeline v16 trong NobiCore

- Source hiện hành: v16; menu/Trừng trị không đổi so với v13.
- Build Windows/Zig: `Set-Location project`, chạy `.\build-khtn.ps1 -InstallName "@rpath/SMLnobita.framework/SMLnobita"` rồi `python build\prepare_module_khtn.py`. Script đã thêm `src\TssGuard.mm`.
- Đóng gói: đặt `KHTN-SMLnobita-v15.ipa` (SHA256 `fc8b9f91e25b82bbb3abd5e7ddb5d18ac49b0c2ae227586dc620ec1d55ac0a54`) vào `project/`, chạy `python build\package_menu_v16.py`. Script chỉ thay entry SMLnobita, kiểm tra CRC/entry list, xuất `project/KHTN-SMLnobita-v16.ipa` và báo cáo trong `project/build/`.
- Toolchain/SDK và IPA nền không nằm trong repository. `build-khtn-macos.sh` và `Makefile` chưa được đồng bộ TssGuard; không coi là pipeline đã kiểm tra.
- Bảy test host và build ARM64 đã PASS trên máy gốc; xem `verification/validation-v16.json`. Chưa thử iPhone từ repository này.

## Bổ sung ngày 23/10/2026 — build/test lại trong NobiCore

Đã build lại source v16 trong NobiCore bằng Zig 0.13 + iPhoneOS16.5 SDK (junction tới toolchain hiện có), module ad-hoc 211 trang; bảy test host PASS. Module SHA256 `2cab2c21ff3dbe886c707fd4f3bf176b8d3e82fb0c5240bab6934ce96cfecbc3`, khác release `7df87652f3aba8e2728fd388f8c5f013410f361c9c2206a9c2408d9504e6c895` vì build root khác: sau khi chuẩn hoá đường dẫn, toàn bộ section code/data đã khởi tạo trùng nhau (gồm `__TEXT,__text`), chỉ khác `LC_UUID`, bảng symbol/string trong `__LINKEDIT` và section `__bss` zero-fill. Đây không phải bằng chứng ký, cài trên thiết bị hay xác nhận phía server. Chi tiết: `verification/nobicore-build-v16.json`.
