"Shared pool"
SharedPool << #MySharedPool
    slots: #();
    sharedVariables: {#TestSharedPoolVariable};
    build.

MySharedPool class ![
initialize
    TestSharedPoolVariable := 41.
].
MySharedPool initialize.

"Class with shared variable and a shared pool"
Object << #MyClassWithSharedVariable
    slots: #();
    sharedVariables: {#TestClassVar};
    sharedPools: {MySharedPool};
    build.

MyClassWithSharedVariable class ![
initialize
    TestClassVar := 42.
].

MyClassWithSharedVariable initialize.

MyClassWithSharedVariable ![
testClassVar
    ^ TestClassVar
].

MyClassWithSharedVariable ![
testSharedPoolVariable
    ^ TestSharedPoolVariable
].

Stdio stdout print: (MyClassWithSharedVariable new testClassVar); lf.
Stdio stdout print: (MyClassWithSharedVariable new testSharedPoolVariable); lf.