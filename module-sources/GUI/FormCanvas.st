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
drawString: aString inRectangle: aRectangle color: aColor
    self drawString: aString inRectangle: aRectangle font: FontFace default color: aColor
].


FormCanvas ![
drawString: aString inRectangle: aRectangle font: aFontFace color: aColor
    self drawString: aString from: 1 to: aString size inRectangle: aRectangle font: aFontFace color: aColor
].

FormCanvas ![
drawString: aString from: startIndex to: stopIndex inRectangle: aRectangle font: aFontFace color: aColor
    | initialBaseline | 
    initialBaseline := (aRectangle origin x asFloat) @ (aRectangle origin y asFloat + aFontFace ascent).
    self drawString: aString from: startIndex to: stopIndex at: initialBaseline font: aFontFace color: aColor
].

FormCanvas ![
translucentImage: aForm at: aPoint
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox
].

FormCanvas ![
translucentImage: aForm at: aPoint color: aColor
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox color: aColor
].

FormCanvas ![
translucentImage: aForm at: aPoint sourceRect: sourceRect
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox color: Color white
].

FormCanvas ![
setForm: aForm
    form := aForm
].

FormCanvas ![
withTranslation: translation do: aBlock
    | oldOrigin |
    oldOrigin := origin.
    origin := origin + translation.
    aBlock ensure: [
        origin := oldOrigin.
    ].
].

FormCanvas ![
scaleFactor
    ^ scaleFactor
].

FormCanvas ![
scaleFactor: aPoint
    scaleFactor := aPoint
].