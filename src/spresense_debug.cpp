#include "spresense_debug.h"
#include <Arduino.h>
#include <stdarg.h> // Cスタイルの可変長引数を扱うために必要

#define BAUDRATE  (115200)

// Serial2 を初期化する
void spresense_debug_init(void)
{
    Serial2.begin(BAUDRATE);
    // Serial2.println() はC++の関数なので、.cppファイル内でのみ使える
    Serial2.println("Spresense Debug Serial Initialized.");
}

// C言語から呼び出される printf の本体
int spresense_printf(const char *format, ...) {
    char buffer[256]; // 一時的なバッファ
    va_list args;
    
    va_start(args, format);
    // vsnprintf はバッファに文字列を書き込み、
    // 書き込むべきだった文字数を int で返す
    int ret = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (ret > 0) {
        // バッファに作られた文字列を Serial2 に書き込む
        Serial2.printf("%s\r", buffer);
    }

    // printf の仕様通り、文字数を返す
    return ret == 0 ? 0 : ret + 1;
}
