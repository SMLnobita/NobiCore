// Apple libc++ 14 headers expect this global alias before partial stddef includes.
#ifdef __cplusplus
typedef decltype(nullptr) nullptr_t;
#endif
