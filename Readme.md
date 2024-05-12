# Usage #

Include `scan.h`. Thats it.

### Simple Usage ###
```cpp
#include <scan.h>
#include <print>

int main()
{
    const std::string data = "Mon, 29 March 2024 08:02:56";
    constexpr std::string_view format{"{}, {} {} {} {}:{}:{}"};
    constexpr int num_args = count_formats(format);

    const auto [a, day, month, year, hour, minute, seconds] 
        = scan<num_args, std::string, int, std::string, int, int, int, int>(data, format);
    std::println("Day: {}\nDate: {} {} {}\nTime: {}:{}:{}", a, day, month, year, hour, minute, seconds);
}
```

```
Day: Mon
Date: 29 March 2024
Time: 8:2:56
```

### Slightly More Advanced Usage ###
```cpp
#include <scan.h>
#include <print>

int main()
{
    /**
        * 9999 FETCH(UID 10000 RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]{4043}
        Date: Thu, 9 May 2024 23:47:03 -0700
        Subject:
        From: Eshwary <eshwaryforreasons@gmail.com
        To: Eshwary <eshwaryforreasons@gmail.com
        
        )
    */

    const std::string data = "* 9999 FETCH(UID 10000 RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]{4043}\r\n"
	"Date: Thu, 9 May 2024 23:47:03 -0700\r\n"
	"Subject: \r\n"
	"From: Eshwary <eshwaryforreasons@gmail.com>\r\n"
	"To: Eshwary <eshwaryforreasons@gmail.com>\r\n"
	"\r\n"
	")";

    constexpr std::string_view format{
        "* {} FETCH(UID {} RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]{{}}\r\n"
        "Date: {}\r\n"
        "Subject: {}\r\n"
        "From: {}\r\n"
        "To: {}\r\n"
        ""
    };
    constexpr int num_args = count_formats(format);

    const auto [sequence_number, uid, size, date, subject, from, to] 
        = scan<num_args, int, int, int, std::string, std::string, std::string, std::string>(data, format);
    std::println("Sequence Number: {}\nUid: {}\nSize: {}\nDate: {}\nSubject: {}\nFrom: {}\nTo: {}", 
        sequence_number, uid, size, date, subject, from, to);
}
```

```
Sequence Number: 9999
Uid: 10000
Size: 4043
Date: Thu, 9 May 2024 23:47:03 -0700
Subject:
From: Eshwary <eshwaryforreasons@gmail.com>
To: Eshwary <eshwaryforreasons@gmail.com>
```