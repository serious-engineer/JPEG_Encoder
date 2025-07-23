# JPEG Encoder & Decoder (YUV420P ↔ JPEG)

This project implements a **minimal JPEG image encoder and decoder** in C++, designed to help understand raw image formats and compression workflows. It manually handles **YUV420 planar data**, converts it to RGB, compresses using `libjpeg`, and supports round-trip decoding.

---

## 🔧 Features

- ✅ Read raw `.yuv` (YUV420P) format
- ✅ Convert YUV → RGB manually
- ✅ Encode RGB → JPEG using `libjpeg`
- ✅ Decode JPEG → RGB → YUV420P
- ✅ Save reconstructed `.yuv` file
- ✅ Compatible with `ffplay` and `ffmpeg` for viewing

---

## 🛠 Dependencies

- C++11 or later
- [`libjpeg-dev`](https://libjpeg.sourceforge.net/)
- [`ffmpeg`](https://ffmpeg.org/) *(optional, for viewing)*

### 🐧 Install on Ubuntu:

```bash
sudo apt update
sudo apt install build-essential libjpeg-dev ffmpeg
```

---

## 🚀 Build Instructions

```bash
g++ main.cpp -o jpeg_tool -ljpeg
```

---

## 📦 Usage

### Encode YUV → JPEG

```bash
./jpeg_tool encode input.yuv output.jpg 640 480
```

### Decode JPEG → YUV

```bash
./jpeg_tool decode input.jpg output.yuv 640 480
```

- Replace `640` and `480` with your image width and height.

---

## 👁️ View YUV Output

Using `ffplay`:

```bash
ffplay -f rawvideo -video_size 640x480 -pixel_format yuv420p output.yuv
```

Or convert to PNG:

```bash
ffmpeg -f rawvideo -pixel_format yuv420p -video_size 640x480 -i output.yuv frame.png
```

---

## 📁 File Format Assumptions

- `input.yuv` is in **YUV420P (planar)** format:
  ```
  [Y plane][U plane][V plane]
  ```
- Width and height must be specified manually (no header parsing)
- JPEG quality is fixed (can be extended to add as argument)

---

## 🙌 Author

**Krishnaprasad Sreekumar Nair**  

---

## 📜 License

MIT License. Contributions and forks are welcome!
