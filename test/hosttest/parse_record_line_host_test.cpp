#include "parse_record_line.h"
#include <cstdio>
#include <cstdlib>

static void Expect(bool cond, const char *msg)
{
    if (!cond) {
        std::fprintf(stderr, "FAIL: %s\n", msg);
        std::exit(1);
    }
}

int main()
{
    RecordPoint p;
    Expect(ParseRecordLine("10,20,1", p) && p.xPosi == 10 && p.yPosi == 20 && p.interval == 1, "ok");
    Expect(ParseRecordLine("-3,0,400", p) && p.xPosi == -3 && p.yPosi == 0 && p.interval == 400, "neg");
    Expect(!ParseRecordLine("10,20", p), "short");
    Expect(!ParseRecordLine("10,abc,1", p), "junk");
    Expect(!ParseRecordLine("10,20,2147483648", p), "overflow");
    Expect(!ParseRecordLine("9999999999999999999,1,1", p), "huge");
    Expect(!ParseRecordLine("", p), "empty");
    std::puts("ok");
    return 0;
}
