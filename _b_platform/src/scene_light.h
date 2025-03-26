#pragma once


namespace xx {

    struct Shader_QuadInstanceLight : Shader {
        using Shader::Shader;
        GLint uCxy{ -1 }, uTex0{ -1 }, uTex1{ -1 }, aVert{ -1 }, aPosAnchor{ -1 }, aScaleRadiansColorplus{ -1 }, aColor{ -1 }, aTexRect{ -1 };
        GLVertexArrays va;
        GLBuffer vb, ib;

        static constexpr int32_t maxQuadNums{ 100000 };
        GLuint lastTextureId{}, lastLightTextureId{};
        std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique_for_overwrite<QuadInstanceData[]>(maxQuadNums);
        int32_t quadCount{};
        EngineBase0* eb{};

        void Init(EngineBase0* eb_) {
            eb = eb_;

            v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uCxy;	// screen center coordinate

in vec2 aVert;	// fans index { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec4 aPosAnchor;
in vec4 aScaleRadiansColorplus;
in vec4 aColor;
in vec4 aTexRect;

out vec2 vTexCoord;
out float vColorplus;
out vec4 vColor;

void main() {
    vec2 pos = aPosAnchor.xy;
    vec2 anchor = aPosAnchor.zw;
    vec2 scale = vec2(aScaleRadiansColorplus.x * aTexRect.z, aScaleRadiansColorplus.y * aTexRect.w);
    float radians = aScaleRadiansColorplus.z;
    vec2 offset = vec2((aVert.x - anchor.x) * scale.x, (aVert.y - anchor.y) * scale.y);

    float c = cos(radians);
    float s = sin(radians);
    vec2 v = pos + vec2(
       dot(offset, vec2(c, s)),
       dot(offset, vec2(-s, c))
    );

    gl_Position = vec4(v * uCxy, 0, 1);
    vColor = aColor;
    vColorplus = aScaleRadiansColorplus.w;
    vTexCoord = vec2(aTexRect.x + aVert.x * aTexRect.z, aTexRect.y + aTexRect.w - aVert.y * aTexRect.w);
})"sv });

            f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;          // mediump draw border has issue
uniform sampler2D uTex0;
uniform sampler2D uTex1;

in vec2 vTexCoord;
in float vColorplus;
in vec4 vColor;

out vec4 oColor;

void main() {
    vec4 c = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
    vec4 c1 = texture(uTex1, vTexCoord / vec2(textureSize(uTex1, 0)));
#if 1
    oColor = vec4( (c.x + 0.00001f) * vColorplus * c1.x
                 , (c.y + 0.00001f) * vColorplus * c1.y
                 , (c.z + 0.00001f) * vColorplus * c1.z, c.w );
#else
    oColor = vec4( (c.x + 0.00001f) * vColorplus
                 , (c.y + 0.00001f) * vColorplus
                 , (c.z + 0.00001f) * vColorplus, c.w );
#endif
})"sv });

            p = LinkGLProgram(v, f);

            uCxy = glGetUniformLocation(p, "uCxy");
            uTex0 = glGetUniformLocation(p, "uTex0");
            uTex1 = glGetUniformLocation(p, "uTex1");

            aVert = glGetAttribLocation(p, "aVert");
            aPosAnchor = glGetAttribLocation(p, "aPosAnchor");
            aScaleRadiansColorplus = glGetAttribLocation(p, "aScaleRadiansColorplus");
            aColor = glGetAttribLocation(p, "aColor");
            aTexRect = glGetAttribLocation(p, "aTexRect");
            CheckGLError();

            glGenVertexArrays(1, va.GetValuePointer());
            glBindVertexArray(va);

            glGenBuffers(1, (GLuint*)&ib);
            static const XY verts[4] = { { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f } };
            glBindBuffer(GL_ARRAY_BUFFER, ib);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, sizeof(XY), 0);
            glEnableVertexAttribArray(aVert);

            glGenBuffers(1, (GLuint*)&vb);
            glBindBuffer(GL_ARRAY_BUFFER, vb);

            glVertexAttribPointer(aPosAnchor, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), 0);  // offsetof(QuadInstanceData, pos
            glVertexAttribDivisor(aPosAnchor, 1);
            glEnableVertexAttribArray(aPosAnchor);

            glVertexAttribPointer(aScaleRadiansColorplus, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, scale));
            glVertexAttribDivisor(aScaleRadiansColorplus, 1);
            glEnableVertexAttribArray(aScaleRadiansColorplus);

            glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, color));
            glVertexAttribDivisor(aColor, 1);
            glEnableVertexAttribArray(aColor);

            glVertexAttribPointer(aTexRect, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, texRect));
            glVertexAttribDivisor(aTexRect, 1);
            glEnableVertexAttribArray(aTexRect);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            CheckGLError();
        }

        virtual void Begin() override {
            assert(!eb->shader);
            glUseProgram(p);
            glActiveTexture(GL_TEXTURE0 + 0);
            glActiveTexture(GL_TEXTURE0 + 1);
            glUniform1i(uTex0, 0);
            glUniform1i(uTex1, 1);
            glUniform2f(uCxy, 2 / eb->windowSize.x, 2 / eb->windowSize.y * eb->flipY);
            glBindVertexArray(va);
        }

        virtual void End() override {
            assert(eb->shader == this);
            if (quadCount) {
                Commit();
            }
        }

        void Commit() {
            glBindBuffer(GL_ARRAY_BUFFER, vb);
            glBufferData(GL_ARRAY_BUFFER, sizeof(QuadInstanceData) * quadCount, quadInstanceDatas.get(), GL_STREAM_DRAW);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, lastTextureId);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, lastLightTextureId);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, quadCount);

            CheckGLError();

            drawVerts += quadCount * 6;
            drawCall += 1;

            lastTextureId = 0;
            lastLightTextureId = 0;
            quadCount = 0;
        }

        QuadInstanceData* Draw(GLuint texId, GLuint lightTexId, int32_t numQuads) {
            assert(eb->shader == this);
            assert(numQuads <= maxQuadNums);
            if (quadCount + numQuads > maxQuadNums
                || (lastTextureId && lastTextureId != texId)
                || (lastLightTextureId && lastLightTextureId != lightTexId)
                ) {
                Commit();
            }
            lastTextureId = texId;
            lastLightTextureId = lightTexId;
            auto r = &quadInstanceDatas[quadCount];
            quadCount += numQuads;
            return r;
        }

        void Draw(GLuint texId, GLuint lightTexId, QuadInstanceData const& qv) {
            memcpy(Draw(texId, lightTexId, 1), &qv, sizeof(QuadInstanceData));
        }
    };
}


namespace Light {

	struct Scene : xx::SceneBase {
		static constexpr int32_t tileSize{ 128 };
		xx::Ref<xx::GLTexture> tex_tiles;
		std::array<xx::UVRect, 6> rect_tiles_1x1;
		std::array<xx::UVRect, 2> rect_tiles_1x2;
		std::array<xx::UVRect, 3> rect_tiles_2x1;
		std::array<xx::UVRect, 3> rect_tiles_2x2;

		xx::Ref<xx::GLTexture> tex_light;

		int32_t lightCreateAvaliableFrameNumber{};
		xx::Listi32<XYi> lightPoss;
		xx::Shader_QuadInstanceLight shaderLightTex;

		xx::FrameBuffer fb;
		void DrawTile(int32_t cx, int32_t cy, xx::UVRect const& uv);
		void DrawTex(xx::Ref<xx::GLTexture> const& tex);

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
