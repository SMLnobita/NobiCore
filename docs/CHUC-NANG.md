# Chức năng của bản chốt

## Menu và nhận diện

- App tên KHTN; framework cuối tên SMLnobita.
- Tiêu đề: Liên Quân Menu By Nguyễn Hoà.
- Giao diện xanh đen/xanh ngọc, chữ tiếng Việt.
- Menu khởi đầu ẩn. Chạm đồng thời 3 ngón tay 2 lần để mở; làm lại để đóng.
- Kéo thanh tiêu đề để di chuyển menu; có nút đóng.
- Không có nút C6 nổi, không còn Stream Mode, tab Hướng dẫn hay bảng thống kê tướng/quái/HP/ngưỡng/cấp.
- Chỉ tab đang chọn có nền xanh; các tab khác nền tối.

## Ba tab

| Tab | Điều khiển |
| --- | --- |
| Map & camera | Hack map, Camera, Khóa camera, thanh Gần–xa từ −2 đến 5 |
| Bổ trợ | Tự Bộc phá, ngưỡng % máu địch, Tự Trừng trị/Tê tái, Chỉ Rồng/Tà Thần/KingKong/Rồng bạo chúa |
| Skin & tên | Mod skin, Hiện tên tướng |

Tất cả toggle chức năng mặc định tắt; lọc chỉ Rồng/Tà Thần mặc định bật. Giá trị camera cộng thêm mặc định 0.6; ngưỡng Bộc phá mặc định 13.7%.

## Những sửa đổi hành vi đã làm

- Map có đường áp dụng thay đổi trên luồng logic; không chỉ chờ callback nhìn thấy địch.
- Camera có revision để làm mới khi đổi setting.
- Auto Trừng trị đọc sát thương hiện tại từ accessor mà UI game sử dụng, cho từng mục tiêu. Nếu accessor trả ≤ 0, dùng công thức donor `1350 + 100*(cấp - 1)`; công thức chỉ là fallback, không thay thế giá trị thực tế khi accessor có giá trị.
- Gửi ReadyUseSkill rồi RequestUseSkill, không dừng chỉ vì Ready trả false.
- Hiện tên tướng dùng bảng tên trích từ bản đối chiếu. Khi tắt, khôi phục bản chụp UTF16 tên/prefix/custom trên HUD thay vì tự đặt “Kiện tướng”.
- Mod skin là nhãn mới của cùng control skin trước đây; đổi nhãn không thêm chức năng mới.

## v12 — Trừng trị hybrid và ID boss đã xác minh

Bản v12 nâng cấp riêng Trừng trị, giữ nguyên phần còn lại và **không thêm control**:
sát thương động có fallback công thức, nhận mục tiêu theo config ID thay vì kiểu actor,
chỉ đọc đúng slot 5 như donor. Nhóm boss gồm 7009, 7012, 7024, 70092, 70093,
King Kong 7051/70514, Rồng bạo chúa 7050/70504 và model Kraydus 702400/702411.
Tắt lọc boss sẽ xét thêm bùa thường 7010/7011. Hai siêu bùa 547010/547011 bị loại
hẳn theo yêu cầu người dùng, kể cả khi tắt lọc. Việc gán chúng thành King Kong/Rồng
trong tài liệu v11 cũ là sai. Chi tiết: `docs/V12-TRUNG-TRI.md`.

## Mức xác minh

v13 bổ sung nhãn tự xuống dòng khi thu nhỏ menu, kẹp toàn bộ cửa sổ vào vùng an toàn hiện tại khi xoay/kéo/mở lại, và bộ icon KHTN do người dùng cung cấp trong `anhnew`. Không đổi runtime v12. Xem `docs/V13-MENU-ICON.md`.

Đã có kiểm tra host cho logic, bridge, luồng frame/map và routing menu; đã dựng preview bằng đúng ImGui/font/theme. Đã kiểm tra ZIP, các đường nạp framework và hash các CodeDirectory của gói cuối. Người dùng đã xác nhận bản pathfix ổn.

Độ tương đương đầy đủ của tiến trình skin Evo với bản 6bohuel chưa được chứng minh. Mock/test host không tự chứng minh server đã thực thi mỗi lần Auto Bộc phá/Trừng trị. Tên tướng là thay đổi HUD cục bộ. Không ghi nhận một cơ chế anti-ban đã được xác minh.
