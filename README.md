# Student Management System

Ứng dụng console quản lý sinh viên viết bằng C++17, dùng CSV cho lưu trữ. Hệ thống hỗ trợ phân quyền ADMIN/STUDENT, xóa mềm sinh viên, đăng ký học phần có kiểm tra sĩ số, điểm Midterm/Final và audit log.

## Build

### Linux/macOS

```bash
make
./sms
```

### Windows (MinGW)

Chạy `build.bat`, hoặc biên dịch trực tiếp:

```bat
g++ src/main.cpp src/CsvUtils.cpp src/Student.cpp src/Course.cpp src/CourseOffering.cpp src/Enrollment.cpp src/User.cpp src/AuditLogger.cpp src/StudentManager.cpp -I include -std=c++17 -Wall -o sms.exe
sms.exe
```

## Dữ liệu

| Tệp | Nội dung |
|---|---|
| `data/students.csv` | Sinh viên: id, name, age, course, status |
| `data/courses.csv` | Môn học: id, name, credits |
| `data/offerings.csv` | Lớp học phần: id, course_id, semester, capacity |
| `data/enrollments.csv` | Đăng ký và điểm thành phần |
| `data/users.csv` | Tài khoản: username, password, role, ref_id |
| `data/audit.log` | Nhật ký vận hành, được tạo khi có hành động thành công |

CSV hỗ trợ trường có dấu phẩy hoặc dấu nháy kép theo chuẩn quoting CSV. Khi nạp phát hiện dòng sai hoặc tham chiếu không hợp lệ, hệ thống từ chối lưu để bảo vệ dữ liệu nguồn.

## Tài khoản mẫu

| Username | Password | Role |
|---|---|---|
| `admin` | `admin123` | ADMIN |
| `student101` | `student123` | STUDENT |

## Quy tắc chính

- Student bị vô hiệu hóa bằng `status=inactive`; không bị xóa khỏi CSV.
- Một sinh viên không được đăng ký hai lần vào cùng lớp học phần active.
- Enrollment chỉ được tạo khi lớp còn chỗ và lớp tham chiếu môn học tồn tại.
- Điểm Midterm và Final phải là số hữu hạn từ 0 đến 10. Điểm tổng kết là `30% Midterm + 70% Final`; khi thiếu điểm, màn hình hiển thị `Chưa có điểm`.
- Lưu CSV dùng tệp tạm và rollback khi không thể thay thế đầy đủ các tệp dữ liệu.

## Kiểm thử đầu vào tự động

`test_input.txt` chứa một kịch bản console mẫu. Trên Linux/macOS:

```bash
./sms < test_input.txt
```

Xem thêm đặc tả nghiệp vụ tại `docs/Backend_Business_Workflow_Spec.docx`.
