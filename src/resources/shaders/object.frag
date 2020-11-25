/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#version 140
#ifdef GL_KHR_blend_equation_advanced
#extension GL_ARB_fragment_coord_conventions : enable
#extension GL_KHR_blend_equation_advanced : enable
#endif
#define lowp
#define mediump
#define highp
#line 1

#pragma include phong.inc.frag

in vec3 worldPosition;
uniform vec3 eyePosition;
in vec3 worldNormal;
in vec2 texCoord;
uniform vec4 ka;
uniform sampler2D diffuseTexture;
uniform vec4 diffuse;
uniform vec4 ks;
uniform float shininess;

#line 12
in vec3 interpolatedVertex;
uniform vec3 clicks[10];
uniform vec3 colors[10];
uniform int clickCount;
uniform bool useDiffuseTexture;
uniform float circumfence;
uniform vec4 selected;
uniform float opacity;

#line 18
out vec4 fragColor;

void main()
{
    vec4 finalDiffuse;
    if (useDiffuseTexture) {
        finalDiffuse = texture2D(diffuseTexture, texCoord);
    } else {
        finalDiffuse = diffuse;
    }
    vec4 currentColor = phongFunction(ka, finalDiffuse, ks, shininess, worldPosition, normalize(((eyePosition - worldPosition))), normalize(worldNormal));

    currentColor -= selected;
    currentColor = vec4(vec3(currentColor), opacity);

    bool isClicked = false;
    bool isAroundClick = false;
    int index = 0;

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
       if (isClicked)
       {
          currentColor = vec4(colors[i], opacity);
          break;
       }
    }

    fragColor = currentColor;
}
