# Alesia


![Alesia Logo](alesia.png)

Alesia is light and simple rendering library with no dependencies written in C. Just add the content of the src folder and you're good to go!

The library requires STB_FONT and STB_IMAGE to run. They are directly includes inside the sources.


Features :
- Shape rendering (Union / XOR)
- Unicode font loading / rendering
- Image loading / rendering

TODO :
- Anti-aliasing for Bezier curves (already implemented for lines, but disabled for now)
- Bezier-bezier intersection for a direct polygon operation (instead of appliying operations on lines)
- Gradiant
- Source surface rotation
- Font underline / bold / italic
- Surface export using stb_image_write
