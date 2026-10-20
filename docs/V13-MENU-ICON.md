# v13 — Menu thích ứng và icon anhnew

Ngày 20/10/2026, sau khi người dùng duyệt preview trước/sau và yêu cầu áp dụng cùng thư mục `anhnew`.

## Thay đổi

- Nhãn checkbox dài tự xuống dòng theo vùng nội dung; ô vuông và chữ đều bấm được, vẫn định tuyến về đúng option.
- `KHTNMenuLayout.h` tính vùng an toàn từ kích thước view hiện tại và safeAreaInsets của UIWindow. Trước mỗi Begin, kẹp vị trí và kích thước cửa sổ vào vùng này; giữ vị trí đang kéo nếu còn hợp lệ. Xử lý cả xoay màn hình, mở lại và viewport nhỏ hơn kích thước tối thiểu cũ. Lề là 16 điểm bên trong vùng an toàn.
- Vẫn 3 tab, 8 checkbox, gesture ba ngón chạm hai lần; không thêm control hoặc thay đổi runtime Trừng trị v12.
- Thay đúng ba entry trong app bằng nguyên byte ba file người dùng cung cấp: AppIcon60x60@2x.png (120x120), AppIcon76x76@2x~ipad.png (152x152), Assets.car. Tên AppIcon của catalog khớp Info.plist; không cần đổi plist. Đây là bộ logo KHTN.

## Artifact

- IPA: `project/KHTN-SMLnobita-v13.ipa`.
- Kích thước: 220.677.871 byte.
- SHA256: `5cf5d6b1211ba2e82595a308ef7c20d77316f90d7a92b9f41482ea9921060e92`.
- Module SHA256: `8e03781586c2ae6943d66fa68ea789643db848be741b10983c32b9d78d597a0a`.
- Báo cáo đóng gói: `project/build/KHTN-SMLnobita-v13.report.json`.
- Kiểm tra: `management/validation-v13/validation.json`.

So với IPA v12, chỉ bốn entry khác: module SMLnobita và ba asset icon. Danh sách entry ZIP không đổi, mọi entry còn lại giống từng byte, gồm kgvn, UnityFramework và Info.plist. IPA v12 và bản chốt pathfix giữ nguyên hash.

## Build

```powershell
Set-Location D:\rea\lienquan\lienquan\project
.\build-khtn.ps1 -InstallName '@rpath/SMLnobita.framework/SMLnobita'
python build\prepare_module_khtn.py
python build\package_menu_v13.py
```

Giữ IPA v12 và thư mục `anhnew` để tái tạo. Script kiểm tra hash v12 đầu vào, PNG/kích thước, magic catalog, đường dẫn framework, CRC ZIP, số entry thay đổi và hash asset/module/source.

## Đã kiểm tra

- test_menu_layout: nhãn vừa vùng nội dung 252 điểm khi menu rộng 280; đủ hai dòng; bấm chữ và ô vuông mỗi lần đổi đúng một lần; xoay ngang/dọc với safe-area, collapse/mở lại, viewport nhỏ đều nằm gọn.
- test_khtn: đủ 8 checkbox định tuyến đúng.
- test_frame, test_selected, test_map, test_logic, test_imgui_bridge: pass.
- Build ARM64 iOS: pass, runtime/frontend với -Wall -Wextra -Werror.
- Kiểm tra độc lập 205 hash trang chữ ký module, đường nạp SMLnobita trong kgvn/UnityFramework/module; icon trong IPA khớp từng byte với anhnew.
- Hash mọi file project/src giữ nguyên so với v12. Các thay đổi này chỉ ở frontend/menu và đóng gói icon.

Chưa chạy v13 trên iPhone. Chữ ký module mới là ad-hoc; cần ký lại toàn bộ IPA/framework bằng chứng chỉ trước khi cài. Các kiểm tra host không tự chứng minh gesture, icon SpringBoard hay hoạt động game trên thiết bị.


## Bổ sung ngày 20/10/2026 — v13 được nâng lên release

Theo yêu cầu người dùng, **bản release hiện hành là `release/KHTN-SMLnobita-v13.ipa`**. Các đoạn phía trên gọi pathfix là bản chốt hoặc v13 là nhánh nâng cấp được giữ làm lịch sử; mốc này cập nhật trạng thái phát hành hiện hành.

V13 gồm menu tự xuống dòng, giữ cửa sổ trong safe area, bộ icon `anhnew` và logic/ID Trừng trị của v12. IPA trong release giống từng byte với IPA v13 đã kiểm tra; SHA256: `5cf5d6b1211ba2e82595a308ef7c20d77316f90d7a92b9f41482ea9921060e92`.

Đã kiểm lại CRC ZIP, hash IPA, ba source menu được ghi trong báo cáo và ba asset anhnew khi nâng release. Báo cáo xác minh trước đó gồm bảy bộ test PASS, build ARM64 iOS PASS và 205 hash trang chữ ký hợp lệ. Việc nâng release không bổ sung kết quả thử thiết bị: v13 chưa được xác nhận chạy trên iPhone trong phiên này, cần ký lại toàn bộ IPA/framework bằng chứng chỉ của người dùng trước khi cài.

Giữ nguyên bản pathfix và các IPA v11/v12. Bộ phát hành có source snapshot, preview host, báo cáo đóng gói, báo cáo validation và manifest/checksum tại `release`; xem `release/README-V13.md`. Source snapshot cần toolchain/SDK cùng IPA v12 đang lưu trong project để build/đóng gói lại; không kèm IPA nền hay toolchain.


## Bổ sung ngày 20/10/2026 — dọn IPA thừa theo yêu cầu người dùng

Đã chuyển vào Thùng rác hai file: `project/KHTN-SMLnobita-v11.ipa` (bản cũ, không là đầu vào pipeline hiện hành) và `project/KHTN-SMLnobita-v13.ipa` (trùng SHA256 với bản release). Tổng 443 844 067 byte, khoảng 423,28 MiB. Các ghi chú phía trên nói giữ v11/project v13 mô tả trạng thái trước mốc dọn này.

IPA v13 dùng hiện tại nằm ở `release/KHTN-SMLnobita-v13.ipa`. Script package_menu_v13.py vẫn xuất lại project/KHTN-SMLnobita-v13.ipa khi chạy; báo cáo đóng gói giữ đường dẫn nguồn lịch sử. Script promote_v13_release.py là nhật ký thao tác một lần, không cần chạy lại.

Giữ bốn IPA: release v13 để sử dụng; release pathfix làm đầu vào script v11/v12 và bản dự phòng; project v12 làm nền đóng gói v13; project no-guide làm seed tái tạo pathfix. Đã kiểm tra hash bốn file trước/sau dọn, không thay đổi. Không sửa artifact/checksum/source snapshot trong release. Báo cáo: `management/cleanup-ipa-v13-report.json`.
