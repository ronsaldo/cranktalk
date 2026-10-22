Canvas class ![
on: aForm
    ^ self new setForm: aForm
].

Canvas ![
initialize
    super initialize.
    origin := 0@0.
    scaleFactor := 1@1.
].

Canvas ![
drawString: aString inRectangle: aRectangle color: aColor
    self drawString: aString inRectangle: aRectangle font: FontFace default color: aColor
].


Canvas ![
drawString: aString inRectangle: aRectangle font: aFontFace color: aColor
    self drawString: aString from: 1 to: aString size inRectangle: aRectangle font: aFontFace color: aColor
].

Canvas ![
drawString: aString from: startIndex to: stopIndex inRectangle: aRectangle font: aFontFace color: aColor
    | initialBaseline | 
    initialBaseline := (aRectangle origin x asFloat) @ (aRectangle origin y asFloat + aFontFace ascent).
    self drawString: aString from: startIndex to: stopIndex at: initialBaseline font: aFontFace color: aColor
].

Canvas ![
drawString: aString from: startIndex to: stopIndex at: initialBaseline font: aFontFace color: aColor
    self subclassResponsibility
].

Canvas ![
fillWithColor: aColor
    self subclassResponsibility
].

Canvas ![
fillRectangle: aRectangle color: aColor
    self subclassResponsibility
].

Canvas ![
frameRectangle: aRectangle borderWidth: aBorderWidth color: aColor
    self subclassResponsibility
].

Canvas ![
translucentImage: aForm at: aPoint
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox
].

Canvas ![
translucentImage: aForm at: aPoint color: aColor
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox color: aColor
].

Canvas ![
translucentImage: aForm at: aPoint sourceRect: sourceRect
    self translucentImage: aForm at: aPoint sourceRect: aForm boundingBox color: Color white
].

Canvas ![
translucentImage: aForm at: aPoint sourceRect: sourceRect color: blendColor
    self translucentImage: aForm atRect: (aPoint extent: aForm extent) sourceRect: sourceRect color: blendColor
].

Canvas ![
translucentImage: aForm atRect: aPoint
    self translucentImage: aForm atRect: aPoint sourceRect: aForm boundingBox
].

Canvas ![
translucentImage: aForm atRect: targetRectangle sourceRect: sourceRect
    self translucentImage: aForm atRect: targetRectangle sourceRect: sourceRect color: Color white
].

Canvas ![
translucentImage: aForm atRect: targetRectangle sourceRect: sourceRect color: aColor
    self translucentImage: aForm atRect: targetRectangle sourceRect: sourceRect color: aColor flipX: false flipY: false
].

Canvas ![
translucentImage: aForm atRect: targetRectangle sourceRect: sourceRect color: aColor flipX: flipX flipY: flipY
    self subclassResponsibility
].

Canvas ![
withTranslation: translation do: aBlock
    | oldOrigin |
    oldOrigin := origin.
    origin := origin + translation.
    aBlock ensure: [
        origin := oldOrigin.
    ].
].

Canvas ![
scaleFactor
    ^ scaleFactor
].

Canvas ![
scaleFactor: aPoint
    scaleFactor := aPoint
].

Canvas ![
withScaleFactor: newScaleFactor do: aBlock
    | oldScaleFactor |
    oldScaleFactor := scaleFactor.
    scaleFactor := scaleFactor * newScaleFactor.
    aBlock ensure: [
        scaleFactor := oldScaleFactor.
    ].
].