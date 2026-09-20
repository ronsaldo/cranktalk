Point ![
corner: aCorner
    ^ Rectangle origin: self corner: aCorner
].

Rectangle class ![
origin: anOrigin corner: aCorner
    ^ self basicNew setPoint: anOrigin point: aCorner
].

Rectangle ![
setPoint: anOrigin point: aCorner
    origin := anOrigin.
    corner := aCorner.
].
