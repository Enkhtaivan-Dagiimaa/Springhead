// wrapper.cs
//
using System;
using System.Runtime.InteropServices;

namespace SprCs {
    // std::vector
    //  int
    public class vectorwrapper_int {
        private IntPtr _ptr;
        public vectorwrapper_int(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_int() {}
        ~vectorwrapper_int() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_int(_ptr); }
        public void push_back(int value) { SprExport.Spr_vector_push_back_int(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_int(_ptr); }
        public int this[int index] {
            get { return (int) SprExport.Spr_vector_get_int(_ptr, index); }
            set { SprExport.Spr_vector_set_int(_ptr, index, value); }
        }
    }
    //  unsigned int
    public class vectorwrapper_unsigned_int {
        private IntPtr _ptr;
        public vectorwrapper_unsigned_int(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_unsigned_int() {}
        ~vectorwrapper_unsigned_int() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_unsigned_int(_ptr); }
        public void push_back(uint value) { SprExport.Spr_vector_push_back_unsigned_int(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_unsigned_int(_ptr); }
        public uint this[int index] {
            get { return (uint) SprExport.Spr_vector_get_unsigned_int(_ptr, index); }
            set { SprExport.Spr_vector_set_unsigned_int(_ptr, index, value); }
        }
    }
    //  size_t
    public class vectorwrapper_size_t {
        private IntPtr _ptr;
        public vectorwrapper_size_t(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_size_t() {}
        ~vectorwrapper_size_t() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_size_t(_ptr); }
        public void push_back(ulong value) { SprExport.Spr_vector_push_back_size_t(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_size_t(_ptr); }
        public ulong this[int index] {
            get { return (ulong) SprExport.Spr_vector_get_size_t(_ptr, index); }
            set { SprExport.Spr_vector_set_size_t(_ptr, index, value); }
        }
    }
    //  float
    public class vectorwrapper_float {
        private IntPtr _ptr;
        public vectorwrapper_float(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_float() {}
        ~vectorwrapper_float() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_float(_ptr); }
        public void push_back(float value) { SprExport.Spr_vector_push_back_float(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_float(_ptr); }
        public float this[int index] {
            get { return (float) SprExport.Spr_vector_get_float(_ptr, index); }
            set { SprExport.Spr_vector_set_float(_ptr, index, value); }
        }
    }
    //  double
    public class vectorwrapper_double {
        private IntPtr _ptr;
        public vectorwrapper_double(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_double() {}
        ~vectorwrapper_double() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_double(_ptr); }
        public void push_back(double value) { SprExport.Spr_vector_push_back_double(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_double(_ptr); }
        public double this[int index] {
            get { return (double) SprExport.Spr_vector_get_double(_ptr, index); }
            set { SprExport.Spr_vector_set_double(_ptr, index, value); }
        }
    }
    //  string
    public class vectorwrapper_string {
        private IntPtr _ptr;
        public vectorwrapper_string(IntPtr ptr) { _ptr = ptr; }
        protected vectorwrapper_string() {}
        ~vectorwrapper_string() {}
        public IntPtr get() { return _ptr; }
        public int size() { return (int) SprExport.Spr_vector_size_string(_ptr); }
        public void push_back(string value) { SprExport.Spr_vector_push_back_string(_ptr, value); }
        public void clear() { SprExport.Spr_vector_clear_string(_ptr); }
        public string this[int index] {
            get {
                IntPtr ptr = SprExport.Spr_vector_get_string(_ptr, index);
                string str = Marshal.PtrToStringBSTR(ptr);
                return str;
            }
            set {
                IntPtr pbstr = Marshal.StringToBSTR(value);
                SprExport.Spr_vector_set_string(_ptr, index, pbstr);
            }
        }
    }

    // array
    //  int
    public class arraywrapper_int {
        private IntPtr _ptr;
        public arraywrapper_int(IntPtr ptr) { _ptr = ptr; }
        protected arraywrapper_int() {}
        ~arraywrapper_int() {}
        public IntPtr get() { return _ptr; }
        public int this[int index] {
            get { return (int) SprExport.Spr_array_get_int(_ptr, index); }
            set { SprExport.Spr_array_set_int(_ptr, index, value); }
        }
    }
    //  float
    public class arraywrapper_float {
        private IntPtr _ptr;
        public arraywrapper_float(IntPtr ptr) { _ptr = ptr; }
        protected arraywrapper_float() {}
        ~arraywrapper_float() {}
        public IntPtr get() { return _ptr; }
        public float this[int index] {
            get { return (float) SprExport.Spr_array_get_float(_ptr, index); }
            set { SprExport.Spr_array_set_float(_ptr, index, value); }
        }
    }
    //  double
    public class arraywrapper_double {
        private IntPtr _ptr;
        public arraywrapper_double(IntPtr ptr) { _ptr = ptr; }
        protected arraywrapper_double() {}
        ~arraywrapper_double() {}
        public IntPtr get() { return _ptr; }
        public double this[int index] {
            get { return (double) SprExport.Spr_array_get_double(_ptr, index); }
            set { SprExport.Spr_array_set_double(_ptr, index, value); }
        }
    }
}

