"Small Integer arithmetic"
[
    self assert: 1 negated = -1.
    self assert: (1 + 2) = 3.
    self assert: (1 - 2) = -1.
    self assert: (2*3) = 6.
    self assert: (1//2) = 0.
    self assert: (1\\2) = 1.
] value.

"Float arithmetic"
[
    self assert: 1.0 negated = -1.0.
    self assert: (1.0 + 2.0) = 3.0.
    self assert: (1.0 - 2.0) = -1.0.
    self assert: (2.0 * 3.0) = 6.0.
    self assert: (1.0/2.0) = 0.5.
] value.

"Small integer and float arithmetic"
[
    self assert: (1 + 1.0) = 2.0.
    self assert: (1.0 + 1) = 2.0.

    self assert: (1 - 1.0) = 0.0.
    self assert: (1.0 - 1) = 0.0.
] value.

"Stdio stdout print: 1 // 2; lf.
Stdio stdout print: 1 \\ 2; lf.

Stdio stdout print: 1 / 2; lf.
Stdio stdout print: 1 / 2.0; lf.
Stdio stdout print: 1.0 / 2; lf.
"