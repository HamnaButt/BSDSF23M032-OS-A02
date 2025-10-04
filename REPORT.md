# Report Questions & Answers

# Feature 1 – Project Setup and Initial Build

**Tasks Completed**
- Created repo BSDSF23M032-OS-A02 on GitHub.
- Cloned repo to Kali.
- Created src, bin, obj, man folders.
- Added starter file ls-v1.0.0.c and Makefile.
- Built executable successfully using make.

**Learning**
- Learned how gcc and make compile C programs.
- Understood directory structure for project builds.
- Practiced basic git commands: clone, add, commit, push, tag.

**Verification**
- make created bin/ls executable.
- ./bin/ls listed files correctly.

# Feature 2 – Long Listing (-l)

### 1. What is the crucial difference between the `stat()` and `lstat()` system calls? In the context of the `ls` command, when is it more appropriate to use `lstat()`?

The crucial difference lies in how they handle **symbolic links**:

- **`stat()`**: Follows a symbolic link and returns information about the **target file** that the link points to.  
  Example: If `file_link` → `real_file.txt`, then `stat("file_link")` shows details of `real_file.txt`.

- **`lstat()`**: Returns information about the **link itself**, not the target.  
  Example: `lstat("file_link")` shows details of `file_link` (file type = symbolic link, its own size/permissions, etc.).

✅ In the context of the `ls` command, **`lstat()` is more appropriate** because `ls -l` must show symbolic links distinctly (with `l` at the start of the permission string and the `-> target` display). Using only `stat()` would make symbolic links appear as normal files.

---

### 2. The `st_mode` field in `struct stat` is an integer that contains both the file type (e.g., regular file, directory) and the permission bits. Explain how you can use bitwise operators (like `&`) and predefined macros (like `S_IFDIR` or `S_IRUSR`) to extract this information.

The field `st_mode` in `struct stat` encodes two types of information in one integer:

1. **File type** (regular file, directory, symbolic link, etc.)  
2. **Permission bits** (read, write, execute for owner, group, and others)

---

#### 📌 File Type Extraction
Use predefined macros with bitwise operations to check type:

```c
if (S_ISDIR(st.st_mode)) {
    printf("This is a directory\n");
}
if (S_ISREG(st.st_mode)) {
    printf("This is a regular file\n");
}

# Feature 3 – Column Display (down-then-across)

### 3. Explain the general logic for printing items in a "down then across" columnar format. Why is a simple single loop through the list of filenames insufficient for this task?

The "down then across" format prints filenames vertically in columns instead of left-to-right row by row. The general steps are:

1. Store all filenames in an array.
2. Find the maximum filename length to determine the column width.
3. Detect the terminal width and calculate:
   - `col_width = maxlen + padding`
   - `ncols = term_width / col_width`
   - `nrows = ceil(nfiles / ncols)`
4. Print with two nested loops:
   - Outer loop → rows
   - Inner loop → columns
   - Index formula: `index = c * nrows + r`
   - Print only if `index < nfiles`.

Example layout:
a d g
b e h
c f i

A simple single loop is insufficient because it only prints items row by row (left to right), not in the required down-then-across column format.

---

### 4. What is the purpose of the `ioctl` system call in this context? What would be the limitations of your program if you only used a fixed-width fallback (e.g., 80 columns) instead of detecting the terminal size?

The `ioctl` system call (with `TIOCGWINSZ`) is used to detect the terminal’s current width in characters. This makes it possible to dynamically calculate how many columns of filenames can fit without breaking alignment.

If only a fixed-width fallback like 80 columns were used:
- On wide terminals → wasted space, fewer columns than possible.
- On narrow terminals → lines may wrap incorrectly, breaking formatting.
- The program would not adjust when the terminal window is resized.

Using `ioctl` ensures the layout adapts to the actual terminal size, giving correct and consistent results.

