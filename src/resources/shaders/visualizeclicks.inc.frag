vec4 visualizeClicks(vec4 currentColor, vec3 interpolatedVertex,
                     int clickCount, vec3 clicks[10], vec3 clickColors[10],
                     float clickDiameter) {
    int index = 0;

    for (int i = 0; i < clickCount; i++) {
       vec3 click = clicks[i];
       vec3 delta = abs(interpolatedVertex - click); // Get delta from middle vec3
       if (pow(delta.r, 2) + pow(delta.g, 2) + pow(delta.b, 2)
               <= clickDiameter) {
           // Doesn't matter what alpha we use -> clicks are visualized on a plane in the pose viewer
           // not on the objects themselves and the pose viewer is the only place where we apply
           // transparency (pose editor visualizes the clicks on the objects but doesn't support transparency)
           currentColor = vec4(clickColors[i], 1.0);
           break;
       }
    }

    return currentColor;
}
