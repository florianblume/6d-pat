vec4 renderClicks(vec3 interpolatedVertex, vec3 clicks[10], vec3 colors[10], vec4 currentColor) {
    bool isClicked = false;
    bool isAroundClick = false;
    int index = 0;
    float circumfence = 0.2;
    for (int i = 0; i < 10; i++)
    {
       vec3 delta = abs(interpolatedVertex - clicks[i]); // Get delta from middle vec3
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
    }
    if (isClicked || isAroundClick)
    {
       return vec4(colors[index], 1.0);
    }
    else
    {
       return currentColor;
    }
}
