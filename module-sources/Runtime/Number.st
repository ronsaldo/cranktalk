
"Number primitives. Most of these methods are ported from Pharo."

"Testing methods"
Object ![
isNumber
    ^ false
].

Number ![
isNumber
    ^ true
].

Object ![
isInteger
    ^ false
].

Integer ![
isInteger
    ^ true
].

Object ![
isFraction
    ^ false
].

Fraction ![
isFraction
    ^ true
].

Object ![
isFloat
    ^ false
].

Float ![
isFloat
    ^ true
].

"Number primitives"
Number ![
abs
    ^ self < 0
        ifTrue: [self negated]
        ifFalse: [self]
].

Number ![
max: other
    ^ self < other ifTrue: [other] ifFalse: [self]
].

Number ![
min: other
    ^ self < other ifTrue: [self] ifFalse: [other]
].

Number ![
min: maxValue max: minValue
    ^ (self min: maxValue) max: minValue
].


"Number adaptation"
Number ![
adaptToInteger: receiver andSend: selector
    self subclassResponsibility
].

Float ![
adaptToInteger: receiver andSend: selector
	^ receiver asFloat perform: selector with: self
].

"Number arithmetic"
Number ![
+ other
    self subclassResponsibility
].

"Fraction arithmetic"
Fraction class ![
numerator: numInteger denominator: denInteger    
    ^ self new setNumerator: numInteger denominator: denInteger    
].

Fraction ![
setNumerator: n denominator: d
    d = 0 ifTrue: [ self error: 'Division by zero.'].
    
    numerator := n.
    denominator := d.
].

Fraction ![
numerator
    ^ numerator
].

Fraction ![
denominator
    ^ denominator
].

Fraction ![
reduced
    | gcd n d|
    numerator = 0 ifTrue: [^ 0].
    gcd := numerator gcd: denominator.
    gcd = 1 ifTrue: [^ self].

    n := numerator // gcd.
    d := denominator // gcd.

    denominator = 1 ifTrue: [^ n].
    ^ Fraction numerator: n denominator: d
].

Fraction ![
printOn: aStream
    aStream nextPut: $(.
    aStream print: numerator.
    aStream nextPut: $/.
    aStream print: denominator.
    aStream nextPut: $).
].

"Integer arithmetic"
Integer ![
+ aNumber
    ^ aNumber adaptToInteger: self andSend: #+
].

Integer ![
- aNumber
    ^ aNumber adaptToInteger: self andSend: #-
].

Integer ![
* aNumber
    ^ aNumber adaptToInteger: self andSend: #*
].

Integer ![
/ aNumber
    ^ aNumber adaptToInteger: self andSend: #/
].

Integer ![
// aNumber
    ^ aNumber adaptToInteger: self andSend: #/
].

Integer ![
gcd: anInteger
	| n m |
	n := self.
	m := anInteger.
	[n = 0]
		whileFalse:
			[n := m \\ (m := n)].
	^ m abs
].

"SmallInteger arithmetic"
SmallInteger ![
+ other
    <primitive: #'SmallInteger::+'>
    ^ super + other
].

SmallInteger ![
- other
    <primitive: #'SmallInteger::-'>
    ^ super - other
].

SmallInteger ![
* other
    <primitive: #'SmallInteger::*'>
    ^ super * other
].

SmallInteger ![
// other
    <primitive: #'SmallInteger:://'>
    other = 0 ifTrue: [
        self error: 'Zero divide'
    ].
    ^ super // other
].

SmallInteger ![
\\ other
    <primitive: #'SmallInteger::\\'>
    other = 0 ifTrue: [
        self error: 'Zero divide'
    ].
    ^ super \\ other
].


SmallInteger ![
/ other
    <primitive: #'SmallInteger::/'>
    other = 0 ifTrue: [
        self error: 'Zero divide'
    ].

    ^ (other isMemberOf: SmallInteger)
        ifTrue: [ (Fraction numerator: self denominator: other) reduced]
        ifFalse: [super / other]
].

"
Stdio stdout print: 1 // 2; lf.
Stdio stdout print: 1 \\ 2; lf.

Stdio stdout print: 1 / 2; lf.
Stdio stdout print: 1 / 2.0; lf.
Stdio stdout print: 1.0 / 2; lf.
"