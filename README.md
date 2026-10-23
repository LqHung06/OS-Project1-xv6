# Project 1: Xv6, Unix Utilities and System Call

Đồ án môn học **Hệ Điều Hành (Operating System)**  
Môi trường phát triển: **xv6 trên kiến trúc RISC-V** (Ubuntu / WSL2 / QEMU)  
Kho mã nguồn: [OS-Project1-xv6](https://github.com/LqHung06/OS-Project1-xv6.git)  
Nhánh làm việc chính: `main` | Nhánh mốc xuất patch: `syscall` (đóng băng)

---

## 👥 Danh sách Thành viên & Phân công nhiệm vụ

Nhóm gồm **4 thành viên** (theo phân công của Giảng viên), mỗi thành viên phụ trách độc lập 1 bài toán kỹ thuật (2.5 điểm/bài):

| STT | Họ và Tên | MSSV | Nhiệm vụ kỹ thuật chính | Nhánh Git |
| :---: | :--- | :---: | :--- | :--- |
| 1 | **Lê Quốc Hưng** *(Trưởng nhóm)* | **24120184** | **Bài 3 — System call `tracing`** (Mục 4.2.1, 2.5đ)<br>Quản trị Git, điều phối merge PR & đóng gói nộp bài | `feat/trace` |
| 2 | **Lê Công Minh Nhựt** | **24120211** | **Bài 4 — System call `procinfo`** (Mục 4.2.2, 2.5đ)<br>Xây dựng helper kernel + test `user/procinfotest.c` | `feat/procinfo` |
| 3 | **Nguyễn Lê Anh Kiên** | **24120196** | **Bài 1 — Tiện ích `xargs`** (Mục 4.1.1, 2.5đ)<br>Xử lý `stdin`, quản lý `MAXARG`, `fork()`, `exec()`, `wait()` | `feat/xargs` |
| 4 | **Võ Minh Khoa** | **24120194** | **Bài 2 — Tiện ích `tree`** (Mục 4.1.2, 2.5đ)<br>Duyệt đệ quy thư mục, định dạng thụt lề & xử lý lỗi | `feat/tree` |

---

## 🚀 Hướng dẫn Cài đặt & Khởi chạy

### 1. Cài đặt môi trường RISC-V (Ubuntu / Debian / WSL2)
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

## 🛠️ Quy trình Làm việc Nhóm (Git Workflow)

* **Nhánh chính (`main`):** Nhánh tích hợp sản phẩm cuối cùng. Không ai được phép push code trực tiếp lên `main`.
* **Nhánh mốc (`syscall`):** Nhánh đóng băng cố định tại commit gốc của MIT để so sánh diff khi xuất patch. **Tuyệt đối không commit hay merge vào nhánh này.**

### Các bước làm việc cho từng thành viên:
1. **Cập nhật code mới nhất và tạo nhánh tính năng:**
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feat/<ten-nhanh-cua-ban>
   ```
2. **Viết code và kiểm thử trong QEMU:**
   * Tuân thủ quy ước ID System Call: `SYS_trace` = 22, `SYS_procinfo` = 23.
   * Chạy `make clean && make qemu` để kiểm tra.
3. **Commit và tạo Pull Request:**
   ```bash
   git add <cac-file-thay-doi>
   git commit -m "feat(<ten-bai>): mo ta ngan gon thay doi"
   git fetch origin
   git rebase origin/main
   git push -u origin feat/<ten-nhanh-cua-ban>
   ```
4. **Quy tắc Review & Merge:**
   * Mở Pull Request hướng về nhánh `main`.
   * **Bắt buộc:** Phải có ít nhất 1 thành viên khác review và bấm **Approve** thì PR mới được phép gộp vào `main`.

---

## 🧪 Các ca kiểm thử nhanh (Testing Reference)

* **Bài 1 (`xargs`):**
  ```bash
  $ echo hello too | xargs echo bye
  $ (echo 1 ; echo 2) | xargs echo
  $ sh < xargstest.sh
  ```
* **Bài 2 (`tree`):**
  ```bash
  $ mkdir a; echo > a/b; mkdir a/aa; echo > a/aa/b; echo > a/aa/c; mkdir a/ab; echo > a/ab/d
  $ tree a
  $ tree .
  $ tree /
  ```
* **Bài 3 (`trace`):**
  ```bash
  $ trace 32 grep hello README
  $ trace 2147483647 grep hello README
  $ trace 2 usertests forkforkfork
  ```
* **Bài 4 (`procinfo`):**
  ```bash
  $ procinfotest
  ```

---

## 📦 Đóng gói Nộp bài (Trưởng nhóm thực hiện)

Trước hạn nộp bài, đứng tại nhánh `main` và thực hiện:
```bash
# 1. Dọn dẹp file nhị phân tạm thời
make clean

# 2. Xuất file patch so với nhánh mốc syscall
git diff origin/syscall...HEAD > 24120184_24120194_24120196_24120211.patch

# 3. Kiểm tra danh sách file thay đổi
git diff --stat origin/syscall...HEAD
```
Nén thư mục mã nguồn sạch `OS-Project1-xv6`, file `.patch` và file báo cáo `24120184_24120194_24120196_24120211_Report.pdf` thành file zip duy nhất:
👉 **`24120184_24120194_24120196_24120211.zip`**
