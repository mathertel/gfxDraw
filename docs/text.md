# Text

The gfxDraw library also supports drawing text on a pixel based display.

The text output relies on GFX fonts that uses a structure to define the dimension of a single character
and a small bitmap for the pixels to be set.

There is a simple built-in font available with a character height of 10 and a line height of 12 pixels.
The fonts also can be loaded from disk and added to the internal [Font Manager](#font-manager).

Fonts can be scaled by integer factors.  The text drawing supports only one scaling factor that is applied to the x and
y dimension.

The algorithm is simplified version of the GFX based font support taken from the
[Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) together with the `gfxfont.h` for font
structures GFXglyph and GFXfont according to the used BSD license.

Like the other primitive drawing functions offers the callback mechanism for drawing and is completely driver chip
independant.


## Font definitions

To define a font 2 structures and a bitmap array have to be defined.

The structures and the bitmap are typically created by the font editor so we can assume that e.g.  the bitmap offsets
are in a usable range.

Font Editors are available at <https://tchapi.github.io/Adafruit-GFX-Font-Customiser/> and
<https://gfxfont.netlify.app/>


The `GFXfont` structure holds the overall font attributes and link to further data structures:

* **first** -- first defined character
* **last** -- first defined character
* **yAdvance** -- the vertical distance to the next line
* **bitmap** -- the byte array for the character bitmaps
* **glyph** -- the dimension and offset in the bitmap of the characters

The `GFXglyph` structure defines the dimension and offset in the bitmap of the characters
for one charcter:

* **bitmapOffset** -- Offset where the character starts in GFXfont->bitmap
* **width** -- width of the character bitmap
* **height** -- height of the character bitmap
* **xAdvance** -- horizontal distance to next character
* **xOffset** -- where to place the bitmap of the character in x-direction.
* **yOffset** -- where to place the bitmap of the character in y-direction.

The character pixels are defined in a `const uint8_t __Bitmaps` array and a character always starts on a full byte.
When a character pixels are stored in subsequent bytes and it might happen that the last byte is not used completely.


## Loading fonts

To load an available font from memory the `addFont()` function can be used.

To load an available font in binary format from disk the `loadFont()` function can be used.
Binary font files can be exported using the [ScottFerg56s GFXFontEditor](https://github.com/ScottFerg56/GFXFontEditor)

Adding a font will analyse the dimension of the given font and will register the font in the built-in
[Font Manager](#font-manager).


## Font Manager

The built-in Font Size Manager can hold a vector of font definitions available in memory.  When a new font is added it
is analysed to find the baseline offset and the total height is retrieved.

When a text with a fontsize is printed or the box is calculated the best fitting font will be used by choosing the largest available font
that fits into the given size.  


## See also

* [GFXglyph Struct Reference](https://adafruit.github.io/Adafruit-GFX-Library/html/struct_g_f_xglyph.html)
* [GFXfont Struct Reference](https://adafruit.github.io/Adafruit-GFX-Library/html/struct_g_f_xfont.html)

