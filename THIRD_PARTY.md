# Thành phần và nguồn gốc

`project/menu/IMGUI` chứa Dear ImGui, các header imstb và backend Metal từ bộ source hiện có. Giữ nguyên các thông báo tác giả/giấy phép nằm trong từng file. `zzz.h` chứa dữ liệu font nén đi cùng menu gốc; bộ lưu hiện tại không có tài liệu giấy phép font riêng.

Các icon trong `anhnew` do người dùng cung cấp. Chưa đặt giấy phép chung cho mã riêng, font hay hình ảnh trong bộ export này; cần xác định quyền phân phối và bổ sung giấy phép phù hợp trước khi phát hành công khai. SDK, compiler và dữ liệu game không nằm trong bộ export.

## Bổ sung ngày 23/10/2026 — TssGuard

project/src/TssGuard.mm là phần triển khai lại từ phân tích hành vi của AWSS3 donor, không chép mã nguồn và không kèm binary/SDK của donor. Việc phân phối công khai vẫn cần xác định giấy phép và quyền phân phối cho mã riêng, font và hình ảnh trong repository.
