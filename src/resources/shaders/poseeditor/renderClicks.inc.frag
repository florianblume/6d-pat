vec4 renderClicks(vec3 interpolatedVertex, sampler1D clicks, sampler1D colors, int clickCount, vec4 currentColor) {
    bool isClicked = false;
    bool isAroundClick = false;
    int index = 0;
    float circumfence = 0.2;
    for (int i = 0; i < clickCount; i++)
    {
       float texelNormIndex = (i + 0.5f) / clickCount;
       vec3 delta = abs(interpolatedVertex - texture1D(clicks, texelNormIndex)); // Get delta from middle vec3
       if (pow(delta.r, 2) + pow(delta.g, 2) + pow(delta.b, 2)
               <= pow(circumfence, 2))
       {
           isClicked = true;
           index = i;
       } else if (pow(delta.r, 2) + pow(delta.g, 2) + pow(delta.b, 2)
                  <= pow(circumfence, 2) + 0.1) {
           isAroundClick = true;
           index = i;
       }
       if (isClicked || isAroundClick)
       {
          return texture1D(colors, texelNormIndex);
       }
    }
    return currentColor;
}
