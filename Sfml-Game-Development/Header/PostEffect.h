#ifndef POSTEFFECT_H
#define POSTEFFECT_H

namespace sf
{
	class RenderTexture;
	class RenderTarget;
	class Shader;
}

class PostEffect
{
public:
	PostEffect(const PostEffect&) = delete;
	PostEffect& operator=(const PostEffect&) = delete;

	PostEffect();
	virtual ~PostEffect();
	virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output) = 0;

	static bool isSupported();

protected:
	static void applyShader(const sf::Shader& shader, sf::RenderTarget& output);
};

#endif
