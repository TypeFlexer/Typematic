// Tests for issue #9 which was encountered while reverting vsftpd netstr.h
// https://github.com/correctcomputation/C3/issues/9

typedef _Ptr<int (_Ptr<struct vsf_session> p_sess, _Array_ptr<char> : count(l), unsigned int l)> str_netfd_read_t;
typedef _Ptr<int (_Array_ptr<char> : count(1))> a;
typedef _Ptr<int (_Array_ptr<char> p)> b;
typedef _Ptr<int (_Array_ptr<char>)> c;
