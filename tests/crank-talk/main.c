#include "crank-talk/object-model.h"

void analysisAndEvaluation_runTests(void);
void string_runTests(void);
void scanner_runTests(void);
void parser_runTests(void);

int main()
{
    string_runTests();
    scanner_runTests();
    parser_runTests();
    analysisAndEvaluation_runTests();
    return 0;
}
