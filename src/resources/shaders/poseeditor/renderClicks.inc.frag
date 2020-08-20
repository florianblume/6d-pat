vec4 renderClicks(vec3 interpolatedVertex, vec3[] clicks, vec3[] colors, int clickCount, vec4 currentColor) {
    bool isClicked = false;
    bool isAroundClick = false;
    int index = 0;
    float circumfence = 0.2;
    for (int i = 0; i < clickCount; i++)
    {
       vec3 click = clicks[i];
       vec3 delta = abs(interpolatedVertex - click); // Get delta from middle vec3
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
          return vec4(colors[i], 1.0);
       } else {
           return currentColor;
       }
    }
    return currentColor;
}
