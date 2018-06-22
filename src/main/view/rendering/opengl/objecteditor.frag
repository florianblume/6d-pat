#version 130
in highp vec3 vert;
in highp vec3 vertNormal;
uniform highp vec3 lightPos;
uniform highp vec4 segmentationColor;
uniform highp vec3 clickPositions[10];
uniform highp vec3 colorsOfClicks[10];
uniform int clickCount;

void main() {
           highp vec3 L = normalize(lightPos - vert);
           highp float NL = max(dot(normalize(vertNormal), L), 0.0);
           highp vec3 color = vec3(0.7, 0.7, 0.7);
           highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
           if (clickCount > 0) {
               bool isClicked = false;
               int index = 0;
               for (int i = 0; i < clickCount; i++)
               {
                   vec3 delta = abs(vert - clickPositions[i]); // Get delta from middle vec3
                   if ((delta.r <= 0.35) && (delta.g <= 0.35) && (delta.b <= 0.35))
                   {
                       isClicked = true;
                       index = i;
                   }
               }
               if (isClicked)
               {
                   gl_FragData[0] = vec4(colorsOfClicks[index], 1.0);
               }
               else
               {
                   gl_FragData[0] = vec4(col, 1.0);
               }
           } else {
               gl_FragData[0] = vec4(col, 1.0);
           }
           gl_FragData[1] = segmentationColor;
}
