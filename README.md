# Hệ Thống Chuẩn Hóa Dữ Liệu IoT (JSON & XML)

Dự án này được thiết kế để thu thập, chuẩn hóa và hiển thị dữ liệu từ các thiết bị IoT (như ESP32/Arduino). Hệ thống hỗ trợ cả hai định dạng dữ liệu phổ biến là **JSON** và **XML**, giúp tích hợp dễ dàng với nhiều loại cảm biến và nhóm phát triển khác nhau.

## 📋 Yêu Cầu Dự Án
1. **Chuẩn hóa JSON & XML**: Thiết kế cấu trúc dữ liệu linh hoạt, tự động nhận diện và chuyển đổi cả JSON và XML về một định dạng chuẩn duy nhất.
2. **Thiết kế Schema Hệ Thống**: Hỗ trợ đầy đủ các loại dữ liệu cảm biến: Nhiệt độ (temp), Độ ẩm (hum), Ánh sáng (light), Rung động (vibration)...
3. **API Lưu Trữ & Hiển Thị**: Xây dựng API để lưu trữ dữ liệu vào Database (MongoDB) và cung cấp dữ liệu cho giao diện Web Dashboard.
4. **Giao Diện Trực Quan**: Hiển thị biểu đồ thời gian thực, trạng thái thiết bị và nhật ký hệ thống.

---

## 🚀 Tính Năng Nổi Bật
- **Normalization Engine**: Tự động chuyển đổi các trường dữ liệu khác nhau (ví dụ: `t`, `temp`, `temperature`) về một tên trường chuẩn.
- **Đa Định Dạng**: Hỗ trợ nhận dữ liệu qua MQTT dưới dạng JSON thô hoặc XML.
- **Real-time Dashboard**: Cập nhật biểu đồ và trạng thái LED ngay lập tức khi có dữ liệu mới.
- **Export Data**: Hỗ trợ xuất nhật ký hệ thống ra file CSV để phục vụ báo cáo.
- **Auto/Manual Mode**: Cho phép điều khiển thiết bị tự động dựa trên ngưỡng cảm biến hoặc điều khiển thủ công từ Web.

---

## 📂 Cấu Trúc Thư Mục
```text
├── arduino_ide/
│   ├── code_phan_cung/         # Code nạp cho chip ESP32 (C++/Arduino)
│   │   └── doc_dht11_co_mqtt/  # Code đọc cảm biến DHT11 và gửi MQTT
│   └── code_phan_mem/
│       └── HTML/               # Giao diện Web Dashboard (HTML/JS/Tailwind)
├── backend/
│   ├── server.js               # MQTT Consumer: Nhận, chuẩn hóa và lưu DB
│   ├── api.js                  # Express API: Cung cấp dữ liệu cho Frontend
│   ├── package.json            # Quản lý thư viện Node.js
├── run.bat                     # File khởi động nhanh toàn bộ hệ thống (Windows)
└── README.md                   # Tài liệu hướng dẫn dự án
```

---

## 🛠️ Hướng Dẫn Cài Đặt

### 1. Yêu cầu hệ thống
- **Node.js**: Phiên bản 16.x trở lên.
- **MongoDB**: Đã cài đặt và đang chạy tại `localhost:27017`.
- **MQTT Broker**: Dự án sử dụng `broker.emqx.io` (công cộng), bạn có thể đổi sang broker riêng nếu cần.

### 2. Cài đặt thư viện Backend
Mở Terminal tại thư mục `backend` và chạy lệnh:
```bash
npm install
```
Các thư viện chính được sử dụng:
- `express`: Tạo API Server.
- `mongoose`: Giao tiếp với cơ sở dữ liệu MongoDB.
- `mqtt`: Kết nối và nhận dữ liệu từ các thiết bị IoT.
- `fast-xml-parser`: Xử lý và chuyển đổi dữ liệu XML sang JSON.
- `cors`: Cho phép Frontend truy cập API.

---

## 💻 Cách Chạy Dự Án

### Cách 1: Sử dụng file chạy nhanh (Khuyên dùng)
Bạn chỉ cần click đúp vào file `run.bat` ở thư mục gốc. File này sẽ tự động:
1. Khởi động **MQTT Server** (server.js).
2. Khởi động **Express API** (api.js).
3. Mở **Giao diện Web** trên trình duyệt mặc định.

### Cách 2: Chạy thủ công bằng lệnh
1. Chạy Server xử lý dữ liệu:
   ```bash
   cd backend
   node server.js
   ```
2. Chạy API Server:
   ```bash
   node api.js
   ```
3. Mở file `index.html` trong thư mục `arduino_ide/code_phan_mem/HTML/` bằng trình duyệt.

---

## 📊 Định Dạng Dữ Liệu Chuẩn (Schema)

Hệ thống sẽ tự động chuẩn hóa dữ liệu từ các nguồn khác nhau về cấu trúc sau trong MongoDB:

| Trường dữ liệu | Kiểu dữ liệu | Mô tả |
| :--- | :--- | :--- |
| `device_id` | String | ID duy nhất của thiết bị (ví dụ: `esp32_01`) |
| `temperature` | Number | Nhiệt độ (°C) |
| `humidity` | Number | Độ ẩm (%) |
| `light` | Number | Cường độ ánh sáng (Lux) |
| `vibration` | Number | Giá trị rung động |
| `led_temp_state` | Boolean | Trạng thái LED nhiệt độ (Bật/Tắt) |
| `led_hum_state` | Boolean | Trạng thái LED độ ẩm (Bật/Tắt) |
| `mode` | String | Chế độ hoạt động (`auto` hoặc `manual`) |
| `timestamp` | Number | Thời gian ghi nhận dữ liệu |

### Ví dụ dữ liệu đầu vào được chấp nhận:
**Dạng JSON:**
```json
{ "id": "dev01", "temp": 28.5, "hum": 65, "led_t": true }
```

**Dạng XML:**
```xml
<sensor>
    <device_id>dev02</device_id>
    <t>30.2</t>
    <h>70</h>
    <light>500</light>
</sensor>
```
*Cả hai dạng trên đều sẽ được lưu trữ đồng nhất vào Database.*

---

## 👨‍💻 Tác giả
Dự án được thực hiện cho môn học: **Thiết bị ngoại vi và Kỹ thuật ghép nối**.
Chúc bạn hiểu rõ và vận hành tốt dự án!
# Thiet_bi_ngoai_vi_Ky_thuat_ghep_noi
