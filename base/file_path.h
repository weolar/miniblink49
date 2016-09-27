
#ifndef base_file_path_h
#define base_file_path_h

// Macros for string literal initialization of FilePath::CharType[], and for
// using a FilePath::CharType[] in a printf-style format string.
#if defined(OS_POSIX)
#define FILE_PATH_LITERAL(x) x
#define PRFilePath "s"
#define PRFilePathLiteral "%s"
#elif defined(OS_WIN)
#define FILE_PATH_LITERAL(x) L ## x
#define PRFilePath "ls"
#define PRFilePathLiteral L"%ls"
#endif  // OS_WIN

#endif // base_file_path_h
