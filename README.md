---

````markdown
# JPEG Encoder from YUV420 using libjpeg

This project is a simple JPEG encoder written in C++ that takes raw YUV420p image data and compresses it into a JPEG image using the `libjpeg` library.

## 📸 Features

- Supports YUV 4:2:0 planar format (Y, U, V planes separately)
- Converts YUV to RGB manually using standard formulas
- Compresses the RGB data into a JPEG file using libjpeg
- Lightweight and dependency-free except for `libjpeg`

## 🛠️ Requirements

- Linux (or any POSIX system)
- `g++` or compatible C++ compiler
- `libjpeg-dev`

Install dependencies (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install libjpeg-dev g++
````

## 🧪 Generate a Sample Input

Use `ffmpeg` to convert an image to raw YUV420:

```bash
ffmpeg -i input.jpg -pix_fmt yuv420p -s 640x480 -f rawvideo input.yuv
```

## 🚀 Build

```bash
g++ -o yuv_to_jpeg yuv_to_jpeg.cpp -ljpeg
```

## 🖼️ Run

```bash
./yuv_to_jpeg
```

* It reads `input.yuv` (YUV420 planar)
* Converts to RGB
* Saves the output as `output.jpg`

## 📂 File Format Assumptions

The `input.yuv` file is expected to be in **YUV420 planar format**, with the following layout:

* Y plane: width × height bytes
* U plane: (width/2) × (height/2) bytes
* V plane: (width/2) × (height/2) bytes

Total size = width × height × 1.5 bytes

## 📎 Sample Output

Output is a valid JPEG image written using `libjpeg`.

## 🧰 Project Structure

```
JPEG_Encoder/
├── yuv_to_jpeg.cpp        # Main C++ source file
├── input.yuv              # Sample raw YUV420 image (not committed)
├── output.jpg             # Encoded JPEG image
└── README.md              # Project documentation
```

