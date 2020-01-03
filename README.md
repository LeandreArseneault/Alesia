# Alesia

Alesia is light and simple rendering library with no dependencies written in C.
 <br>
 <br>
 <br>



<p align="center">
<img src="alesia.png" >
</p>

<br>

To use it just add the content of the src folder and you're good to go!
The library requires STB_FONT and STB_IMAGE to run. They are directly included inside the sources. We recommend using a library like SDL for the windowing.


Features :
- Shape rendering (Union / XOR)
- Unicode font loading / rendering (ttf)
- Image loading / rendering (png,jpg,jpeg...)


Here is an example of what the library can do :


<p align="center">
<img src="res/github_alesia.png" >
</p>





TODO :
- Anti-aliasing for Bezier curves (already implemented for lines, but disabled for now)
- Bezier-bezier intersection for a direct polygon operation (instead of appliying operations on lines)
- Gradiant
- Shape stroke
- Source surface rotation
- Font underline / bold / italic
- Surface export using stb_image_write
- Optional alpha on surface
