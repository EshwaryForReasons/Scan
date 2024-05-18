# Usage #

Include `scan.h`. Thats it.

### Specifiers ###

- {s}: std::string
- {i}: int
- {f}: float
- {d}: double

### Simple Usage ###
```cpp
#include <scan.h>
#include <print>

int main()
{
    const std::string data = "Mon, 29 March 2024 08:02:56";
    const auto [a, day, month, year, hour, minute, seconds] = scan::scan("{s}, {i} {s} {i} {i}:{i}:{i}"_f, data);
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
        * 9999 FETCH(UID 10000 RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]
        Date: Thu, 9 May 2024 23:47:03 -0700
        Subject:
        From: Eshwary <eshwaryforreasons@gmail.com
        To: Eshwary <eshwaryforreasons@gmail.com
        
        )
    */

    const std::string other_data = "* 9999 FETCH(UID 10000 RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]\r\n"
        "Date: Thu, 9 May 2024 23:47:03 -0700\r\n"
        "Subject: \r\n"
        "From: Eshwary <eshwaryforreasons@gmail.com>\r\n"
        "To: Eshwary <eshwaryforreasons@gmail.com>\r\n"
        "\r\n"
        ")";

    constexpr scan::FormatString format{
        "* {i} FETCH(UID {i} RFC822.SIZE 1975220 FLAGS(\\Seen) BODY[HEADER.FIELDS(From To Subject Date)]\r\n"
        "Date: {s}\r\n"
        "Subject: {s}\r\n"
        "From: {s}\r\n"
        "To: {s}\r\n"
        ""
    };

    const auto [sequence_number, uid, date, subject, from, to] = scan::scan<format>(other_data);
    std::println("Sequence Number: {}\nUid: {}\nDate: {}\nSubject: {}\nFrom: {}\nTo: {}", sequence_number, uid, date, subject, from, to);
}
```

```
Sequence Number: 9999
Uid: 10000
Date: Thu, 9 May 2024 23:47:03 -0700
Subject:
From: Eshwary <eshwaryforreasons@gmail.com>
To: Eshwary <eshwaryforreasons@gmail.com>
```
