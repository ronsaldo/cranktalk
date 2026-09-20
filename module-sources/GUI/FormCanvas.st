FormCanvas class ![
on: aForm
    ^ self new setForm: aForm
].

FormCanvas ![
initialize
    super initialize.
    origin := 0@0.
    scaleFactor := 1@1.
].

FormCanvas ![
setForm: aForm
    form := aForm
].


