# 🔧 C Stuff

This repository contains C programs developed to extract strings resembling URLs from binary files. These tools are particularly useful for analyzing binaries to uncover embedded web addresses or similar patterns.

---

## 📂 Repository Contents

- **`lstrings.c`**: A C program that scans binary files to extract strings that resemble URLs.
- **`tlstrings.c`**: An enhanced version of `lstrings.c` with additional features or optimizations.

---

## 🛠️ Setup and Compilation

To compile the programs, follow these steps:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/ltdenard/c_stuff.git
   cd c_stuff
   ```

2. **Compile the Programs**:

   Use `gcc` to compile each C file:

   ```bash
   gcc -o lstrings lstrings.c
   gcc -o tlstrings tlstrings.c
   ```

   This will generate executable files named `lstrings` and `tlstrings`.

---

## 🚀 Usage

After compilation, you can use the tools as follows:

```bash
./lstrings <binary_file>
```

or

```bash
./tlstrings <folder_with_binary_files>
```

Replace `<binary_file>` with the path to the binary file you wish to analyze. The programs will output strings that resemble URLs found within the specified binary file.

---

## 🧠 Notes

- Ensure you have the necessary permissions to read the binary files you are analyzing.
- The accuracy of the extracted strings depends on the heuristics implemented in the programs to identify URL-like patterns.

---

## 🤝 Contributions

Contributions to enhance the functionality or performance of these tools are welcome. Feel free to fork the repository and submit pull requests with your improvements.

---

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

---

> **Disclaimer**: Use these tools responsibly and ensure you have the right to analyze the binary files in question.
