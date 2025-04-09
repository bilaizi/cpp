//reinterpret_cast.cpp
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>

int fun() {
    printf("fun\r\n");
    return 42;
}

int main()
{
    float f = 3.14f;

    // initialize pointer to an int with float address
    auto a = reinterpret_cast<int*>(&f);
    printf("a = %d\r\n", *a);

    // the above is same as:
    a = static_cast<int*>(static_cast<void*>(&f));
    printf("a = %d\r\n", *a);

    // casting back to float pointer
    auto fptr = reinterpret_cast<float*>(a);
    printf("f = %.2f\r\n", *fptr);

    // converting a pointer to integer
    auto int_val = reinterpret_cast<std::uintptr_t>(fptr);
    printf("Address of float f is 0x%8lX\r\n", int_val);

    auto fun_void_ptr = reinterpret_cast<void(*)()>(fun);
    // undefined behavior
    fun_void_ptr();

    auto fun_int_ptr = reinterpret_cast<int(*)()>(fun);
    // safe call
    printf("fun_int_ptr returns %d\r\n", fun_int_ptr());
    
    while(true)
    {
    }
}

//strong_types.cpp
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
namespace {
constexpr uint8_t c_PUBLIC_ADDR = 0x01;
constexpr uint16_t c_SUPERV_TIMEOUT = 0x0C80;

class time {
private:
	uint16_t time_in_ms_;
public:
	explicit time(uint16_t time_in_ms) : time_in_ms_(time_in_ms){}
	uint16_t & get_ms() {return time_in_ms_;}
};

time operator""_ms(unsigned long long t) {
    return time(t);
}

class conn_l {
private:
	uint16_t val_; 
public:
	explicit conn_l(time t) : val_(t.get_ms()/0.625f){}
	uint16_t & get() {return val_;}
};

class conn_p {
private:
	uint16_t val_; 
public:
	explicit conn_p(time t) : val_(t.get_ms()/1.25f){}
	uint16_t & get() {return val_;}
};


using tBleStatus = bool;

tBleStatus aci_gap_create_connection_wrapper( 
                            conn_l LE_Scan_Interval, 
                            conn_l LE_Scan_Window, 
                            uint8_t Peer_Address_Type, 
                            uint8_t Peer_Address[6], 
                            conn_p Conn_Interval_Min, 
                            conn_p Conn_Interval_Max, 
                            time Conn_Latency, 
                            time Supervision_Timeout, 
                            conn_l Minimum_CE_Length, 
                            conn_l Maximum_CE_Length) {
    static_cast<void>(LE_Scan_Interval);
    static_cast<void>(LE_Scan_Window);
    static_cast<void>(Peer_Address_Type);
    static_cast<void>(Peer_Address);
    static_cast<void>(Conn_Interval_Min);
    static_cast<void>(Conn_Interval_Max);
    static_cast<void>(Conn_Latency);
    static_cast<void>(Supervision_Timeout);
    static_cast<void>(Minimum_CE_Length);
    static_cast<void>(Maximum_CE_Length);

    return true;
}
}

int main()
{
    const tBleStatus stat = aci_gap_create_connection_wrapper(
            conn_l(80_ms),
            conn_l(120_ms),
            c_PUBLIC_ADDR,
            nullptr,
            conn_p(50_ms),
            conn_p(60_ms),
            0_ms,
            4000_ms,
            conn_l(10_ms),
            conn_l(15_ms)
    );
    static_cast<void>(stat);
    while(true)
    {
    }
}

//type_punning.cpp
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
#include <bit>

namespace {
struct my_struct {
    int a;
    char c;
};

void print_my_struct (const my_struct & str) {
    printf("a = %d, c = %c\r\n", str.a, str.c);
}

void process_data(const char * data) {
    const auto *pstr = reinterpret_cast<const my_struct *>(data);

    printf("%s\r\n", __func__);
    print_my_struct(pstr[0]);
    print_my_struct(pstr[1]);
}

void process_data_memcpy(const char * data) {
    my_struct my_structs[2];
    std::memcpy(my_structs, data, sizeof(my_structs));

    printf("%s\r\n", __func__);
    print_my_struct(my_structs[0]);
    print_my_struct(my_structs[1]);
}
};

int main()
{
    int i = 42;
    auto * i_ptr = reinterpret_cast<char*>(&i);

    if (i_ptr[0]==42) {
        printf("Little endian!\r\n");
    } else {
        printf("Big endian!\r\n");
    }

    my_struct my_structs_arr[] = {{4, 'a'}, {5, 'b'}};
    char arr[128];
    std::memcpy(&arr, my_structs_arr, sizeof(my_structs_arr));

    process_data(arr);
    process_data_memcpy(arr);

    while(true)
    {
    }
}

//usual_arithmetic_conversion.cpp
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>

struct my_struct {
    char c;
    int a;
};

void print_my_struct (const my_struct & str) {
    printf("c = %d, a = %d\r\n", str.c, str.a);
}

int main()
{
#if 0
    char c;
    char arr[128];
    arr[0] = 'A';
    my_struct str{'X', 42};

    constexpr size_t offset = 3;

    memcpy(&arr[offset], &str, sizeof(my_struct));

    auto *pstr = reinterpret_cast<my_struct *>(&arr[offset]);
    printf("arr[0] = %d\r\n", arr[0]);
    print_my_struct(str);
    print_my_struct(*pstr);
#endif
    struct bitfield{ 
        long long a:31;  
    }; 
    bitfield b {4}; 
    int c = 1; 
    auto res1 = b.a + c;    
    static_assert(sizeof(int) == 4); 
    static_assert(sizeof(long long) == 8); 
    static_assert(std::is_same_v<int, decltype(res1)>);  

    long e = 5;  
    auto res2 = e - b.a;  
    static_assert(std::is_same_v<long, decltype(res2)>);  
    printf("res2 = %ld\r\n", res2);
    while(true)
    {
    }
}
