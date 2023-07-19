#version 330 core

out vec4 FragColor;
in vec4 outCol;
in vec2 outTex;
uniform sampler2D tex;
uniform int isZapper;
uniform int isFlying;
uniform int needsTexture;

void main()
{
   if(needsTexture==1){
      vec4 texCol = texture(tex, outTex);
      if (texCol.a < 1){
         discard;
      }
      FragColor = texCol;
   }
   else{
      vec4 texCol = texture(tex, outTex);
      if(texCol.a<1){
         discard;
      }
      if(isZapper == 1){
         float pos_ndc = (2.0 * outTex.x - 1);
         float dist = length(pos_ndc);

         vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
         vec4 curr = vec4(outCol.x, outCol.y, outCol.z, 1.0);

         vec4 color = mix(curr, black, smoothstep(0.0, 1.0, dist));
         FragColor = texCol * color;
      }

      else{
         if(isFlying==1){
            vec2 pos_ndc = 0.6*(2*outTex - 1);
            float dist = length(pos_ndc);

            vec4 black = vec4(1.0, 0.0, 1.0, 1.0);
            vec4 curr = vec4(outCol.x, outCol.y, outCol.z, 1.0);

            vec4 color = mix(curr, black, smoothstep(0.0, 1.0, dist));
            FragColor = texCol * color;
         }
         else{
            // FragColor = outCol;
            FragColor = texCol;
         }
      }
   }
}