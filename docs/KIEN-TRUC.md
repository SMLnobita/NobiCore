# Cấu trúc và dữ liệu để tiếp tục

## Source

| File | Vai trò |
| --- | --- |
| `project/src/Clone6.c` | Gắn runtime, chọn frontend ImGui; tên source nội bộ được giữ |
| `HookLogic.h` | Hook map/camera và trạng thái/revision |
| `SelectedRuntime.h` | Lịch chạy, actor/target, skin, Bộc phá, Trừng trị, tên HUD |
| `MapRuntime.h` | Áp dụng revision map trên luồng logic |
| `HeroNames.h` | Bảng tên trích từ dữ liệu đối chiếu |
| `Clone6Bridge.h`, `Clone6BridgeImpl.h` | Cầu nối UI và runtime |
| `project/menu/ImGuiDrawView.mm` | Metal overlay, gesture và touch |
| `Clone6MenuContent.h` | Nội dung ba tab, tên control |
| `KHTNTheme.h` | Màu, tiêu đề và style |
| `project/menu/IMGUI` | ImGui 1.83WIP, Metal backend, font tiếng Việt |

Tên class/ABI nội bộ C6/Clone6 và tên file source không cần trùng tên framework đóng gói. Đừng thay chuỗi hàng loạt trong Mach-O; có thể làm hỏng string table/offset. Tên cũ còn trong log hoặc đường dẫn debug không phải đường nạp framework.

`release/KHTN-SMLnobita-pathfix-source.zip` gồm snapshot no-guide và script patch để tái tạo bản chốt. Source đang sửa trong `project` có thay đổi UI mới hơn snapshot. Hai dạng này được giữ riêng để phân biệt code tham khảo/chỉnh sửa với công thức tái tạo chính xác gói đã xác nhận.

## Toolchain

- Windows: `project/build/zig013/ziglang/zig.exe`, Zig 0.13.0.
- SDK: `project/build/apple-sdks/iPhoneOS16.5.sdk`.
- SDK lấy từ theos/sdks, commit `0222fd5413cf4b9af096f37b4621afa2688572f7`.
- Build script đặt ZIG_LIB_DIR đúng runtime đi kèm. Dùng explicit SDK libc++.tbd, không đổi sang build libc++ Darwin của Zig.
- `ios-cxx-compat.h` xử lý tương thích header; các `.mm` dùng ARC.
- Backend ObjC có prefix C6Menu để tránh trùng class.
- Các source test được giữ; file exe/pdb test có thể biên dịch lại.

## Dữ liệu đầu vào

- `originals/com.garena.game.kgvn_1.64.1.7_und3fined`: game nền nguyên bản.
- `originals/6bohuel`: bản dùng để đối chiếu.
- `originals/menu-original-backup.zip`: menu người dùng đưa trước khi ghép.
- `analysis/dump/result/dump.cs`, `DummyDll`, `il2cpp.h`, `script.json`, `stringliteral.json`: đầu ra dump.
- `analysis/dump/metadata-runtime.dat`: metadata đã khôi phục header.
- `analysis/dump/metadata-dumper.dat`: bản điều chỉnh cho Il2CppDumper; một số HasFieldRVA initializer không biểu diễn thành constant. Xem README và verification trong thư mục dump.
- `analysis/comparison`: tài liệu so sánh, assembly, bảng tên và IDA database.
- `tools/Il2CppDumper-win-v6.7.46`: công cụ dump.
- `tools/tooldoofffset/fixed`: OffsetFinder đã sửa và đầu vào tự chứa.

## Giới hạn phiên bản và đường dẫn

Toàn bộ kết quả áp dụng cho game 1.64.1.7 / build 1.64.11768577 đã phân tích. Offset và bố cục phải được kiểm tra lại khi đổi phiên bản game. dump.cs không phải mã nguồn gameplay đầy đủ.

Các tài liệu và script phân tích lịch sử có thể còn đường dẫn `D:\rea\6bohuel`, `D:\rea\dump`, `D:\rea\clone6bohuel`. Nay đối chiếu sang originals, analysis/dump và project như bảng trên. Hai script tái tạo bản cuối dùng vị trí của chính script, đã chạy thành công sau khi di chuyển. Không tạo junction ngược tới thư mục cũ.


## Bổ sung ngày 20/10/2026 — cấu trúc hiện tại của v13

- Root đang dùng là `D:\rea\lienquan\lienquan`; các đường dẫn một cấp `D:\rea\lienquan` trong phần lịch sử cần đối chiếu lại root thực tế này.
- `project/menu/KHTNMenuLayout.h` là helper dùng chung cho frontend và host test: tính safe area, kẹp vị trí/kích thước cửa sổ trước mỗi Begin. `ImGuiDrawView.mm` truyền kích thước view và safeAreaInsets hiện tại.
- `project/menu/Clone6MenuContent.h` có đường xuống dòng cho nhãn dài; vẫn giữ ba tab và tám checkbox.
- `project/build/test_menu_layout.cpp` kiểm tra nhãn hẹp, thao tác bấm chữ/ô vuông, xoay với safe area, collapse/mở lại và viewport nhỏ.
- `project/build/package_menu_v13.py` lấy `project/KHTN-SMLnobita-v12.ipa` làm nền, thay module và ba asset từ `anhnew`; giữ IPA v12 cùng anhnew làm đầu vào.
- `anhnew` gồm AppIcon60x60@2x.png, AppIcon76x76@2x~ipad.png và Assets.car; đã đưa nguyên byte vào app trong v13.
- `analysis/kraydus` giữ bằng chứng giải nén bảng quái/tên, mapping 765 record và script tái kiểm tra ID. Các ID mới đã áp dụng từ v12, runtime v13 giữ nguyên.
- `management/validation-v13/validation.json` và `project/build/KHTN-SMLnobita-v13.report.json` giữ kết quả kiểm tra/gói; nhật ký đầy đủ ở `10_20_2026v2.md`.

## Bổ sung ngày 23/10/2026 — TssGuard và đóng gói v16

- `project/src/TssGuard.mm` là constructor ObjC++ chạy sớm: stub các lớp Tss/Ano, cài 6 hook `NSURLSession`/`NSURLConnection`/`NSFileManager`, lọc URL/body và trả fake HTTP 200 cho request bị chặn.
- `project/build-khtn.ps1` thêm `src\TssGuard.mm` vào danh sách source; đây là thay đổi build duy nhất ngoài source mới.
- `project/build/package_menu_v16.py` lấy nền `KHTN-SMLnobita-v15.ipa`, kiểm tra SHA256, thay module và chỉ entry SMLnobita; không đụng asset/menu.
- `verification/package-v16.json` và `verification/validation-v16.json` là báo cáo đóng gói/xác minh release v16. `SOURCE-SHA256.json` được cập nhật sau khi thêm các file trên.
- `build-khtn-macos.sh`/`Makefile` vẫn là tài liệu lịch sử, chưa có TssGuard; chỉ pipeline PowerShell/Zig được xác minh.
