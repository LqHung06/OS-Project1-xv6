# Project 1: Xv6, Unix Utilities and System Call

Đồ án môn học **Hệ Điều Hành (Operating System)**  
Môi trường phát triển: **xv6 trên kiến trúc RISC-V** (Ubuntu / Linux)

---

## 👥 Thành viên nhóm & Phân công nhiệm vụ

| STT | Họ và Tên | MSSV | Nhiệm vụ chính | Nhánh Git |
| :---: | :--- | :---: | :--- | :--- |
| 1 | **Lê Quốc Hưng** *(Trưởng nhóm)* | [Điền MSSV] | **Bài 3: System call `tracing`** & Quản trị Git | `feat/trace` |
| 2 | [Họ và Tên TV2] | [Điền MSSV] | **Bài 4: System call `procinfo`** | `feat/procinfo` |
| 3 | [Họ và Tên TV3] | [Điền MSSV] | **Bài 1: Tiện ích `xargs`** | `feat/xargs` |
| 4 | [Họ và Tên TV4] | [Điền MSSV] | **Bài 2: Tiện ích `tree`** | `feat/tree` |

---

## 🚀 Hướng dẫn cài đặt & Chạy thử

### 1. Cài đặt môi trường (Ubuntu / Debian / WSL2)
```bash
sudo apt-get update
sudo apt-get install -y git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

### 2. Biên dịch và khởi động xv6
```bash
make clean
make qemu
```
* **Thoát khỏi QEMU:** Nhấn tổ hợp phím `Ctrl + a`, nhả tay ra rồi bấm `x`.

---

## 🛠️ Quy trình làm việc nhóm (Git Workflow)

1. **Nhánh làm việc chính:** `main`
2. **Quy tắc tạo nhánh:** Mỗi thành viên kéo code mới nhất từ `main` và tạo nhánh riêng:
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feat/<ten-bai>
   ```
3. **Quy tắc nộp code:**
   * Sau khi code và tự kiểm thử thành công bằng `make qemu`, đẩy nhánh lên GitHub.
   * Mở **Pull Request (PR)** gộp vào `main`.
   * **Bắt buộc:** Phải có ít nhất 1 thành viên khác review và bấm **Approve** thì PR mới được phép merge vào `main`.

---

## 📦 Đóng gói nộp bài (Trưởng nhóm phụ trách)

Trước khi nộp bài, đứng tại nhánh `main` và thực hiện:
```bash
make clean
git diff origin/syscall...HEAD > MSSV1_MSSV2_MSSV3_MSSV4.patch
```
Nén toàn bộ thư mục, file patch và file báo cáo PDF theo đúng cấu trúc: `MSSV1_MSSV2_MSSV3_MSSV4.zip`.
