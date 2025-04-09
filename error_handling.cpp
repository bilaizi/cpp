//assert.cpp
#include <cstdint>
#include <cstdio>

void log_pc_and_halt(std::uint32_t pc) {
    printf("Assert at 0x%08lX\r\n", pc);
    while(true) {}
}

#define light_assert(expr)         \
        (static_cast<bool> (expr)  \
        ? void (0)                 \
        : log_pc_and_halt(10000000)\
        )

enum class option : std::uint8_t {
    Option1 = 0, 
    Option2, 
    Option3,
    Last
};
    
option uint8_to_option(uint8_t num) {
    light_assert(num < static_cast<uint8_t>(option::Last));
    return static_cast<option>(num);
}

int main()
{
    printf("Assert example\r\n");
    const option opt = uint8_to_option(3);
    while(true)
    {
    }
}

//exceptions.cpp
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <exception>
#include <stdexcept>

template <class T, std::size_t N> 
struct ring_buffer {
  std::array<T, N> arr;
  std::size_t write_idx = 0; 

  void push(T t) {
    arr.at(write_idx++) = t;
  }
};

int main()
{
    printf("Exceptions example\r\n");

    std::set_terminate([]() {
        printf("My terminate handler!\r\n");
        while(true){}
    });

    std::array<int, 4> arr;

    // uncomment the following line to trigger terminate handler
    // arr.at(5) = 6;

    try {
      arr.at(5) = 6;
    } catch(std::out_of_range &e) {
      printf("Array out of range!\r\n");
    } catch (...) {
      printf("Unexpected expection...\r\n");
    }

    ring_buffer<int, 4> rb;
    try {
      for(int i = 0; i < 6; i++) {
        rb.push(i);
      }
    } catch(std::out_of_range &e) {
      printf("Ring buffer out of range!\r\n");
    } 
    while(true)
    {
    }
}

//expected.cpp
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <expected>

struct ble_light_bulb {
    enum class error {
        disconnected,
        timeout
    };
    struct config {
        int r;
        int g;
        int b;
    };

    bool ret_val;
    std::expected<config, error> get_config() {
        ret_val = !ret_val;
        if (ret_val) {
            return config {10, 20, 30};
        } else {
            return std::unexpected(error::timeout);
        }
    }
};

int main()
{
    printf("std::expected example\r\n");
    ble_light_bulb bulb;
    const auto get_config_from_main = [&bulb]() {
        auto result = bulb.get_config();
        if (result.has_value()) {
            auto conf = result.value();
            printf("Config r %d, g %d, b %d\r\n", conf.r, conf.g, conf.b);
        } else {
            auto err = result.error();
            if (err == ble_light_bulb::error::disconnected) {
                printf("The bulb is disconnected! \r\n");
            } else if (err == ble_light_bulb::error::timeout) {
                printf("Timeout!\r\n");
            }
        }
    };

    get_config_from_main();
    get_config_from_main();

    while(true)
    {
    }
}

//optional.cpp
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>

struct sensor {
    struct data {
        int x;
        int y;
    };
    static inline bool ret_val = true;
    static std::optional<data> get_data() {
        ret_val = !ret_val;
        if (ret_val) {
            return data{4, 5};
        } else {
            return std::nullopt;
        }
    }
};

int main()
{
    printf("std::optional example\r\n");

    const auto get_data_from_main = [] () {
      auto result = sensor::get_data();
      if (result) {
          printf("x = %d, y = %d\r\n", (*result).x, (*result).y);
      } else {
          printf("No data!\r\n");
      }
    };

    get_data_from_main();
    get_data_from_main();

    while(true)
    {
    }
}
