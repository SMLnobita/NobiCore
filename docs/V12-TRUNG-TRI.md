# v12 — Cập nhật ID boss, loại siêu bùa

> Source hiện tại đã nâng giao diện lên v13. IPA v12 ở đây được giữ nguyên làm nền đóng gói v13; không chạy lại script package v12 với module giao diện v13. Quy trình hiện hành ở `docs/V13-MENU-ICON.md`.

Ngày 20/10/2026. Áp dụng bảng người dùng đã review, sau đó bỏ hẳn 547010/547011 theo yêu cầu. Đây là bản nâng cấp riêng, giữ IPA pathfix và v11 cũ.

## Mục tiêu

| Nhóm | Config ID |
| --- | --- |
| Caesar/Rồng đã có | 7009, 7012, 7024, 70092, 70093 |
| King Kong | 7051 |
| King Kong thời không | 70514 |
| Rồng bạo chúa | 7050 |
| Rồng bạo chúa thời không | 70504 |
| Rồng bạo chúa Kraydus, model Enhanced Crystal Dragon | 702400, 702411 |
| Bùa thường, chỉ xét khi tắt lọc boss | 7010, 7011 |
| Loại hẳn, kể cả khi tắt lọc | 547010, 547011 |

Tổng whitelist là 13 ID: 11 boss và 2 bùa thường. Hai ID bị loại là Siêu Khổng lồ Thuật sĩ/Lực sĩ, không phải King Kong/Kraydus. Mapping được giải nén từ normalMonster và bảng tên tiếng Việt, có CRC/hash tại `analysis/kraydus/`.

Nhóm Trừng trị trong menu vẫn hai công tắc: Tự Trừng trị/Tê tái và Chỉ Rồng/Tà Thần/KingKong/Rồng bạo chúa. Không thêm control. Giữ slot 5, sát thương động + fallback, tầm 6.5, hồi chiêu/giãn cách 0.3 s và Ready → Request như v11. Bộc phá, map, camera, skin và tên không đổi.

## Gói và build

- IPA: `project/KHTN-SMLnobita-v12.ipa`, 223.167.171 byte.
- SHA256 IPA: `c58e0e3c80b0e1b962c74ae3612e484da14b80fb8981b4046af9df3715efd70f`.
- Module SHA256: `4acd1e3c5ff8716bb801657d0dcc506d45b6a0b53f4134d40289e333655d80bf`.
- Báo cáo: `project/build/KHTN-SMLnobita-v12.report.json`.
- Entry duy nhất thay đổi so với pathfix: `Payload/kgvn.app/Frameworks/SMLnobita.framework/SMLnobita`.

```powershell
Set-Location D:\rea\lienquan\lienquan\project
.\build-khtn.ps1 -InstallName '@rpath/SMLnobita.framework/SMLnobita'
python build\prepare_module_khtn.py
python build\package_trungtri_v12.py
```

## Xác minh

- Biên dịch mới và chạy test_frame, test_selected, test_map, test_logic, test_imgui_bridge: pass.
- test_frame kiểm tra gửi yêu cầu cho đủ 11 ID boss khi bật lọc, và không gửi cho 547010/547011 ở cả hai trạng thái lọc. Các ca slot 5, fallback, hồi chiêu và Ready=false vẫn qua.
- Build ARM64 iOS thành công; runtime/frontend với -Wall -Wextra -Werror.
- ZIP CRC qua; mọi entry ngoài module giống từng byte với pathfix; install name đúng SMLnobita; 205 hash trang chữ ký module hợp lệ.
- Chưa có kiểm thử iPhone cho v12. Cần ký toàn bộ IPA/framework bằng chứng chỉ trước khi cài. Mapping config từ dữ liệu không thay thế kiểm tra việc server thực thi Trừng trị trong trận.
