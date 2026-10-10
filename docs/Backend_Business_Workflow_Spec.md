---
title: "Đặc tả nghiệp vụ và Workflow Backend"
subtitle: "Student Management System – C++17 + CSV"
date: "10/10/2026"
lang: vi-VN
toc: true
toc-depth: 3
---

# 1. Mục đích và phạm vi

Tài liệu này mô tả quy trình nghiệp vụ của backend Student Management System sau khi tái cấu trúc. Hệ thống là ứng dụng console C++17, lưu trữ dữ liệu bằng CSV, không dùng cơ sở dữ liệu và không phụ thuộc thư viện ngoài khi chạy ứng dụng.

Backend phục vụ bốn nhóm nghiệp vụ chính: quản lý sinh viên, quản lý danh mục học phần, đăng ký học phần và quản lý điểm. Hệ thống cũng cung cấp phân quyền theo vai trò, xóa mềm, mô phỏng giao dịch cho đăng ký và nhật ký kiểm toán.

Phạm vi hiện tại gồm hai giao diện nghiệp vụ đang hoạt động:

| Vai trò | Phạm vi thao tác |
|---|---|
| ADMIN | Quản lý sinh viên, xem dữ liệu, vô hiệu hóa sinh viên, nhập/sửa điểm, lưu dữ liệu, xem thống kê |
| STUDENT | Xem hồ sơ cá nhân, xem lớp học phần, đăng ký học phần, xem các đăng ký và điểm |

Vai trò `TEACHER` được hỗ trợ trong cấu trúc `users.csv`, nhưng phiên bản hiện tại chưa cung cấp menu riêng cho vai trò này.

# 2. Kiến trúc nghiệp vụ

## 2.1 Luồng tổng quát

```text
Khởi động
  -> Nạp toàn bộ CSV vào unordered_map
  -> Đăng nhập users.csv
  -> Xác định role
       -> ADMIN menu
       -> STUDENT menu
  -> Thực hiện nghiệp vụ
  -> Ghi CSV khi lưu/commit
  -> Ghi audit.log cho thao tác thành công
  -> Đăng xuất hoặc đăng nhập phiên tiếp theo
```

`StudentManager` là lớp điều phối nghiệp vụ. Dữ liệu được nạp vào `std::unordered_map<string, Object>` với khóa là ID/username để truy vấn trung bình O(1). Khi chương trình kết thúc phiên có thay đổi chưa lưu, dữ liệu sẽ được lưu xuống CSV.

## 2.2 Thành phần chính

| Thành phần | Trách nhiệm |
|---|---|
| `StudentManager` | Nạp/lưu dữ liệu, đăng nhập, phân quyền, điều phối nghiệp vụ |
| `Student` | Hồ sơ sinh viên và trạng thái hoạt động |
| `Course` | Danh mục môn học và số tín chỉ |
| `CourseOffering` | Lớp học phần mở theo học kỳ, có sĩ số tối đa |
| `Enrollment` | Liên kết sinh viên – lớp học phần, trạng thái đăng ký và điểm thành phần |
| `User` | Thông tin xác thực, vai trò và khóa tham chiếu hồ sơ |
| `AuditLogger` | Ghi nhật ký hành động thành công theo thời gian |

# 3. Mô hình dữ liệu

## 3.1 Thực thể Student

| Trường | Kiểu | Ý nghĩa |
|---|---|---|
| `id` | string | Mã sinh viên, khóa chính |
| `name` | string | Họ tên sinh viên |
| `age` | int | Tuổi, phải nằm trong khoảng 1–120 khi tạo mới |
| `course` | string | Thông tin chương trình/ngành học hiện tại |
| `status` | string | `active` hoặc `inactive`; mặc định là `active` |

Tệp lưu trữ: `data/students.csv`.

```csv
id,name,age,course,status
101,Rishi Sharma,20,BCA Cyber Security,active
```

## 3.2 Thực thể Course

| Trường | Kiểu | Ý nghĩa |
|---|---|---|
| `id` | string | Mã môn học, khóa chính |
| `name` | string | Tên môn học |
| `credits` | int | Số tín chỉ |

Tệp lưu trữ: `data/courses.csv`.

## 3.3 Thực thể CourseOffering

| Trường | Kiểu | Ý nghĩa |
|---|---|---|
| `id` | string | Mã lớp học phần, khóa chính |
| `course_id` | string | Mã môn học tham chiếu đến Course |
| `semester` | string | Học kỳ mở lớp |
| `capacity` | int | Sĩ số tối đa |

Tệp lưu trữ: `data/offerings.csv`.

## 3.4 Thực thể Enrollment

| Trường | Kiểu | Ý nghĩa |
|---|---|---|
| `id` | string | Mã đăng ký, khóa chính |
| `student_id` | string | Mã sinh viên |
| `offering_id` | string | Mã lớp học phần |
| `status` | string | Trạng thái đăng ký; chỉ `active` được xem là còn hiệu lực |
| `grades` | vector cặp tên/điểm | Điểm thành phần của lần đăng ký |

Tệp lưu trữ: `data/enrollments.csv`. Điểm thành phần được gom trong một cột bằng ký tự `|`; mỗi phần tử dùng ký tự `:`.

```csv
id,student_id,offering_id,status,grades
ENR001,101,OFF001,active,Midterm:8.00|Final:9.00
```

Điểm tổng kết được tính động:

```text
FinalGrade = Midterm × 0.30 + Final × 0.70
```

Điểm chưa nhập được hiểu là `0`.

## 3.5 Thực thể User

| Trường | Kiểu | Ý nghĩa |
|---|---|---|
| `username` | string | Tên đăng nhập, khóa chính |
| `password` | string | Mật khẩu hiện lưu dạng plaintext trong phạm vi bài toán CSV |
| `role` | string | `ADMIN`, `STUDENT` hoặc `TEACHER` |
| `ref_id` | string | ID của thực thể được liên kết; STUDENT liên kết đến `Student.id` |

Tệp lưu trữ: `data/users.csv`.

```csv
username,password,role,ref_id
admin,admin123,ADMIN,none
student101,student123,STUDENT,101
```

Nếu không có người dùng nào sau khi tải tệp, backend tự tạo tài khoản mặc định `admin,admin123,ADMIN,none`.

# 4. Quy tắc dữ liệu và tính toàn vẹn

1. ID của mỗi thực thể phải khác rỗng và duy nhất trong map tương ứng.
2. Sinh viên chỉ được tạo với tuổi từ 1 đến 120.
3. Sinh viên `inactive` không xuất hiện trong danh sách, tìm kiếm, hồ sơ STUDENT hoặc đăng ký mới.
4. Enrollment `active` là enrollment hợp lệ để kiểm tra trùng lặp, sĩ số và hiển thị cho STUDENT.
5. Một sinh viên chỉ có một enrollment `active` cho cùng một `offering_id`.
6. Số enrollment `active` không được vượt quá `CourseOffering.capacity`.
7. Điểm Midterm và Final phải nằm trong khoảng 0–10.
8. Tất cả các bản ghi, kể cả `inactive`, được giữ khi ghi CSV để bảo toàn lịch sử.

# 5. Workflow xác thực và phân quyền

## 5.1 Đăng nhập

1. Hệ thống nạp `users.csv` vào map `users` với khóa `username`.
2. Người dùng nhập username và password.
3. Backend tìm username trong map.
4. Nếu username tồn tại và mật khẩu khớp, backend trả về đối tượng `User` của phiên hiện tại.
5. Backend lưu `currentUsername`, ghi audit action `LOGIN`, rồi hiển thị menu theo role.
6. Nếu sai, người dùng có tối đa ba lần thử.

## 5.2 ADMIN menu

| Chức năng | Quy tắc nghiệp vụ |
|---|---|
| Thêm sinh viên | Kiểm tra ID duy nhất, tên/ngành không rỗng và tuổi hợp lệ |
| Xem sinh viên | Chỉ hiển thị `Student.status = active` |
| Tìm sinh viên | Chỉ tìm và hiển thị sinh viên active |
| Vô hiệu hóa sinh viên | Chuyển status thành `inactive`, không xóa map/CSV |
| Lưu CSV | Ghi toàn bộ data map xuống năm tệp CSV |
| Thống kê | Đếm sinh viên active và số bản ghi các nhóm dữ liệu |
| Nhập/sửa điểm | Cập nhật Midterm/Final theo Enrollment ID active |

## 5.3 STUDENT menu

| Chức năng | Quy tắc nghiệp vụ |
|---|---|
| Xem hồ sơ | Đọc `Student` bằng `User.ref_id`; hồ sơ phải active |
| Xem lớp học phần | Hiển thị các `CourseOffering` đang được nạp |
| Đăng ký lớp học phần | Kiểm tra transaction trước khi commit |
| Xem đăng ký | Chỉ xem enrollment active của `ref_id`, gồm tên môn và các điểm |

# 6. Workflow quản lý sinh viên và xóa mềm

## 6.1 Tạo sinh viên

```text
ADMIN nhập ID, tên, tuổi, ngành
  -> Kiểm tra ID trống/trùng
  -> Kiểm tra tuổi và trường bắt buộc
  -> Thêm Student(status=active) vào students map
  -> Đánh dấu dataModified
  -> Ghi CREATE_STUDENT vào audit.log
```

## 6.2 Vô hiệu hóa sinh viên

```text
ADMIN nhập Student ID
  -> Tìm Student trong students map
  -> Chỉ chấp nhận Student.status = active
  -> Đổi status thành inactive
  -> Không gọi erase(), không xóa dòng CSV
  -> Đánh dấu dataModified
  -> Ghi DEACTIVATE_STUDENT vào audit.log
```

Hệ quả: lịch sử enrollment và thông tin sinh viên còn nguyên trong CSV, nhưng các màn hình người dùng thông thường không còn hiển thị hồ sơ này.

# 7. Workflow điểm thành phần

## 7.1 Nhập/sửa điểm

1. ADMIN nhập Enrollment ID.
2. Backend xác nhận enrollment tồn tại và `status=active`.
3. ADMIN nhập Midterm và Final, mỗi điểm từ 0 đến 10.
4. Backend dùng `setGrade()` để cập nhật hoặc thêm cặp `Midterm` và `Final`.
5. `calculateFinalGrade()` tính điểm tổng kết theo trọng số 30/70.
6. Dữ liệu được đánh dấu đã thay đổi và audit ghi `UPDATE_GRADE`.

## 7.2 Xem điểm của STUDENT

1. Backend lọc enrollment có `student_id = currentUser.ref_id` và `status=active`.
2. Từ enrollment tìm `CourseOffering`, sau đó tìm `Course` để lấy tên môn.
3. Hiển thị tên môn, Midterm, Final và Final Grade.

Ví dụ: Midterm 8.0 và Final 9.0 cho kết quả `8.7`.

# 8. Workflow đăng ký học phần và Transaction Simulation

## 8.1 Mục tiêu

Đăng ký có nhiều điều kiện. Hệ thống không thay đổi map chính ngay khi người dùng bấm đăng ký, tránh trạng thái dữ liệu dở dang khi validation thất bại.

## 8.2 Chi tiết transaction

```text
STUDENT chọn offering_id
  -> Kiểm tra tài khoản có Student active qua ref_id
  -> Kiểm tra offering tồn tại
  -> Tạo deep copy: temp_enrollments = enrollments
  -> Kiểm tra đăng ký trùng trong temp_enrollments
       -> Có trùng: rollback (bỏ temp), báo lỗi
  -> Đếm enrollment active của offering trong temp_enrollments
       -> Đủ capacity: rollback (bỏ temp), báo lỗi
  -> Tạo Enrollment mới trong temp_enrollments
  -> Commit: enrollments = temp_enrollments
  -> saveAllData()
  -> Thành công: ghi ENROLL_COURSE vào audit.log
```

Enrollment mới được sinh mã theo dạng `ENR-<số>`, đảm bảo không đụng mã đang tồn tại trong map trước khi commit.

# 9. Workflow lưu và nạp CSV

## 9.1 Nạp dữ liệu

Khi chương trình khởi động, `loadAllData()` nạp lần lượt:

1. `students.csv`
2. `courses.csv`
3. `offerings.csv`
4. `enrollments.csv`
5. `users.csv`

Mỗi hàm nạp bỏ qua dòng header nếu đúng định dạng. Bản ghi hợp lệ được parse thành entity tương ứng và đưa vào unordered_map.

## 9.2 Lưu dữ liệu

`saveAllData()` ghi toàn bộ map ra các tệp CSV với header. Cả bản ghi active và inactive đều được ghi. Vì container là `unordered_map`, thứ tự dòng sau khi lưu không được đảm bảo; khóa và nội dung nghiệp vụ không thay đổi do việc sắp xếp lại này.

# 10. Audit Logging

Tệp: `data/audit.log`.

Mỗi dòng có định dạng:

```text
[YYYY-MM-DD HH:MM:SS] | User: <username> | Action: <action> | Details: <details>
```

| Action | Thời điểm ghi |
|---|---|
| `LOGIN` | Đăng nhập thành công |
| `CREATE_STUDENT` | Tạo Student thành công |
| `DEACTIVATE_STUDENT` | Chuyển Student sang inactive thành công |
| `UPDATE_GRADE` | Cập nhật điểm Enrollment thành công |
| `ENROLL_COURSE` | Enrollment đã commit và lưu CSV thành công |

Logger mở tệp bằng `std::ios::app`, do đó log mới được nối vào cuối tệp thay vì ghi đè. `audit.log` được đưa vào `.gitignore` vì là dữ liệu vận hành phát sinh theo môi trường.

# 11. Kịch bản nghiệm thu end-to-end

Tệp `test_input.txt` mô phỏng một lần chạy có hai phiên đăng nhập.

1. ADMIN đăng nhập bằng `admin/admin123`.
2. ADMIN xem sinh viên active.
3. ADMIN cập nhật điểm Enrollment `ENR001`: Midterm 8.0, Final 9.0.
4. ADMIN đăng xuất.
5. STUDENT `student101/student123` đăng nhập.
6. STUDENT xem enrollment và điểm tổng kết 8.7.
7. STUDENT đăng xuất; chương trình đóng và dữ liệu được lưu.

Kết quả kỳ vọng:

- `enrollments.csv` có `Midterm:8.00|Final:9.00` cho `ENR001`.
- `audit.log` có hai LOGIN và một UPDATE_GRADE.
- STUDENT chỉ nhìn thấy STUDENT menu, không thấy quyền ADMIN.

# 12. Giới hạn hiện tại và hướng phát triển

1. CSV parser hiện dùng ký tự phân tách `,`, `|` và `:`; các giá trị dữ liệu không nên chứa các ký tự này.
2. Mật khẩu trong CSV đang là plaintext theo phạm vi bài toán; hệ thống triển khai thực tế cần hash và salt mật khẩu.
3. TEACHER chưa có menu nghiệp vụ riêng.
4. Course và CourseOffering đã có mô hình/lưu trữ, nhưng ADMIN menu hiện chưa có CRUD trực tiếp cho hai thực thể này.
5. Ghi CSV toàn phần chưa bảo đảm tính nguyên tử cấp hệ điều hành; phiên bản nâng cao nên ghi file tạm rồi đổi tên sau khi thành công.
6. Có thể bổ sung hủy đăng ký (soft delete enrollment), giới hạn tín chỉ và báo cáo thống kê chi tiết theo học kỳ.

# 13. Tiêu chí nghiệm thu backend

| Tiêu chí | Trạng thái |
|---|---|
| Build C++17 với Makefile/build.bat | Đạt |
| Không dùng database hoặc thư viện runtime ngoài | Đạt |
| Dữ liệu Student/Course/Offering/Enrollment lưu CSV | Đạt |
| Tra cứu theo ID bằng unordered_map | Đạt |
| Xóa mềm Student | Đạt |
| RBAC ADMIN/STUDENT | Đạt |
| Composite grade 30/70 | Đạt |
| Transaction commit/rollback cho enrollment | Đạt |
| Audit log theo user và thời gian | Đạt |
