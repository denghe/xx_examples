#pragma once

namespace Battle {

	inline void Drawable::Draw() {
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 1);
		//auto scale = gLooper.camera.scale * (radius * 2 / frame->spriteSize.y);
		q.pos = gLooper.camera.ToGLPos(pos);
		q.anchor = *frame->anchor;
		q.scale = gLooper.camera.scale * scale;
		q.radians = radians;
		q.colorplus = colorplus;
		q.color = color;
		q.texRect.data = frame->textureRect.data;
	}

};
